// SPDX-License-Identifier: GPL-3.0-only
// M6.3 分片导出布局单测：id 分配 + 拍位换算 + 可复制 BMS raw（纯计算，无音频/无 Qt）。
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "beatbench/core/slice/SliceExport.hpp"

namespace {

using beatbench::slice::SliceExportItem;
using beatbench::slice::Slice;
using beatbench::slice::allocate_wav_ids;
using beatbench::slice::build_export_layout;
using beatbench::slice::build_placement_raw;

std::vector<Slice> make_slices(std::vector<double> starts, std::vector<double> ends) {
    std::vector<Slice> out;
    for (std::size_t i = 0; i < starts.size(); ++i) {
        Slice s;
        s.index = static_cast<int>(i);
        s.startSec = starts[i];
        s.endSec = ends[i];
        s.kind = "grid";
        out.push_back(s);
    }
    return out;
}

TEST(SliceExportTest, AllocateSkipsOccupiedAndStart) {
    // 起始 1，occupied {1,2}，count 3 → {3,4,5}
    const auto ids = allocate_wav_ids({1, 2}, 1, 3);
    ASSERT_EQ(ids.size(), 3u);
    EXPECT_EQ(ids[0], 3u);
    EXPECT_EQ(ids[1], 4u);
    EXPECT_EQ(ids[2], 5u);
    // start_id=0 → 归 1；count=0 → 空
    EXPECT_EQ(allocate_wav_ids({}, 0, 0).size(), 0u);
    const auto ids2 = allocate_wav_ids({}, 0, 2);
    ASSERT_EQ(ids2.size(), 2u);
    EXPECT_EQ(ids2[0], 1u);
}

TEST(SliceExportTest, LayoutAssignsIdsAndPositions) {
    // bpm=120, 4/4, 细分4, offset0 → beat = startSec*2
    const auto slices = make_slices({0.0, 0.5, 2.0, 2.5}, {0.125, 0.625, 2.125, 2.625});
    // 只启用前两个
    const std::vector<bool> enabled = {true, true, false, false};
    const auto items = build_export_layout(slices, enabled, {}, 1, "slice", 120.0, 4, 4, 0.0);
    ASSERT_EQ(items.size(), 4u);
    // 文件命名按切片序号补零
    EXPECT_EQ(items[0].fileName, "slice_000.wav");
    EXPECT_EQ(items[3].fileName, "slice_003.wav");
    // 启用者分得 id：start=1,2；未启用 wavId=0
    EXPECT_EQ(items[0].wavId, 1u);
    EXPECT_EQ(items[1].wavId, 2u);
    EXPECT_EQ(items[2].wavId, 0u);
    // 拍位：startSec=0 → beat0 → measure0 pos0；startSec=0.5 → beat1 → pos 1/4
    EXPECT_EQ(items[0].measure, 0);
    EXPECT_EQ(items[0].pos, beatbench::Rational(0, 1));
    EXPECT_EQ(items[1].measure, 0);
    EXPECT_EQ(items[1].pos, beatbench::Rational(1, 4));
    // startSec=2.0 → beat4 → measure1 pos0
    EXPECT_EQ(items[2].measure, 1);
    EXPECT_EQ(items[2].pos, beatbench::Rational(0, 1));
}

TEST(SliceExportTest, PlacementRawHasWavDefAndCh01Line) {
    const auto slices = make_slices({0.0, 0.5}, {0.125, 0.625});
    const std::vector<bool> enabled = {true, true};
    const auto items = build_export_layout(slices, enabled, {1}, 1, "slice", 120.0, 4, 4, 0.0);
    // occupied {1} → 从 start 1 起跳 1 → id 2,3
    ASSERT_EQ(items.size(), 2u);
    EXPECT_EQ(items[0].wavId, 2u);
    EXPECT_EQ(items[1].wavId, 3u);
    const auto raw = build_placement_raw(items, 120.0, 4);
    // ch01 数据行：#00001:...（含 id 文本 02 / 03）
    EXPECT_NE(raw.find("#00001:"), std::string::npos);
    EXPECT_NE(raw.find("02"), std::string::npos);
    EXPECT_NE(raw.find("03"), std::string::npos);
}

}  // namespace
