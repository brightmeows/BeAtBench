// SPDX-License-Identifier: GPL-3.0-only
// SliceWorkspace 实现（见 hpp 注释）。线程编排：
// - loadAudioFile（UI）→ QThreadPool（**局部 ReferenceTrack**：解码 + 金字塔构建；
//   宽字符路径，Windows）→ 回 UI 线程：move 进 m_track → AudioEngine::setReferencePcm；
// - loadMidiFile 同步（core 解析快）；失败/成功都 statusText 提示；
// - offset 仅数据（画 note 刻度/列表显示时 +offsetSec）；切片边界推导（M6.2）再消费。
#include "bridge/SliceWorkspace.hpp"

#include <QFile>
#include <QFileInfo>
#include <QMetaObject>
#include <QThreadPool>
#include <QVariantMap>

#include "bridge/AudioEngine.hpp"

namespace beatbench::app {

namespace {

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
    emit audioChanged();
    emit midiChanged();
    emit offsetChanged();
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

}  // namespace beatbench::app
