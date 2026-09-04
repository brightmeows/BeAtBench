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

public:
    explicit SliceWaveformItem(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    QObject* workspace() const { return m_workspace; }
    void setWorkspace(QObject* v);
    QObject* theme() const { return m_theme; }
    void setTheme(QObject* v);
    qreal playheadSec() const { return m_playheadSec; }
    void setPlayheadSec(qreal v);

signals:
    void workspaceChanged();
    void themeChanged();
    void playheadSecChanged();
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

    QObject* m_workspace = nullptr;
    QObject* m_theme = nullptr;
    qreal m_playheadSec = -1.0;
};

}  // namespace beatbench::app
