// SPDX-License-Identifier: GPL-3.0-only
// M6.3 分片导出布局（纯计算，零 Qt/零音频）：#WAV id 分配 + 切片→拍位 + 可复制 BMS raw。
//
// 定位（doc/02 §7.1 M6.3）：切音工作台把参考音频切成 one-shot 分片并导出为 .wav，
// 再按拍位铺进 ch01（BGM）重建谱面。本模块 = 布局层（与文件 I/O / 音频解码解耦）：
// - 分配最低空闲 #WAV id（跳过谱面已占用；支持「导出起始 id」连续分配）；
// - 把每个切片起点（区间秒）换算成拍位（measure + pos）；
// - 生成「可复制 BMS raw」——#WAVxx 定义 + ch01 铺放 note 行（复用 bms::write_bms，
//   正确性由 writer 保证），供粘贴到编辑区。
//
// 文件落盘 window()+fade+write_wav 在 audio 层（导出原语），GUI 工作台组合调用。
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "beatbench/core/Rational.hpp"
#include "beatbench/core/slice/Slice.hpp"

namespace beatbench::slice {

/// 一个导出分片的布局结果。
struct SliceExportItem {
    int sliceIndex = 0;       ///< 原切片表序号
    std::uint32_t wavId = 0;  ///< 分配的 #WAV id
    std::string fileName;     ///< 落盘文件名（如 slice_000.wav）
    int measure = 0;          ///< 铺放拍位（小节）
    Rational pos;             ///< 节内位置（[0,1)）
    bool enabled = false;     ///< 放置开关（决定是否生成 ch01 note）
};

/// 为 count 个切片分配最低空闲 #WAV id：从 start_id 起，跳过 occupied + 本次已分配。
/// 超出上限（1295 = ZZ）时截断（count 只计到可分配数）。返回按序的长度 ≤ count。
std::vector<std::uint32_t> allocate_wav_ids(
    const std::vector<std::uint32_t>& occupied, std::uint32_t start_id, int count);

/// 汇总导出布局：切片 → {id, 文件名, 拍位}。enabled[i] 决定是否铺放。
/// 拍位换算：beat = (startSec - offset) * bpm / 60；measure = floor(beat/beatsPerMeasure)；
/// pos = round(fracBeat*subdivision) / (beatsPerMeasure*subdivision)（吸附到细分网格）。
/// baseName = 文件名前缀（如 "slice"），序号补零到 width 位。
/// startMeasure = 铺放起始小节（1-based：ch01 从第 N 小节起 = 文件 `#(N-1)01:`；
/// 所有切片的 measure 整体平移 startMeasure-1；默认 1 = 从第 1 小节起，即文件 #000）。
std::vector<SliceExportItem> build_export_layout(
    const std::vector<Slice>& slices, const std::vector<bool>& enabled,
    const std::vector<std::uint32_t>& occupied, std::uint32_t start_id,
    const std::string& baseName, double bpm, int beatsPerMeasure,
    int subdivision, double offset, int startMeasure = 1, int width = 3);

/// 生成「可复制 BMS raw」：把 items 铺进一个临时 Chart（WAV 定义 + ch01 note），
/// 经 bms::write_bms 写出再筛掉杂线，返回 `#WAVxx <file>` 定义 + ch01 数据行。
/// beatsPerMeasure 决定 ch02 小节长度（=4 时按默认 4/4，不输出）。
std::string build_placement_raw(const std::vector<SliceExportItem>& items,
                                double bpm, int beatsPerMeasure);

}  // namespace beatbench::slice
