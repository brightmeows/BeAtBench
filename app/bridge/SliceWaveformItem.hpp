// SPDX-License-Identifier: GPL-3.0-only
// 切音工作台波形视图（M6.1）：参考音频全曲波形 + MIDI note 刻度 + 播放头 + 点击/拖动 seek。
// 数据源 = SliceWorkspace（波形金字塔 + note 表 + offset）；播放头位置由 QML 定时
// 读 audioEngine.refPositionSec 传入（~20Hz）。皮肤：QPainter 自绘 + Theme token。
#pragma once

#include <QQuickPaintedItem>
#include <QtQml/qqmlregistration.h>

namespace beatbench::app {

class SliceWorkspace;
class ThemeManager;

class SliceWaveformItem : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QObject* workspace READ workspace WRITE setWorkspace NOTIFY workspaceChanged)
    Q_PROPERTY(QObject* theme READ theme WRITE setTheme NOTIFY themeChanged)
    /// 播放头秒（<0 = 不显示；QML 定时刷新）。
    Q_PROPERTY(qreal playheadSec READ playheadSec WRITE setPlayheadSec NOTIFY playheadSecChanged)
    // ---- M6.2 实时拍子网格参考线（offset/BPM/snap 调参的视觉反馈） ----
    /// 显示开关（QML 绑定「切片源=网格」时 true）。
    Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    /// 网格 BPM（QML bpmBox 绑定；>0 才画）。
    Q_PROPERTY(qreal gridBpm READ gridBpm WRITE setGridBpm NOTIFY gridBpmChanged)
    /// 每拍细分（QML subBox 绑定；>=1）。
    Q_PROPERTY(int gridSubdivision READ gridSubdivision WRITE setGridSubdivision NOTIFY gridSubdivisionChanged)
    /// 每小节拍数（网格参考线小节分组；默认 4 = 4/4；同 core GridConfig.beatsPerMeasure）。
    Q_PROPERTY(int gridBeatsPerMeasure READ gridBeatsPerMeasure WRITE setGridBeatsPerMeasure NOTIFY gridBeatsPerMeasureChanged)
    /// MIDI note 刻度线显示（M6.3c：网格模式下默认关；QML「MIDI 线」开关 + Ctrl 临时取反）。
    Q_PROPERTY(bool midiVisible READ midiVisible WRITE setMidiVisible NOTIFY midiVisibleChanged)

public:
    explicit SliceWaveformItem(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    QObject* workspace() const { return m_workspace; }
    void setWorkspace(QObject* v);
    QObject* theme() const { return m_theme; }
    void setTheme(QObject* v);
    qreal playheadSec() const { return m_playheadSec; }
    void setPlayheadSec(qreal v);
    bool gridVisible() const { return m_gridVisible; }
    void setGridVisible(bool v);
    qreal gridBpm() const { return m_gridBpm; }
    void setGridBpm(qreal v);
    int gridSubdivision() const { return m_gridSubdivision; }
    void setGridSubdivision(int v);
    int gridBeatsPerMeasure() const { return m_gridBeatsPerMeasure; }
    void setGridBeatsPerMeasure(int v);
    bool midiVisible() const { return m_midiVisible; }
    void setMidiVisible(bool v);

signals:
    void workspaceChanged();
    void themeChanged();
    void playheadSecChanged();
    void gridVisibleChanged();
    void gridBpmChanged();
    void gridSubdivisionChanged();
    void gridBeatsPerMeasureChanged();
    void midiVisibleChanged();
    /// 点击/拖动 → 目标秒（QML 接 audioEngine.refSeek）。
    void seekRequested(double seconds);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    SliceWorkspace* workspaceObj() const;
    ThemeManager* themeObj() const;
    /// x（widget 坐标）→ 秒，发出 seekRequested。
    void requestSeek(qreal x);
    /// 画实时拍子网格参考线（BPM/细分/offset；等分；M6.2）。四级：起点/小节/拍/细分。
    void drawGridLines(QPainter* p, qreal w, qreal h, const SliceWorkspace* ws) const;
    /// 画起点（time=offset）显眼标记：顶部 tab + 秒数标签。
    void drawOriginMarker(QPainter* p, qreal x, const ThemeManager* th,
                          double t, qreal w) const;

    QObject* m_workspace = nullptr;
    QObject* m_theme = nullptr;
    qreal m_playheadSec = -1.0;
    // ---- M6.2 实时拍子网格 ----
    bool m_gridVisible = false;
    qreal m_gridBpm = 120.0;
    int m_gridSubdivision = 4;
    int m_gridBeatsPerMeasure = 4;
    bool m_midiVisible = true;  ///< MIDI note 刻度（M6.3c 网格模式默认关）
};

}  // namespace beatbench::app
