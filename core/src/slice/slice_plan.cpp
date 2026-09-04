// SPDX-License-Identifier: GPL-3.0-only
// 切片位置规划实现（M6.2）。纯计算；见 Slice.hpp 头注释。
#include "beatbench/core/slice/Slice.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace beatbench::slice {

SlicePlan plan_from_grid(const GridConfig& cfg) {
    SlicePlan plan;
    plan.durationSec = cfg.durationSec;

    const double cell = 60.0 / cfg.bpm / static_cast<double>(cfg.subdivision);
    if (!std::isfinite(cell) || cell <= 0.0) {
        plan.warnings.push_back("网格无效（bpm/细分须为正有限值）");
        return plan;
    }

    // 无时长上限：按 1 小时生成（防无界循环；GUI 总会传 durationSec）
    double limit = cfg.durationSec;
    if (limit <= 0.0) {
        limit = 3600.0;
        plan.warnings.push_back("未提供时长，网格按前 3600 秒生成");
    }

    // 边界序列：t = offset, offset+cell, …（负 offset：切片起点夹到 0）
    // 每块边界 = 一个切片 [t, t+cell)（末尾夹逼到 durationSec）
    double t = cfg.offsetSec;
    while (t < limit && plan.slices.size() < 10000000) {
        Slice s;
        s.startSec = std::max(0.0, t);
        s.endSec = std::min(t + cell, limit);
        if (s.endSec > s.startSec) {
            s.kind = "grid";
            s.index = static_cast<int>(plan.slices.size());
            plan.slices.push_back(std::move(s));
        }
        t += cell;
        if (!std::isfinite(t)) break;
    }
    return plan;
}

SlicePlan plan_from_midi(const midi::MidiFile& midi, double offsetSec,
                         double durationSec) {
    SlicePlan plan;
    plan.durationSec = durationSec;

    for (const auto& n : midi.notes) {
        Slice s;
        s.index = static_cast<int>(plan.slices.size());
        s.startSec = n.startSec + offsetSec;
        s.endSec = n.endSec + offsetSec;
        s.kind = "midi";
        s.note = n.pitch;
        s.startTick = n.startTick;
        if (durationSec > 0.0) {
            if (s.startSec >= durationSec) continue;  // 起点已越界 → 丢弃
            if (s.endSec > durationSec) s.endSec = durationSec;
        }
        if (s.endSec <= s.startSec) continue;
        plan.slices.push_back(std::move(s));
    }
    return plan;
}

}  // namespace beatbench::slice
