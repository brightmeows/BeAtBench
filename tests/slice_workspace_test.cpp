// SPDX-License-Identifier: GPL-3.0-only
// Query tests use in-memory sessions only; no AudioEngine, files, or rendering.
#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <utility>

#include "bridge/ChartSession.hpp"
#include "bridge/SliceWorkspace.hpp"
#include "beatbench/core/Chart.hpp"
#include "beatbench/core/bms/BmsUtil.hpp"
#include "beatbench/core/edit/SessionRegistry.hpp"

namespace {

class SliceWorkspaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& registry = beatbench::edit::session_registry();
        previousSession_ = registry.active_id();
        ASSERT_TRUE(registry.create(sessionId_));
        workspace_.setChartSession(&chartSession_);
    }

    void TearDown() override {
        auto& registry = beatbench::edit::session_registry();
        EXPECT_TRUE(registry.activate(previousSession_));
        EXPECT_TRUE(registry.close(sessionId_));
    }

    void loadChart(beatbench::Chart chart) {
        beatbench::edit::session_registry().active().load(std::move(chart));
        chartSession_.refresh();
    }

    static void addWav(beatbench::Chart& chart, std::uint32_t id) {
        chart.samples[{beatbench::SampleKind::Wav, id}].file = "sample.wav";
    }

    const std::string sessionId_ = "slice-workspace-test";
    std::string previousSession_;
    beatbench::app::ChartSession chartSession_;
    beatbench::app::SliceWorkspace workspace_;
};

TEST_F(SliceWorkspaceTest, MissingAndEmptyChartStartAtOne) {
    workspace_.setChartSession(nullptr);
    EXPECT_EQ(workspace_.nextFreeWavId(), 1);
    workspace_.setChartSession(&chartSession_);
    EXPECT_EQ(workspace_.nextFreeWavId(), 1);
    loadChart({});
    EXPECT_TRUE(chartSession_.hasChart());
    EXPECT_EQ(workspace_.nextFreeWavId(), 1);
}

TEST_F(SliceWorkspaceTest, OccupiedWavIdsUseLowestGapWithoutMutation) {
    beatbench::Chart chart;
    addWav(chart, 1);
    addWav(chart, 2);
    addWav(chart, 4);
    loadChart(std::move(chart));
    const auto before = workspace_.occupiedWavIds();
    ASSERT_EQ(before.size(), 3);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(workspace_.nextFreeWavId(), 3);
    }
    EXPECT_EQ(workspace_.occupiedWavIds(), before);
    EXPECT_EQ(beatbench::edit::session_registry().active().undo_depth(), 0u);
}

TEST_F(SliceWorkspaceTest, NonWavDefinitionsDoNotOccupyWavIds) {
    beatbench::Chart chart;
    chart.samples[{beatbench::SampleKind::Bmp, 1}].file = "image.png";
    chart.samples[{beatbench::SampleKind::Bpm, 1}].value = "120";
    chart.samples[{beatbench::SampleKind::Stop, 1}].value = "48";
    loadChart(chart);
    EXPECT_TRUE(workspace_.occupiedWavIds().empty());
    EXPECT_EQ(workspace_.nextFreeWavId(), 1);

    addWav(chart, 1);
    loadChart(std::move(chart));
    EXPECT_EQ(workspace_.nextFreeWavId(), 2);
}

TEST_F(SliceWorkspaceTest, AaBoundaryUsesNumericOccupancy) {
    const auto aa = beatbench::bms::c36_to_u32("AA", 2);
    ASSERT_EQ(aa, 370u);
    beatbench::Chart chart;
    for (std::uint32_t id = 1; id < aa; ++id) addWav(chart, id);
    loadChart(chart);
    EXPECT_EQ(workspace_.nextFreeWavId(), static_cast<int>(aa));

    addWav(chart, aa);
    loadChart(std::move(chart));
    EXPECT_EQ(workspace_.nextFreeWavId(), static_cast<int>(aa + 1));
}

TEST_F(SliceWorkspaceTest, FullBase36TableDoesNotWrapToOccupiedId) {
    beatbench::Chart chart;
    for (std::uint32_t id = 1; id <= 1295; ++id) addWav(chart, id);
    loadChart(std::move(chart));
    // Preserve the numeric query contract; export capacity policy is separate.
    EXPECT_EQ(workspace_.nextFreeWavId(), 1296);
}

TEST_F(SliceWorkspaceTest, SessionSwitchReadsCurrentChart) {
    beatbench::Chart chart;
    addWav(chart, 1);
    loadChart(std::move(chart));
    ASSERT_EQ(workspace_.nextFreeWavId(), 2);

    auto& registry = beatbench::edit::session_registry();
    const std::string other = "slice-workspace-other";
    ASSERT_TRUE(registry.create(other));
    registry.active().load(beatbench::Chart{});
    chartSession_.refresh();
    EXPECT_EQ(workspace_.nextFreeWavId(), 1);

    EXPECT_TRUE(registry.activate(sessionId_));
    chartSession_.refresh();
    EXPECT_EQ(workspace_.nextFreeWavId(), 2);
    EXPECT_TRUE(registry.close(other));
}

}  // namespace
