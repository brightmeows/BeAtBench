// SPDX-License-Identifier: GPL-3.0-only
// SliceWorkspace 实现（见 hpp 注释）。线程编排：
// - loadAudioFile（UI）→ QThreadPool（**局部 ReferenceTrack**：解码 + 金字塔构建；
//   宽字符路径，Windows）→ 回 UI 线程：move 进 m_track → AudioEngine::setReferencePcm；
// - loadMidiFile 同步（core 解析快）；失败/成功都 statusText 提示；
// - offset 仅数据（画 note 刻度/列表显示时 +offsetSec）；切片边界推导（M6.2）再消费。
#include "bridge/SliceWorkspace.hpp"

#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMetaObject>
#include <QThreadPool>
#include <QVariantMap>

#include <algorithm>
#include <set>

#include "bridge/AudioEngine.hpp"
#include "bridge/ChartSession.hpp"
#include "beatbench/audio/ChartRenderer.hpp"
#include "beatbench/core/slice/SliceExport.hpp"

namespace beatbench::app {

namespace {

/// 当前谱面已占用的 #WAV id（数值；无 chart → 空）。
std::vector<std::uint32_t> occupied_wav_ids(const ChartSession* session) {
    std::vector<std::uint32_t> out;
    if (!session || !session->chart()) return out;
    for (const auto& [key, def] : session->chart()->samples) {
        if (key.first == SampleKind::Wav) out.push_back(key.second);
    }
    return out;
}

QVariantMap pyramid_info(const beatbench::audio::WaveformPyramid* p) {
    QVariantMap m;
    if (!p || !p->valid()) {
        m.insert(QStringLiteral("valid"), false);
        return m;
    }
    m.insert(QStringLiteral("valid"), true);
    m.insert(QStringLiteral("frames"),
             static_cast<qlonglong>(p->frameCount()));
    m.insert(QStringLiteral("sampleRate"), p->sampleRate());
    m.insert(QStringLiteral("durationSec"),
             static_cast<double>(p->frameCount()) / p->sampleRate());
    return m;
}

}  // namespace

SliceWorkspace::SliceWorkspace(QObject* parent) : QObject(parent) {}

void SliceWorkspace::setAudioEngine(AudioEngine* engine) { m_engine = engine; }

void SliceWorkspace::setChartSession(ChartSession* session) { m_chartSession = session; }

QVariantList SliceWorkspace::occupiedWavIds() const {
    QVariantList out;
    for (const std::uint32_t id : occupied_wav_ids(m_chartSession)) out.append(id);
    return out;
}

int SliceWorkspace::nextFreeWavId() const {
    std::set<std::uint32_t> taken(occupied_wav_ids(m_chartSession).begin(),
                                  occupied_wav_ids(m_chartSession).end());
    std::uint32_t cand = 1;
    while (taken.count(cand)) ++cand;
    return static_cast<int>(cand);
}

int SliceWorkspace::suggestedStartMeasure() const {
    // 下一空小节（1-based）：谱面已有小节数 + 1；无谱面 → 第 1 小节。
    // 夹逼 [1,999]（文件 3 位小节号上限）。
    if (!m_chartSession || !m_chartSession->chart()) return 1;
    const int n = m_chartSession->measureCount();
    return std::clamp(n + 1, 1, 999);
}

QVariantMap SliceWorkspace::exportSlices(qreal bpm, int subdivision,
                                         int beatsPerMeasure, int startId,
                                         int startMeasure,
                                         const QString& outDir, const QString& prefix,
                                         qreal fadeMs) {
    QVariantMap res;
    res.insert(QStringLiteral("ok"), false);
    if (m_slices.empty()) {
        res.insert(QStringLiteral("error"), QStringLiteral("无切片可导出"));
        return res;
    }
    if (!m_track.valid()) {
        res.insert(QStringLiteral("error"), QStringLiteral("无参考音频（无法分片导出）"));
        return res;
    }
    if (outDir.isEmpty()) {
        res.insert(QStringLiteral("error"), QStringLiteral("输出目录为空"));
        return res;
    }
    const QString baseName = prefix.isEmpty() ? QStringLiteral("slice") : prefix;
    // 防御：起始 id / 起始小节 夹逼到合法域（QML 侧异常输入不得进 core）
    const int safeStartId = std::clamp(startId, 1, 1295);
    const int safeStartMeasure = std::clamp(startMeasure, 1, 999);
    qWarning("slice export: begin id=%d measure=%d slices=%zu enabled=%zu",
             safeStartId, safeStartMeasure, m_slices.size(), m_sliceEnabled.size());

    const auto items = slice::build_export_layout(
        m_slices, m_sliceEnabled, occupied_wav_ids(m_chartSession),
        static_cast<std::uint32_t>(safeStartId), baseName.toStdString(), bpm,
        beatsPerMeasure, subdivision, m_offsetSec, safeStartMeasure);
    qWarning("slice export: layout ok (%zu items)", items.size());

    // 输出：<outDir>/<prefix>_<NNN>.wav；prefix 含 `/` 或 `\` 时建对应子目录
    const int lastSlash =
        std::max(baseName.lastIndexOf(QLatin1Char('/')), baseName.lastIndexOf(QLatin1Char('\\')));
    const QString prefixDir = (lastSlash >= 0) ? baseName.left(lastSlash) : QString();
    if (!prefixDir.isEmpty())
        QDir().mkpath(QDir(outDir).filePath(prefixDir));
    const double sr = m_track.sampleRate();
    const double fade = fadeMs / 1000.0;
    int written = 0;
    QStringList errors;
    for (const auto& it : items) {
        if (!it.enabled || it.wavId == 0) continue;
        if (it.sliceIndex < 0 || it.sliceIndex >= static_cast<int>(m_slices.size()))
            continue;
        const auto& s = m_slices[static_cast<std::size_t>(it.sliceIndex)];
        const QString outPath = QDir(outDir).filePath(QString::fromStdString(it.fileName));
        auto pcm = m_track.window(s.startSec, s.endSec);
        if (pcm.empty()) {
            errors << outPath + QStringLiteral(": 空窗口");
            continue;
        }
        beatbench::audio::ReferenceTrack::apply_slice_fade(pcm, sr, fade);
        beatbench::audio::RenderedAudio ra;
        ra.sampleRate = sr;
        ra.interleavedStereo = std::move(pcm);
        std::string msg;
        const bool ok = beatbench::audio::write_wav_file_w(outPath.toStdWString(), ra, &msg);
        if (!ok) {
            errors << outPath + QStringLiteral(": ") + QString::fromStdString(msg);
            continue;
        }
        ++written;
    }

    const std::string rawStr = slice::build_placement_raw(items, bpm, beatsPerMeasure);
    qWarning("slice export: wrote=%d raw_chars=%zu", written, rawStr.size());
    res.insert(QStringLiteral("ok"), errors.isEmpty());
    res.insert(QStringLiteral("count"), written);
    res.insert(QStringLiteral("raw"), QString::fromStdString(rawStr));
    // 铺放起点信息（用户问「自动铺放知道从第几小节开始吗」）：启用切片的 measure 范围
    // （文件 0-based；对外显示 1-based「第 N 小节」= 文件小节号 + 1）
    int startMeasureFile = -1;
    int endMeasureFile = -1;
    for (const auto& it : items) {
        if (!it.enabled) continue;
        if (startMeasureFile < 0 || it.measure < startMeasureFile) startMeasureFile = it.measure;
        if (it.measure > endMeasureFile) endMeasureFile = it.measure;
    }
    if (startMeasureFile >= 0) {
        res.insert(QStringLiteral("startMeasure"), startMeasureFile + 1);
        res.insert(QStringLiteral("endMeasure"), endMeasureFile + 1);
        QString placement = QStringLiteral("铺放从第 %1 小节起").arg(startMeasureFile + 1);
        if (endMeasureFile > startMeasureFile)
            placement += QStringLiteral("（至第 %1 小节）").arg(endMeasureFile + 1);
        res.insert(QStringLiteral("placementText"), placement);
    }
    // 连续导入导出（用户 2026-09）：下一起始 id = 本次分配的最大 id + 1（跳过谱面已占用；
    // 无启用切片 → 起始不变；分配达上限 ZZ → 兜底最低空闲）
    int nextStartId = safeStartId;
    int maxAllocId = -1;
    for (const auto& it : items)
        if (it.enabled && it.wavId > 0)
            maxAllocId = std::max(maxAllocId, static_cast<int>(it.wavId));
    if (maxAllocId >= 0) {
        const auto occ = occupied_wav_ids(m_chartSession);
        int cand = maxAllocId + 1;
        while (cand <= 1295 && std::find(occ.begin(), occ.end(),
                                         static_cast<std::uint32_t>(cand)) != occ.end())
            ++cand;
        nextStartId = (cand <= 1295) ? cand : nextFreeWavId();
    }
    res.insert(QStringLiteral("nextStartId"), std::clamp(nextStartId, 1, 1295));
    qWarning("slice export: done ok=%d nextStartId=%d", errors.isEmpty() ? 1 : 0, nextStartId);
    if (!errors.isEmpty())
        res.insert(QStringLiteral("error"), errors.join(QStringLiteral("; ")));
    return res;
}

void SliceWorkspace::copyToClipboard(const QString& text) {
    QGuiApplication::clipboard()->setText(text);
}

void SliceWorkspace::setStatus(const QString& text) {
    m_statusText = text;
    emit statusChanged();
}

qreal SliceWorkspace::audioDurationSec() const {
    return m_track.valid() ? static_cast<qreal>(m_track.durationSec()) : 0.0;
}

bool SliceWorkspace::loadAudioFile(const QString& path) {
    if (path.isEmpty()) return false;
    const QString root = path;
    const auto ext = QFileInfo(root).suffix().toLower();
    if (!beatbench::audio::audio_extension_supported(ext.toStdString())) {
        setStatus(QStringLiteral("不支持的音频格式: .%1").arg(ext));
        return false;
    }
    if (m_busy) {
        setStatus(QStringLiteral("正在解码中…"));
        return false;
    }
    m_busy = true;
    emit busyChanged();
    QThreadPool::globalInstance()->start([this, root] {
        // 工作线程：局部 ReferenceTrack（解码 + 金字塔构建；不碰成员）。
        // ⚠️ Windows 含非 ASCII 路径必须走宽字符（ma_decoder_init_file_w）。
        auto* track = new beatbench::audio::ReferenceTrack();
#ifdef _WIN32
        const bool ok = track->load_w(root.toStdWString());
#else
        const bool ok = track->load(root.toStdString());
#endif
        QMetaObject::invokeMethod(
            this, [this, track, ok, root] { audioDecoded(ok, track, root); },
            Qt::QueuedConnection);
    });
    return true;
}

void SliceWorkspace::audioDecoded(bool ok, beatbench::audio::ReferenceTrack* track,
                                  const QString& path) {
    m_busy = false;
    emit busyChanged();
    if (!ok || !track || !track->valid()) {
        const QString message = (track && !track->error().empty())
                                    ? QString::fromStdString(track->error())
                                    : QStringLiteral("未知错误");
        delete track;
        setStatus(QStringLiteral("解码失败：%1").arg(message));
        return;  // 原参考音轨保留（工作台语义）
    }
    m_track = std::move(*track);
    delete track;
    m_audioPath = path;
    if (m_engine) m_engine->setReferencePcm(m_track.pcm(), m_track.sampleRate());
    emit audioChanged();
    setStatus(QStringLiteral("参考音频已载入：%1 秒").arg(
        QString::number(audioDurationSec(), 'f', 2)));
}

bool SliceWorkspace::loadMidiFile(const QString& path) {
    if (path.isEmpty()) {
        setStatus(QStringLiteral("MIDI 路径为空"));
        return false;
    }
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("无法读取 MIDI 文件: %1").arg(path));
        return false;
    }
    const QByteArray data = f.readAll();
    try {
        std::vector<std::uint8_t> bytes(data.begin(), data.end());
        midi::MidiFile parsed = midi::parse_midi_bytes(bytes);
        m_midi = std::move(parsed);
        m_midiPath = path;
        emit midiChanged();
        setStatus(QStringLiteral("MIDI 已解析：%1 个音符（%2 轨）")
                      .arg(m_midi.notes.size())
                      .arg(m_midi.ntrks));
        return true;
    } catch (const midi::MidiError& e) {
        setStatus(QStringLiteral("MIDI 解析失败：%1").arg(
            QString::fromUtf8(e.what())));
        return false;
    }
}

void SliceWorkspace::setOffsetSec(qreal v) {
    if (qFuzzyCompare(m_offsetSec, v)) return;
    m_offsetSec = v;
    emit offsetChanged();
}

void SliceWorkspace::clearAll() {
    if (m_engine) m_engine->clearReferencePcm();
    m_track.clear();
    m_audioPath.clear();
    m_midiPath.clear();
    m_midi = midi::MidiFile();
    m_offsetSec = 0.0;
    m_slices.clear();
    m_sliceEnabled.clear();
    emit audioChanged();
    emit midiChanged();
    emit offsetChanged();
    emit slicesChanged();
    setStatus(QStringLiteral("已清除参考素材"));
}

QVariantMap SliceWorkspace::waveformInfo() const {
    return pyramid_info(m_track.valid() ? &m_track.waveform() : nullptr);
}

QVariantMap SliceWorkspace::waveformRange(qlonglong frameLo, qlonglong frameHi) const {
    QVariantMap m;
    if (!m_track.valid()) {
        m.insert(QStringLiteral("min"), 0.0);
        m.insert(QStringLiteral("max"), 0.0);
        return m;
    }
    const auto r = m_track.waveform().range(static_cast<std::size_t>(frameLo),
                                            static_cast<std::size_t>(frameHi));
    m.insert(QStringLiteral("min"), static_cast<double>(r.min));
    m.insert(QStringLiteral("max"), static_cast<double>(r.max));
    return m;
}

QVariantList SliceWorkspace::midiNotes() const {
    QVariantList out;
    for (const auto& n : m_midi.notes) {
        QVariantMap e;
        e.insert(QStringLiteral("track"), n.track);
        e.insert(QStringLiteral("channel"), n.channel);
        e.insert(QStringLiteral("pitch"), n.pitch);
        e.insert(QStringLiteral("velocity"), n.velocity);
        e.insert(QStringLiteral("startSec"), n.startSec);
        e.insert(QStringLiteral("endSec"), n.endSec);
        e.insert(QStringLiteral("startTick"), n.startTick);
        e.insert(QStringLiteral("endTick"), n.endTick);
        out.append(e);
    }
    return out;
}

// ---- M6.2 切片 ----

bool SliceWorkspace::detectSlices(const QString& source, qreal bpm, int subdivision,
                                  qreal durationSec) {
    beatbench::slice::SlicePlan plan;
    if (source == QLatin1String("grid")) {
        beatbench::slice::GridConfig cfg;
        cfg.bpm = bpm;
        cfg.subdivision = subdivision;
        cfg.offsetSec = m_offsetSec;
        cfg.durationSec = durationSec;
        plan = beatbench::slice::plan_from_grid(cfg);
    } else if (source == QLatin1String("midi")) {
        if (!hasMidi()) {
            setStatus(QStringLiteral("尚无 MIDI（请先导入 notes.mid）"));
            return false;
        }
        plan = beatbench::slice::plan_from_midi(m_midi, m_offsetSec, durationSec);
    } else {
        setStatus(QStringLiteral("未知切片源: %1").arg(source));
        return false;
    }

    m_slices = std::move(plan.slices);
    m_sliceEnabled.assign(m_slices.size(), true);
    emit slicesChanged();

    if (!plan.warnings.empty()) {
        QStringList ws;
        for (const auto& w : plan.warnings) ws << QString::fromStdString(w);
        setStatus(QStringLiteral("切片生成（%1 个）— %2")
                      .arg(m_slices.size())
                      .arg(ws.join(QStringLiteral("；"))));
    } else {
        setStatus(QStringLiteral("切片生成：%1 个（%2 源）")
                      .arg(m_slices.size())
                      .arg(source));
    }
    return true;
}

void SliceWorkspace::clearSlices() {
    if (m_slices.empty()) return;
    m_slices.clear();
    m_sliceEnabled.clear();
    emit slicesChanged();
    setStatus(QStringLiteral("已清除切片"));
}

void SliceWorkspace::setSliceEnabled(int index, bool v) {
    if (index < 0 || index >= static_cast<int>(m_sliceEnabled.size())) return;
    if (m_sliceEnabled[static_cast<std::size_t>(index)] == v) return;
    m_sliceEnabled[static_cast<std::size_t>(index)] = v;
    emit slicesChanged();
}

// ---- M6.4c 手动切分点 ----

namespace {
/// 边界判定容差（秒）：手动点与切片起点的「同一性」判断。
constexpr double kPointEps = 1e-4;
}

/// 命中内部边界的切片序号（startSec ≈ t 且 i >= 1；靠后优先——负数容差对称无所谓）。
int SliceWorkspace::findBoundaryIndex(double t) const {
    for (std::size_t i = 1; i < m_slices.size(); ++i) {
        if (std::abs(m_slices[i].startSec - t) <= kPointEps)
            return static_cast<int>(i);
    }
    return -1;
}

void SliceWorkspace::renumberSlices() {
    for (std::size_t i = 0; i < m_slices.size(); ++i)
        m_slices[i].index = static_cast<int>(i);
}

bool SliceWorkspace::toggleManualPoint(double t) {
    if (m_slices.empty()) {
        setStatus(QStringLiteral("无切片可切分（先生成切片再双击）"));
        return false;
    }
    const int bi = findBoundaryIndex(t);
    if (bi >= 0) return removeManualPointAt(static_cast<std::size_t>(bi));
    // 拆分包含 t 的切片（t 须在切片内部，非边界）
    for (std::size_t i = 0; i < m_slices.size(); ++i) {
        const auto& s = m_slices[i];
        if (t > s.startSec + kPointEps && t < s.endSec - kPointEps) {
            const bool en = m_sliceEnabled[i];
            beatbench::slice::Slice a = s; a.endSec = t;
            beatbench::slice::Slice b = s; b.startSec = t;
            m_slices.insert(m_slices.begin() + static_cast<std::ptrdiff_t>(i + 1), b);
            m_slices[i] = a;
            m_sliceEnabled.insert(m_sliceEnabled.begin() + static_cast<std::ptrdiff_t>(i + 1), en);
            renumberSlices();
            emit slicesChanged();
            setStatus(QStringLiteral("手动切分点：+1（第 %1 片拆为 %2/%3）")
                          .arg(i + 1).arg(i + 1).arg(i + 2));
            return true;
        }
    }
    setStatus(QStringLiteral("该位置不在现有切片内（无法放置切分点）"));
    return false;
}

bool SliceWorkspace::removeManualPoint(double t) {
    const int bi = findBoundaryIndex(t);
    if (bi < 0) return false;
    return removeManualPointAt(static_cast<std::size_t>(bi));
}

bool SliceWorkspace::removeManualPointAt(std::size_t i) {
    // 合并 i-1 与 i：[start_{i-1}, end_i)；两侧须连续（网格切片必连续）
    if (i == 0 || i >= m_slices.size()) return false;
    if (std::abs(m_slices[i - 1].endSec - m_slices[i].startSec) > kPointEps) {
        setStatus(QStringLiteral("该边界两侧不连续，无法合并"));
        return false;
    }
    m_slices[i - 1].endSec = m_slices[i].endSec;
    m_slices.erase(m_slices.begin() + static_cast<std::ptrdiff_t>(i));
    m_sliceEnabled.erase(m_sliceEnabled.begin() + static_cast<std::ptrdiff_t>(i));
    renumberSlices();
    emit slicesChanged();
    setStatus(QStringLiteral("手动切分点：-1（第 %1/%2 片合并）").arg(i).arg(i + 1));
    return true;
}

QVariantList SliceWorkspace::slices() const {
    QVariantList out;
    for (std::size_t i = 0; i < m_slices.size(); ++i) {
        const auto& s = m_slices[i];
        QVariantMap e;
        e.insert(QStringLiteral("index"), s.index);
        e.insert(QStringLiteral("startSec"), s.startSec);
        e.insert(QStringLiteral("endSec"), s.endSec);
        e.insert(QStringLiteral("durationSec"), s.endSec - s.startSec);
        e.insert(QStringLiteral("kind"), QString::fromStdString(s.kind));
        e.insert(QStringLiteral("note"), s.note);
        e.insert(QStringLiteral("enabled"),
                 i < m_sliceEnabled.size() && m_sliceEnabled[i]);
        out.append(e);
    }
    return out;
}

qreal SliceWorkspace::midiTempoBpm() const {
    if (m_midi.tempos.empty()) return 120.0;
    const auto& t = m_midi.tempos.front();  // 已按 tick 排序；首个（通常 tick0）
    if (t.usPerQuarter <= 0) return 120.0;
    return 60.0 * 1e6 / static_cast<double>(t.usPerQuarter);
}

}  // namespace beatbench::app
