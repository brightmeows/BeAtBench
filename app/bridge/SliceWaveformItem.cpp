// SPDX-License-Identifier: GPL-3.0-only
// SliceWaveformItem 实现（见 hpp 注释）。绘制顺序：底 → 波形列 → 中央轴 →
// MIDI note 刻度（+offset 修正）→ 播放头 → 边缘。
#include "bridge/SliceWaveformItem.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

#include <algorithm>
#include <cmath>

#include "bridge/SliceWorkspace.hpp"
#include "bridge/ThemeManager.hpp"

namespace beatbench::app {

SliceWaveformItem::SliceWaveformItem(QQuickItem* parent) : QQuickPaintedItem(parent) {
    setAntialiasing(false);  // 波形逐列 1px：抗锯齿反而糊
    setAcceptedMouseButtons(Qt::LeftButton);
}

void SliceWaveformItem::setWorkspace(QObject* v) {
    if (m_workspace == v) return;
    if (auto* old = workspaceObj()) disconnect(old, nullptr, this, nullptr);
    m_workspace = v;
    if (auto* ws = workspaceObj()) {
        // 数据变更（解码完成/MIDI 导入/offset/切片变化）→ 重绘（信号均在 UI 线程发）
        connect(ws, &SliceWorkspace::audioChanged, this, [this] { update(); });
        connect(ws, &SliceWorkspace::midiChanged, this, [this] { update(); });
        connect(ws, &SliceWorkspace::offsetChanged, this, [this] { update(); });
        connect(ws, &SliceWorkspace::slicesChanged, this, [this] { update(); });
    }
    emit workspaceChanged();
    update();
}

void SliceWaveformItem::setTheme(QObject* v) {
    if (m_theme == v) return;
    m_theme = v;
    emit themeChanged();
    update();
}

void SliceWaveformItem::setPlayheadSec(qreal v) {
    if (qFuzzyCompare(m_playheadSec, v)) return;
    m_playheadSec = v;
    emit playheadSecChanged();
    update();
}

void SliceWaveformItem::setGridVisible(bool v) {
    if (m_gridVisible == v) return;
    m_gridVisible = v;
    emit gridVisibleChanged();
    update();
}

void SliceWaveformItem::setGridBpm(qreal v) {
    if (qFuzzyCompare(m_gridBpm, v)) return;
    m_gridBpm = v;
    emit gridBpmChanged();
    update();
}

void SliceWaveformItem::setGridSubdivision(int v) {
    if (m_gridSubdivision == v) return;
    m_gridSubdivision = v;
    emit gridSubdivisionChanged();
    update();
}

void SliceWaveformItem::setGridBeatsPerMeasure(int v) {
    if (m_gridBeatsPerMeasure == v) return;
    m_gridBeatsPerMeasure = v;
    emit gridBeatsPerMeasureChanged();
    update();
}

SliceWorkspace* SliceWaveformItem::workspaceObj() const {
    return qobject_cast<SliceWorkspace*>(m_workspace);
}

ThemeManager* SliceWaveformItem::themeObj() const {
    return qobject_cast<ThemeManager*>(m_theme);
}

void SliceWaveformItem::requestSeek(qreal x) {
    const SliceWorkspace* ws = workspaceObj();
    if (!ws || !ws->hasAudio()) return;
    const double dur = static_cast<double>(ws->audioDurationSec());
    const qreal extent = std::max<qreal>(1.0, width());
    const double frac = std::clamp(x / extent, 0.0, 1.0);
    emit seekRequested(frac * dur);
}

void SliceWaveformItem::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        requestSeek(event->position().x());
    }
    QQuickPaintedItem::mousePressEvent(event);
}

void SliceWaveformItem::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        event->accept();
        requestSeek(event->position().x());
    }
    QQuickPaintedItem::mouseMoveEvent(event);
}

void SliceWaveformItem::mouseReleaseEvent(QMouseEvent* event) {
    // 拖动结束：落点再 seek 一次（QML 端 refSeek 幂等；保证最终位置精确）
    if (event->button() == Qt::LeftButton) requestSeek(event->position().x());
    QQuickPaintedItem::mouseReleaseEvent(event);
}

void SliceWaveformItem::paint(QPainter* p) {
    const qreal w = width();
    const qreal h = height();
    if (w <= 0 || h <= 0) return;
    const ThemeManager* th = themeObj();
    p->fillRect(QRectF(0, 0, w, h), th ? th->surface() : QColor(QStringLiteral("#12151a")));

    const SliceWorkspace* ws = workspaceObj();
    const auto pyr = ws ? ws->waveformPyramid() : nullptr;
    if (!ws || !pyr || !pyr->valid()) {
        p->setPen(th ? th->textFaint() : QColor(QStringLiteral("#6b7484")));
        p->drawText(QRectF(0, 0, w, h), Qt::AlignCenter,
                    QStringLiteral("导入参考音频后显示波形"));
        return;
    }

    const double dur = static_cast<double>(ws->audioDurationSec());
    if (dur <= 0.0) return;
    const qreal barTop = 2.0;
    const qreal barH = h - 2.0 * barTop;
    const qreal centerY = h / 2.0;
    const qreal pxPerSec = w / dur;

    // ---- 全曲幅度归一化（先扫全局；防逐列归一化闪烁） ----
    qreal amp = 0.0;
    {
        const auto r = pyr->range(0, pyr->frameCount());
        amp = std::max(std::abs(r.min), std::abs(r.max));
    }

    // ---- 逐列 min/max ----
    QColor waveCol = th ? th->wave() : QColor(QStringLiteral("#8b9cf8"));
    waveCol.setAlpha(220);
    QColor gridCol = th ? th->border() : QColor(QStringLiteral("#2a2f3a"));
    QColor axisCol = th ? th->textFaint() : QColor(QStringLiteral("#6b7484"));
    p->setPen(Qt::NoPen);
    const double sr = pyr->sampleRate() > 0.0 ? pyr->sampleRate() : 44100.0;
    for (int i = 0; i < static_cast<int>(w); ++i) {
        const qreal frac = static_cast<qreal>(i) / std::max<qreal>(1.0, w);
        const std::size_t f0 = static_cast<std::size_t>(frac * dur * sr);
        const std::size_t f1 = static_cast<std::size_t>((frac + 1.0 / std::max<qreal>(1.0, w)) * dur * sr);
        const auto r = pyr->range(f0, std::max(f0 + 1, f1));
        if (amp > 1e-6) {
            const qreal yTop = centerY -
                static_cast<qreal>(r.max) / amp * (barH / 2.0 - 1.0);
            const qreal yBot = centerY -
                static_cast<qreal>(r.min) / amp * (barH / 2.0 - 1.0);
            p->fillRect(QRectF(i, yTop, 1.0,
                               std::max<qreal>(1.0, yBot - yTop)), waveCol);
        }
    }

    // ---- 中央轴 + 边框 ----
    p->setPen(QPen(gridCol, 1));
    p->drawLine(QPointF(0, centerY), QPointF(w, centerY));
    p->drawLine(QPointF(0, 0.5), QPointF(w, 0.5));
    p->drawLine(QPointF(0, h - 0.5), QPointF(w, h - 0.5));

    // ---- 实时拍子网格参考线（M6.2：offset/BPM/细分的视觉反馈；等分） ----
    drawGridLines(p, w, h, ws);

    // ---- MIDI note 刻度（startSec+offset → x；1px 竖线；低音/高音不区分） ----
    const double offset = ws->offsetSecD();
    QColor noteCol = th ? th->accent2() : QColor(QStringLiteral("#2dd8c8"));
    noteCol.setAlpha(170);
    for (const auto& n : ws->notes()) {
        const qreal x0 = static_cast<qreal>((n.startSec + offset) * pxPerSec);
        const qreal x1 = static_cast<qreal>((n.endSec + offset) * pxPerSec);
        if (x1 < 0.0 || x0 > w) continue;
        const qreal cx = std::clamp(x0, 0.0, w);
        p->fillRect(QRectF(cx, 4.0, 1.5, h - 8.0), noteCol);
        if (x1 > x0 + 1.0) {
            // 时长>~1ms：末刻度淡色（区分「音符段」与「起始点」）
            QColor tail = noteCol;
            tail.setAlpha(90);
            p->fillRect(QRectF(std::clamp(x1, 0.0, w), 4.0, 1.5, h - 8.0), tail);
        }
    }

    // ---- 切片边界线（M6.2：grid = 主色；midi = 强调；淡色 = 切片末端） ----
    // 切片 startSec 已含 offset（core plan 应用过），此处直接换算。
    if (ws->hasSlices()) {
        QColor gridLine = th ? th->primary() : QColor(QStringLiteral("#8b9cf8"));
        gridLine.setAlpha(210);
        QColor midiLine = th ? th->accent2() : QColor(QStringLiteral("#2dd8c8"));
        midiLine.setAlpha(230);
        for (const auto& s : ws->slicesC()) {
            const bool isMidi = s.kind == "midi";
            QColor col = isMidi ? midiLine : gridLine;
            const qreal x0 = static_cast<qreal>(s.startSec * pxPerSec);
            const qreal x1 = static_cast<qreal>(s.endSec * pxPerSec);
            if (x1 < 0.0 || x0 > w) continue;
            p->fillRect(QRectF(std::clamp(x0, 0.0, w), 1.0, 1.0, h - 2.0), col);
            QColor tail = col;
            tail.setAlpha(80);
            p->fillRect(QRectF(std::clamp(x1, 0.0, w), 1.0, 1.0, h - 2.0), tail);
        }
    }

    // ---- 播放头 ----
    if (m_playheadSec >= 0.0) {
        const qreal x = static_cast<qreal>(m_playheadSec * pxPerSec);
        if (x >= 0.0 && x <= w) {
            QColor ph = th ? th->primary() : QColor(QStringLiteral("#8b9cf8"));
            p->fillRect(QRectF(x - 0.75, 0.0, 1.5, h), ph);
        }
    }
    Q_UNUSED(axisCol);
}

void SliceWaveformItem::drawGridLines(QPainter* p, qreal w, qreal h,
                                      const SliceWorkspace* ws) const {
    if (!m_gridVisible || !ws || !ws->hasAudio()) return;
    const double dur = static_cast<double>(ws->audioDurationSec());
    if (dur <= 0.0 || m_gridBpm <= 0.0) return;
    const int sub = std::max(1, m_gridSubdivision);
    const int bpmCount = std::max(1, m_gridBeatsPerMeasure);
    const double cell = 60.0 / static_cast<double>(m_gridBpm) /
                        static_cast<double>(sub);
    if (!std::isfinite(cell) || cell <= 0.0) return;

    const double offset = ws->offsetSecD();
    const qreal pxPerSec = w / dur;
    const ThemeManager* th = themeObj();
    // 层级配色（背景 #12151a 深色 → 必须亮色拉开对比，否则看不见）：
    //   起点线 = accent 青（最显眼）+ 顶部 tab/标签
    //   小节线 = keyNote(亮靛) 2px；拍线 = keyNote 1px（中亮）；细分线 = textMuted 灰 1px
    const QColor originCol = th ? th->accent() : QColor(QStringLiteral("#22d3ee"));
    QColor measureCol = th ? th->keyNote() : QColor(QStringLiteral("#8b9cf8"));
    QColor beatCol = th ? th->keyNote() : QColor(QStringLiteral("#8b9cf8"));
    QColor subCol = th ? th->textMuted() : QColor(QStringLiteral("#9aa3b2"));
    measureCol.setAlpha(250);
    beatCol.setAlpha(190);
    subCol.setAlpha(110);
    const qreal top = 4.0;
    const qreal bot = h - 4.0;
    const int cellsPerBeat = sub;
    const int cellsPerMeasure = cellsPerBeat * bpmCount;

    // 起始 cell 序号：offset 为负时跳过 t<0 的边界（避免 x=0 叠线；同 plan 的夹逼语义）
    double firstK = 0.0;
    if (offset < 0.0) firstK = std::ceil(-offset / cell);
    constexpr int kMaxLines = 20000;
    int drawn = 0;
    for (int k = static_cast<int>(firstK); ; ++k, ++drawn) {
        if (drawn > kMaxLines) break;
        const double t = offset + static_cast<double>(k) * cell;
        if (t >= dur) break;
        const qreal x = static_cast<qreal>(t * pxPerSec);
        if (x > w) break;
        if (x < 0.0) continue;
        const bool isOrigin = (k == 0);
        const bool isMeasure = (k % cellsPerMeasure) == 0;
        const bool isBeat = (k % cellsPerBeat) == 0;
        QColor col;
        double penW = 1.0;
        if (isOrigin) {
            col = originCol;
            penW = 2.0;
        } else if (isMeasure) {
            col = measureCol;
            penW = 2.0;
        } else if (isBeat) {
            col = beatCol;
            penW = 1.0;
        } else {
            col = subCol;
            penW = 1.0;
        }
        p->setPen(QPen(col, penW));
        p->drawLine(QPointF(x, top), QPointF(x, bot));
        if (isOrigin) drawOriginMarker(p, x, th, t, w);
    }
}

void SliceWaveformItem::drawOriginMarker(QPainter* p, qreal x,
                                         const ThemeManager* th, double t,
                                         qreal w) const {
    // 顶部 tab（起点标记）：一条亮色短横 + 秒数标签（mono），clamp 在视口内。
    const QColor col = th ? th->accent() : QColor(QStringLiteral("#22d3ee"));
    const qreal tabW = 10.0;
    const qreal tabH = 8.0;
    const qreal tabLeft = std::clamp(x - tabW / 2.0, 0.0,
                                     std::max(0.0, w - tabW));
    p->fillRect(QRectF(tabLeft, 0.0, tabW, tabH), col);
    const QString label = QStringLiteral("%1s").arg(t, 0, 'f', 3);
    QFont f = p->font();
    f.setFamily(th ? th->fontMono() : QStringLiteral("Consolas"));
    f.setPixelSize(10);
    p->setFont(f);
    p->setPen(col);
    const int tw = p->fontMetrics().horizontalAdvance(label);
    qreal lx = x + tabW / 2.0 + 3.0;
    lx = std::clamp(lx, 4.0, std::max(4.0, w - tw - 4.0));
    p->drawText(QPointF(lx, tabH - 1.0), label);
}

}  // namespace beatbench::app
