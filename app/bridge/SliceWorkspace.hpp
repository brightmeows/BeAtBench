// SPDX-License-Identifier: GPL-3.0-only
// 切音工作台数据桥（M6.1）：参考音频（外部 stem.wav）+ MIDI（notes.mid）的装载与持有。
//
// 定位（doc/02 §7.1 M6.1）：与编辑页平级的切音工作台的数据侧——「导入工作台」：
// 外部音频解码（→ 波形金字塔 + 时长/采样率）+ MIDI 解析（→ note 表）+ offset 微调
// （全局；逻辑在此桥换算出「+offset」后的秒）。播放/seek 委托 AudioEngine
// （参考音频专用 PcmPlayback，与谱面播放独立）。MIDI 解析走 core `midi` 模块
// （headless 可测；本桥只做文件读取与 QML 适配）。
//
// 双语言纪律（doc/08 §2）：解码/解析/数据持有在 C++，QML 只消费信号/属性。
// 线程：解码在 QThreadPool（不卡 UI），完成回 UI 线程装载。
#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

#include <memory>
#include <vector>

#include "beatbench/audio/ReferenceTrack.hpp"
#include "beatbench/core/midi/MidiFile.hpp"
#include "beatbench/core/slice/Slice.hpp"

namespace beatbench::app {

class AudioEngine;

class SliceWorkspace : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString audioPath READ audioPath NOTIFY audioChanged)
    Q_PROPERTY(bool hasAudio READ hasAudio NOTIFY audioChanged)
    Q_PROPERTY(qreal audioDurationSec READ audioDurationSec NOTIFY audioChanged)
    Q_PROPERTY(qreal audioSampleRate READ audioSampleRate NOTIFY audioChanged)
    Q_PROPERTY(QString midiPath READ midiPath NOTIFY midiChanged)
    Q_PROPERTY(bool hasMidi READ hasMidi NOTIFY midiChanged)
    Q_PROPERTY(QVariantList midiNotes READ midiNotes NOTIFY midiChanged)
    Q_PROPERTY(qreal offsetSec READ offsetSec WRITE setOffsetSec NOTIFY offsetChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    // ---- M6.2 切片 ----
    Q_PROPERTY(QVariantList slices READ slices NOTIFY slicesChanged)
    Q_PROPERTY(bool hasSlices READ hasSlices NOTIFY slicesChanged)
    /// MIDI tempo（首个 tempo 事件 → BPM；无 MIDI/无 tempo → 120）。网格参数默认值。
    Q_PROPERTY(qreal midiTempoBpm READ midiTempoBpm NOTIFY midiChanged)

public:
    explicit SliceWorkspace(QObject* parent = nullptr);

    /// 播放/seek 委托目标（main.cpp 接线；不拥有）。
    void setAudioEngine(AudioEngine* engine);

    /// 导入参考音频（异步解码；完成 → 波形金字塔 + 交给 AudioEngine 预览）。
    /// 返回 false = 立即失败（无文件/格式不支持）；解码失败异步报 statusText。
    Q_INVOKABLE bool loadAudioFile(const QString& path);
    /// 导入 MIDI（同步；core midi 解析；失败 statusText + false）。
    Q_INVOKABLE bool loadMidiFile(const QString& path);
    /// 清除全部参考素材（音频 + MIDI + offset；停声卸载）。
    Q_INVOKABLE void clearAll();

    /// 微调偏移（秒；正 = MIDI 相对音频延后；全局）。
    qreal offsetSec() const { return m_offsetSec; }
    void setOffsetSec(qreal v);
    /// 原始秒 → 应用 offset 后的秒（QML 显示/波形刻度用）。
    Q_INVOKABLE double adjustedSec(double rawSec) const { return rawSec + m_offsetSec; }

    // ---- M6.2 切片 ----
    /// 生成切片：source = "grid" | "midi"；offset 用当前 m_offsetSec（全局）。
    /// bpm/subdivision 仅网格用；durationSec = 音频时长（夹逼/丢弃越界切片）。
    Q_INVOKABLE bool detectSlices(const QString& source, qreal bpm, int subdivision,
                                  qreal durationSec);
    /// 清除切片（保留参考素材）。
    Q_INVOKABLE void clearSlices();
    /// 切片「放置」开关（M6.3 铺放预选；越界忽略）。
    Q_INVOKABLE void setSliceEnabled(int index, bool v);
    QVariantList slices() const;
    bool hasSlices() const { return !m_slices.empty(); }
    qreal midiTempoBpm() const;

    // ---- C++ 消费（SliceWaveformItem 等） ----
    /// 波形金字塔指针（无音频 → nullptr；音频层 ReferenceTrack 持有）。
    const beatbench::audio::WaveformPyramid* waveformPyramid() const {
        return m_track.valid() ? &m_track.waveform() : nullptr;
    }
    const std::vector<midi::MidiNote>& notes() const { return m_midi.notes; }
    double offsetSecD() const { return m_offsetSec; }
    /// M6.2 切片表（C++ 消费：SliceWaveformItem 画线等）。
    const std::vector<beatbench::slice::Slice>& slicesC() const { return m_slices; }
    /// 参考音轨（C++ 消费：M6.3 分片导出 window() 等；无音频 → invalid）。
    const beatbench::audio::ReferenceTrack& track() const { return m_track; }

    // ---- QML 波形查询（总览条同构接口） ----
    Q_INVOKABLE QVariantMap waveformInfo() const;
    Q_INVOKABLE QVariantMap waveformRange(qlonglong frameLo, qlonglong frameHi) const;

    QString audioPath() const { return m_audioPath; }
    bool hasAudio() const { return m_track.valid(); }
    qreal audioDurationSec() const;
    qreal audioSampleRate() const { return m_track.sampleRate(); }
    QString midiPath() const { return m_midiPath; }
    bool hasMidi() const { return !m_midiPath.isEmpty(); }
    QVariantList midiNotes() const;
    bool busy() const { return m_busy; }
    QString statusText() const { return m_statusText; }

signals:
    void audioChanged();
    void midiChanged();
    void offsetChanged();
    void busyChanged();
    void statusChanged();
    /// M6.2 切片表变化（生成/清除/放置开关）。
    void slicesChanged();

private:
    void setStatus(const QString& text);
    /// 解码完成（UI 线程）：ok 且 track 有效 → 移入 m_track + 交 AudioEngine 预览；
    /// 失败 → statusText（原参考音轨保留）。
    void audioDecoded(bool ok, beatbench::audio::ReferenceTrack* track,
                      const QString& path);

    AudioEngine* m_engine = nullptr;
    QString m_audioPath;
    QString m_midiPath;
    beatbench::audio::ReferenceTrack m_track;  ///< 参考音轨（PCM + 金字塔 + 统计；音频层）
    midi::MidiFile m_midi;  ///< core 解析结果（notes() 供绘制；midiNotes() 供 QML）
    // ---- M6.2 切片 ----
    std::vector<beatbench::slice::Slice> m_slices;  ///< 切片表（core 计算结果）
    std::vector<bool> m_sliceEnabled;               ///< 放置开关（与 m_slices 一一对应）
    qreal m_offsetSec = 0.0;
    bool m_busy = false;
    QString m_statusText;
};

}  // namespace beatbench::app
