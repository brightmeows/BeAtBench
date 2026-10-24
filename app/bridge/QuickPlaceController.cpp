// SPDX-License-Identifier: GPL-3.0-only
// QuickPlaceController 实现：选中集快照 / LN 逻辑对象 / 碰撞与隐藏预检 / 单步移动。
#include "bridge/QuickPlaceController.hpp"

#include <QEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QVariantMap>
#include <algorithm>
#include <set>

#include "beatbench/core/ChartMode.hpp"
#include "beatbench/core/edit/EditorSession.hpp"
#include "beatbench/core/edit/SessionRegistry.hpp"
#include "bridge/ChartSession.hpp"

namespace beatbench::app {

namespace {

using beatbench::Lane;
using beatbench::LaneKind;
using beatbench::Rational;
using beatbench::edit::NoteRef;

QString laneKey(const Lane& lane) {
    const char* kind = lane.kind == LaneKind::Scratch ? "scratch"
                       : lane.kind == LaneKind::Pedal ? "pedal"
                       : lane.kind == LaneKind::Bgm   ? "bgm"
                                                      : "key";
    return QStringLiteral("lane:%1:%2:%3")
        .arg(static_cast<int>(lane.player))
        .arg(QString::fromLatin1(kind))
        .arg(static_cast<int>(lane.index));
}

QString laneLabel(const Lane& lane) {
    if (lane.kind == LaneKind::Scratch)
        return lane.player == 0 ? QStringLiteral("皿") : QStringLiteral("2P·皿");
    const QString key = QStringLiteral("键%1").arg(static_cast<int>(lane.index));
    return lane.player == 0 ? key : QStringLiteral("2P·") + key;
}

QVariantMap refToMap(const NoteRef& ref) {
    QVariantMap map;
    map.insert(QStringLiteral("measure"), static_cast<int>(ref.measure));
    QVariantMap pos;
    pos.insert(QStringLiteral("num"), static_cast<qlonglong>(ref.pos.num));
    pos.insert(QStringLiteral("den"), static_cast<qlonglong>(ref.pos.den));
    map.insert(QStringLiteral("pos"), pos);
    QVariantMap lane;
    lane.insert(QStringLiteral("player"), static_cast<int>(ref.lane.player));
    const char* kind = ref.lane.kind == LaneKind::Scratch ? "scratch"
                       : ref.lane.kind == LaneKind::Pedal ? "pedal"
                       : ref.lane.kind == LaneKind::Bgm   ? "bgm"
                                                          : "key";
    lane.insert(QStringLiteral("kind"), QString::fromLatin1(kind));
    lane.insert(QStringLiteral("index"), static_cast<int>(ref.lane.index));
    map.insert(QStringLiteral("lane"), lane);
    map.insert(QStringLiteral("sample"), static_cast<int>(ref.sample));
    map.insert(QStringLiteral("sub_line"), static_cast<int>(ref.sub_line));
    return map;
}

}  // namespace

QuickPlaceController::QuickPlaceController(QObject* parent) : QObject(parent) {
    QSettings settings;
    m_preset = settings.value(QStringLiteral("quickplace/preset"), 0).toInt() == 1 ? 1 : 0;
}

void QuickPlaceController::setChartSession(ChartSession* session) { m_session = session; }

void QuickPlaceController::setEnabled(bool enabled) { m_enabled = enabled; }

void QuickPlaceController::setInputBlocked(bool blocked) { m_inputBlocked = blocked; }

void QuickPlaceController::setHiddenLanes(const QStringList& keys) { m_hiddenLanes = keys; }

QString QuickPlaceController::mode() const {
    if (!m_session || !m_session->chart()) return {};
    return QString::fromStdString(m_session->chart()->mode_id.value_or("sp7k"));
}

bool QuickPlaceController::isPms() const { return mode().startsWith(QStringLiteral("pms9k")); }

bool QuickPlaceController::isDoublePlay() const {
    const QString m = mode();
    return m == QStringLiteral("dp") || m == QStringLiteral("battle");
}

void QuickPlaceController::setPreset(int preset) {
    const int v = preset == 1 ? 1 : 0;
    if (v == m_preset) return;
    m_preset = v;
    QSettings settings;
    settings.setValue(QStringLiteral("quickplace/preset"), m_preset);
    emit presetChanged();
    emit statusMessage(m_preset == 0 ? tr("摆音键位：单手换边") : tr("摆音键位：双手固定"));
}

void QuickPlaceController::cancel() {
    m_selection.clear();
    m_queue.clear();
    m_index = 0;
    m_lastSignature.clear();
    emit pendingCountChanged();
}

QString QuickPlaceController::signature(const std::vector<NoteRef>& refs) const {
    std::vector<NoteRef> sorted = refs;
    std::sort(sorted.begin(), sorted.end());
    QStringList parts;
    parts.reserve(static_cast<int>(sorted.size()));
    for (const auto& r : sorted) {
        parts.push_back(QStringLiteral("%1|%2/%3|%4|%5|%6|%7|%8")
                            .arg(r.measure)
                            .arg(static_cast<qlonglong>(r.pos.num))
                            .arg(static_cast<qlonglong>(r.pos.den))
                            .arg(static_cast<int>(r.lane.player))
                            .arg(static_cast<int>(r.lane.kind))
                            .arg(static_cast<int>(r.lane.index))
                            .arg(r.sample)
                            .arg(r.sub_line));
    }
    return parts.join(QLatin1Char(';'));
}

std::vector<NoteRef> QuickPlaceController::readSelection(const QVariantList& list) const {
    std::vector<NoteRef> refs;
    refs.reserve(static_cast<std::size_t>(list.size()));
    for (const auto& item : list) {
        const QVariantMap m = item.toMap();
        if (m.isEmpty()) continue;
        NoteRef ref;
        ref.measure = static_cast<std::uint32_t>(m.value(QStringLiteral("measure")).toInt());
        const QVariantMap pos = m.value(QStringLiteral("pos")).toMap();
        ref.pos = Rational(pos.value(QStringLiteral("num")).toLongLong(),
                           pos.value(QStringLiteral("den")).toLongLong());
        const QVariantMap lane = m.value(QStringLiteral("lane")).toMap();
        ref.lane.player = static_cast<std::uint8_t>(lane.value(QStringLiteral("player")).toInt());
        const QString kind = lane.value(QStringLiteral("kind")).toString();
        ref.lane.kind = kind == QStringLiteral("scratch") ? LaneKind::Scratch
                        : kind == QStringLiteral("pedal") ? LaneKind::Pedal
                        : kind == QStringLiteral("bgm")   ? LaneKind::Bgm
                                                          : LaneKind::Key;
        ref.lane.index = static_cast<std::uint8_t>(lane.value(QStringLiteral("index")).toInt());
        ref.sample = static_cast<std::uint32_t>(m.value(QStringLiteral("sample")).toInt());
        ref.sub_line = static_cast<std::uint32_t>(m.value(QStringLiteral("sub_line")).toInt());
        refs.push_back(ref);
    }
    return refs;
}

QVariantList QuickPlaceController::toVariant(const std::vector<NoteRef>& refs) const {
    QVariantList out;
    out.reserve(static_cast<int>(refs.size()));
    for (const auto& r : refs) out.push_back(refToMap(r));
    return out;
}

std::optional<std::size_t> QuickPlaceController::findNote(const NoteRef& ref) const {
    if (!m_session || !m_session->chart()) return std::nullopt;
    const auto& notes = m_session->chart()->notes;
    for (std::size_t i = 0; i < notes.size(); ++i) {
        const auto& e = notes[i];
        if (e.measure == ref.measure && e.pos == ref.pos && e.value.lane == ref.lane &&
            e.value.sample.id == ref.sample && e.value.sub_line == ref.sub_line)
            return i;
    }
    return std::nullopt;
}

void QuickPlaceController::rebuildFromSelection(const std::vector<NoteRef>& input) {
    if (!m_session || !m_session->chart()) {
        cancel();
        return;
    }
    if (input.empty()) {
        cancel();
        return;
    }
    const QString inputSig = signature(input);
    if (inputSig == m_lastSignature) return;  // 控制器自己回写的 selection：保留队列指针

    std::set<NoteRef> normalized;
    for (const auto& ref : input) {
        const auto idx = findNote(ref);
        if (!idx) continue;
        normalized.insert(ref);
        const auto& note = m_session->chart()->notes[*idx].value;
        if (note.ln_pair && *note.ln_pair < m_session->chart()->notes.size()) {
            const auto& partnerEv = m_session->chart()->notes[*note.ln_pair];
            normalized.insert(NoteRef{partnerEv.measure, partnerEv.pos, partnerEv.value.lane,
                                      partnerEv.value.sample.id, partnerEv.value.sub_line});
        }
    }

    std::vector<NoteRef> refs(normalized.begin(), normalized.end());
    m_selection = refs;
    m_queue.clear();
    m_index = 0;
    std::set<NoteRef> used;
    for (const auto& ref : refs) {
        if (used.count(ref)) continue;
        const auto idx = findNote(ref);
        if (!idx) continue;
        const auto& note = m_session->chart()->notes[*idx].value;
        Item item;
        item.a = ref;
        if (note.ln_pair && *note.ln_pair < m_session->chart()->notes.size()) {
            const auto& partnerEv = m_session->chart()->notes[*note.ln_pair];
            const NoteRef partner{partnerEv.measure, partnerEv.pos, partnerEv.value.lane,
                                  partnerEv.value.sample.id, partnerEv.value.sub_line};
            if (!used.count(partner)) {
                item.b = partner;
                item.ln = true;
                used.insert(partner);
            }
        }
        used.insert(ref);
        m_queue.push_back(std::move(item));
    }
    m_lastSignature = signature(m_selection);
    emit pendingCountChanged();
    if (inputSig != m_lastSignature) emitSelection();
}

void QuickPlaceController::emitSelection() {
    m_lastSignature = signature(m_selection);
    emit selectionUpdated(toVariant(m_selection));
}

void QuickPlaceController::setSelection(const QVariantList& list) {
    rebuildFromSelection(readSelection(list));
}

bool QuickPlaceController::targetLaneValid(const Lane& lane) const {
    if (!m_session || !m_session->chart()) return false;
    const QString m = mode();
    if (lane.kind == LaneKind::Scratch) return !isPms() && lane.player <= 1 && lane.index == 0;
    if (lane.kind != LaneKind::Key) return false;
    if (lane.player > 1) return false;
    if (isPms() && lane.player != 0) return false;
    const int maxKey = beatbench::chart_mode_key_count(m.toStdString());
    return lane.index >= 1 && lane.index <= maxKey;
}

bool QuickPlaceController::targetHidden(const Lane& lane) const {
    return m_hiddenLanes.contains(laneKey(lane));
}

bool QuickPlaceController::collisionAt(const beatbench::Event<beatbench::Note>& source,
                                       const Lane& target, std::size_t sourceIndex,
                                       const std::optional<std::size_t>& partnerIndex) const {
    const auto& notes = m_session->chart()->notes;
    for (std::size_t i = 0; i < notes.size(); ++i) {
        if (i == sourceIndex || (partnerIndex && i == *partnerIndex)) continue;
        const auto& e = notes[i];
        if (e.measure == source.measure && e.pos == source.pos && e.value.lane == target)
            return true;
    }
    return false;
}

void QuickPlaceController::placeLane(const Lane& lane) {
    if (!m_session || !m_session->chart() || !hasPending()) {
        emit statusMessage(tr("没有待摆音符：先选中 note"));
        return;
    }
    if (!targetLaneValid(lane)) {
        emit statusMessage(tr("目标轨在当前模式不存在：%1").arg(laneLabel(lane)));
        return;
    }
    if (targetHidden(lane)) {
        emit statusMessage(tr("目标轨已隐藏：先恢复 %1 显示").arg(laneLabel(lane)));
        return;
    }

    const Item& item = m_queue[m_index];
    const auto sourceIndex = findNote(item.a);
    if (!sourceIndex) {
        cancel();
        emit statusMessage(tr("选中的 note 已不存在，摆音队列已重置"));
        return;
    }
    const auto& chart = *m_session->chart();
    const auto& source = chart.notes[*sourceIndex];
    std::optional<std::size_t> partnerIndex;
    if (item.ln) {
        if (!source.value.ln_pair || *source.value.ln_pair >= chart.notes.size()) {
            cancel();
            emit statusMessage(tr("LN 配对已失效，摆音队列已重置"));
            return;
        }
        partnerIndex = *source.value.ln_pair;
    }
    if (collisionAt(source, lane, *sourceIndex, partnerIndex)) {
        emit statusMessage(
            tr("目标位置已有 note：%1 拍位 %2").arg(laneLabel(lane)).arg(source.measure));
        return;
    }
    if (partnerIndex && collisionAt(chart.notes[*partnerIndex], lane, *sourceIndex, partnerIndex)) {
        emit statusMessage(tr("目标位置已有 note：%1 拍位 %2")
                               .arg(laneLabel(lane))
                               .arg(chart.notes[*partnerIndex].measure));
        return;
    }

    const std::uint32_t sample = source.value.sample.id;
    const std::uint32_t fromMeasure = source.measure;
    const Rational fromPos = source.pos;
    const Lane fromLane = source.value.lane;
    const std::uint32_t fromSub = source.value.sub_line;
    std::optional<beatbench::Event<beatbench::Note>> partnerSnapshot;
    if (partnerIndex) partnerSnapshot = chart.notes[*partnerIndex];
    std::unique_ptr<beatbench::edit::EditCommand> cmd;
    if (item.ln)
        cmd = std::make_unique<beatbench::edit::MoveLnPairCommand>(fromMeasure, fromPos, fromLane,
                                                                   sample, fromSub, lane);
    else
        cmd = std::make_unique<beatbench::edit::MoveNoteCommand>(
            fromMeasure, fromPos, fromLane, sample, fromMeasure, fromPos, false, lane, fromSub,
            std::nullopt);
    auto& session = beatbench::edit::session_registry().active();
    if (!session.exec(std::move(cmd))) {
        emit statusMessage(tr("摆音失败：命令未应用"));
        return;
    }

    std::erase_if(m_selection,
                  [&](const NoteRef& r) { return r == item.a || (item.b && r == *item.b); });
    m_selection.push_back(NoteRef{fromMeasure, fromPos, lane, sample, 0});
    if (partnerSnapshot) {
        m_selection.push_back(NoteRef{partnerSnapshot->measure, partnerSnapshot->pos, lane,
                                      partnerSnapshot->value.sample.id, 0});
    }
    std::sort(m_selection.begin(), m_selection.end());
    m_selection.erase(std::unique(m_selection.begin(), m_selection.end()), m_selection.end());
    ++m_index;
    emit pendingCountChanged();
    emitSelection();
    if (m_session) m_session->refresh();
    emit edited();
    emit statusMessage(
        tr("已摆 #WAV%1 → %2（剩余 %3）").arg(sample).arg(laneLabel(lane)).arg(pendingCount()));
}

std::optional<Lane> QuickPlaceController::laneForKey(int key, int effectiveSide) const {
    static constexpr int kRow7[7] = {Qt::Key_Z, Qt::Key_S, Qt::Key_X, Qt::Key_D,
                                     Qt::Key_C, Qt::Key_F, Qt::Key_V};
    static constexpr int kPms9[9] = {Qt::Key_Z, Qt::Key_S, Qt::Key_X, Qt::Key_D, Qt::Key_C,
                                     Qt::Key_F, Qt::Key_V, Qt::Key_G, Qt::Key_H};
    static constexpr int kPms9Two[9] = {Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G,
                                        Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L};
    static constexpr int kRow2[7] = {Qt::Key_H,        Qt::Key_J,         Qt::Key_K,
                                     Qt::Key_L,        Qt::Key_Semicolon, Qt::Key_Apostrophe,
                                     Qt::Key_Backslash};

    if (isPms()) {
        const int* keys = m_preset == 1 ? kPms9Two : kPms9;
        for (int i = 0; i < 9; ++i) {
            if (key == keys[i]) return Lane{0, LaneKind::Key, static_cast<std::uint8_t>(i + 1)};
        }
        return std::nullopt;
    }

    for (int i = 0; i < 7; ++i) {
        if (key == kRow7[i])
            return Lane{static_cast<std::uint8_t>(effectiveSide), LaneKind::Key,
                        static_cast<std::uint8_t>(i + 1)};
    }
    if (key == Qt::Key_A)
        return Lane{static_cast<std::uint8_t>(effectiveSide), LaneKind::Scratch, 0};

    if (m_preset == 1 && isDoublePlay()) {
        for (int i = 0; i < 7; ++i) {
            if (key == kRow2[i]) return Lane{1, LaneKind::Key, static_cast<std::uint8_t>(i + 1)};
        }
        if (key == Qt::Key_G) return Lane{1, LaneKind::Scratch, 0};
    }
    return std::nullopt;
}

bool QuickPlaceController::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);
    if (event->type() != QEvent::KeyPress || !m_enabled || m_inputBlocked || !m_session ||
        !m_session->chart())
        return false;
    auto* ke = static_cast<QKeyEvent*>(event);
    if (ke->isAutoRepeat()) return false;
    if (ke->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) return false;

    const bool dp = isDoublePlay();
    if (m_preset == 0 && dp && ke->key() == Qt::Key_G) {
        m_side = m_side == 0 ? 1 : 0;
        emit sideChanged();
        emit statusMessage(m_side == 0 ? tr("摆音目标：1P") : tr("摆音目标：2P"));
        return true;
    }
    const int effectiveSide =
        (m_preset == 0 && dp && (ke->modifiers() & Qt::ShiftModifier)) ? 1 - m_side : m_side;
    const auto lane = laneForKey(ke->key(), effectiveSide);
    if (!lane) return false;
    if (!hasPending()) return false;
    placeLane(*lane);
    return true;
}

}  // namespace beatbench::app
