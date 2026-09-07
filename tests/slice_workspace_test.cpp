// SPDX-License-Identifier: GPL-3.0-only
// Query tests use in-memory sessions only; export conflict tests write a temp dir
// and decode a short synthetic wav. AudioEngine is linked but never opened.
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "bridge/ChartSession.hpp"
#include "bridge/SliceWorkspace.hpp"
#include "beatbench/core/Chart.hpp"
#include "beatbench/core/bms/BmsUtil.hpp"
#include "beatbench/core/edit/SessionRegistry.hpp"
#include "beatbench/core/slice/Slice.hpp"

namespace {

class SliceWorkspaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& registry = beatbench::edit::session_registry();
        previousSession_ = registry.active_id();
        ASSERT_TRUE(registry.create(sessionId_));
        workspace_.setChartSession(&chartSession_);
    }

    void loadChart(beatbench::Chart chart) {
        beatbench::edit::session_registry().active().load(std::move(chart));
        chartSession_.refresh();
    }

    static void addWav(beatbench::Chart& chart, std::uint32_t id) {
        chart.samples[{beatbench::SampleKind::Wav, id}].file = "sample.wav";
    }

    static void touchFile(const std::filesystem::path& path, const std::string& body) {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
        out << body;
    }

    static std::string readFile(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        return std::string((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
    }

    static std::string writeSineWav(const std::filesystem::path& path) {
        FILE* f = std::fopen(path.string().c_str(), "wb");
        EXPECT_TRUE(f != nullptr);
        const int sampleRate = 8000;
        const int channels = 1;
        const int frames = 800;  // 0.1s
        auto write16 = [&](std::uint16_t v) { std::fwrite(&v, 2, 1, f); };
        auto write32 = [&](std::uint32_t v) { std::fwrite(&v, 4, 1, f); };
        std::fwrite("RIFF", 1, 4, f);
        write32(36 + static_cast<std::uint32_t>(frames) * channels * 2);
        std::fwrite("WAVE", 1, 4, f);
        std::fwrite("fmt ", 1, 4, f);
        write32(16);
        write16(1);
        write16(static_cast<std::uint16_t>(channels));
        write32(static_cast<std::uint32_t>(sampleRate));
        write32(static_cast<std::uint32_t>(sampleRate * channels * 2));
        write16(static_cast<std::uint16_t>(channels * 2));
        write16(16);
        std::fwrite("data", 1, 4, f);
        write32(static_cast<std::uint32_t>(frames * channels * 2));
        for (int i = 0; i < frames; ++i) {
            const auto s = static_cast<std::int16_t>(
                std::lround(12000 * std::sin(2.0 * 3.14159265358979323846 * 440.0 * i / sampleRate)));
            std::fwrite(&s, 2, 1, f);
        }
        std::fclose(f);
        return path.string();
    }

    std::filesystem::path makeTempDir(const std::string& tag) {
        const auto dir = std::filesystem::temp_directory_path() /
                         ("bb_slice_export_" + tag + "_" +
                          std::to_string(reinterpret_cast<std::uintptr_t>(this)));
        std::filesystem::remove_all(dir);
        std::filesystem::create_directories(dir);
        tempDirs_.push_back(dir);
        return dir;
    }

    void TearDown() override {
        auto& registry = beatbench::edit::session_registry();
        EXPECT_TRUE(registry.activate(previousSession_));
        EXPECT_TRUE(registry.close(sessionId_));
        for (const auto& dir : tempDirs_) std::filesystem::remove_all(dir);
    }

    const std::string sessionId_ = "slice-workspace-test";
    std::string previousSession_;
    beatbench::app::ChartSession chartSession_;
    beatbench::app::SliceWorkspace workspace_;
    std::vector<std::filesystem::path> tempDirs_;
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

TEST_F(SliceWorkspaceTest, PreviewFindsCollisionsWithoutWriting) {
    const auto dir = makeTempDir("preview");
    const QString outDir = QString::fromStdString(dir.string());
    beatbench::slice::Slice a;
    a.index = 0;
    a.startSec = 0.0;
    a.endSec = 0.05;
    a.kind = "grid";
    beatbench::slice::Slice b = a;
    b.index = 1;
    b.startSec = 0.05;
    b.endSec = 0.10;
    workspace_.setSlicesForTest({a, b}, {true, true});

    auto empty = workspace_.previewExportFiles(outDir, QStringLiteral("slice"));
    ASSERT_TRUE(empty.value(QStringLiteral("ok")).toBool());
    EXPECT_EQ(empty.value(QStringLiteral("collisions")).toStringList().size(), 0);
    EXPECT_EQ(empty.value(QStringLiteral("nextContinueIndex")).toInt(), 0);

    touchFile(dir / "slice_000.wav", "OLD000");
    touchFile(dir / "slice_001.wav", "OLD001");
    auto hit = workspace_.previewExportFiles(outDir, QStringLiteral("slice"));
    ASSERT_TRUE(hit.value(QStringLiteral("ok")).toBool());
    const auto collisions = hit.value(QStringLiteral("collisions")).toStringList();
    ASSERT_EQ(collisions.size(), 2);
    EXPECT_TRUE(collisions.contains(QStringLiteral("slice_000.wav")));
    EXPECT_TRUE(collisions.contains(QStringLiteral("slice_001.wav")));
    EXPECT_EQ(hit.value(QStringLiteral("nextContinueIndex")).toInt(), 2);
    EXPECT_EQ(readFile(dir / "slice_000.wav"), "OLD000");
}

TEST_F(SliceWorkspaceTest, ExportErrorCancelOverwriteAndContinue) {
    const auto dir = makeTempDir("export");
    const auto src = dir / "src.wav";
    ASSERT_TRUE(workspace_.loadAudioFileSyncForTest(QString::fromStdString(writeSineWav(src))));
    beatbench::slice::Slice a;
    a.index = 0;
    a.startSec = 0.0;
    a.endSec = 0.04;
    a.kind = "grid";
    beatbench::slice::Slice b = a;
    b.index = 1;
    b.startSec = 0.04;
    b.endSec = 0.08;
    workspace_.setSlicesForTest({a, b}, {true, true});
    const QString outDir = QString::fromStdString(dir.string());
    touchFile(dir / "slice_000.wav", "OLD000");
    touchFile(dir / "slice_001.wav", "OLD001");

    auto denied = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                          0.0, false, QStringLiteral("error"));
    EXPECT_FALSE(denied.value(QStringLiteral("ok")).toBool());
    EXPECT_EQ(readFile(dir / "slice_000.wav"), "OLD000");
    EXPECT_EQ(readFile(dir / "slice_001.wav"), "OLD001");
    EXPECT_FALSE(std::filesystem::exists(dir / "slice_002.wav"));

    auto continued = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                             0.0, false, QStringLiteral("continue"));
    ASSERT_TRUE(continued.value(QStringLiteral("ok")).toBool()) << continued.value("error").toString().toStdString();
    EXPECT_EQ(readFile(dir / "slice_000.wav"), "OLD000");
    EXPECT_EQ(readFile(dir / "slice_001.wav"), "OLD001");
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_002.wav"));
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_003.wav"));
    const auto files = continued.value(QStringLiteral("files")).toStringList();
    EXPECT_TRUE(files.contains(QStringLiteral("slice_002.wav")));
    EXPECT_TRUE(files.contains(QStringLiteral("slice_003.wav")));
    EXPECT_NE(continued.value(QStringLiteral("raw")).toString().indexOf(QStringLiteral("slice_002.wav")), -1);

    auto overwritten = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                               0.0, false, QStringLiteral("overwrite"));
    ASSERT_TRUE(overwritten.value(QStringLiteral("ok")).toBool()) << overwritten.value("error").toString().toStdString();
    EXPECT_NE(readFile(dir / "slice_000.wav"), "OLD000");
    EXPECT_NE(readFile(dir / "slice_001.wav"), "OLD001");
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_002.wav"));
}

TEST_F(SliceWorkspaceTest, ExportFileNumbersStayIndependentFromWavIds) {
    const auto dir = makeTempDir("ids");
    const auto src = dir / "src.wav";
    ASSERT_TRUE(workspace_.loadAudioFileSyncForTest(QString::fromStdString(writeSineWav(src))));
    beatbench::Chart chart;
    addWav(chart, 1);
    addWav(chart, 2);
    loadChart(std::move(chart));
    beatbench::slice::Slice a;
    a.index = 0;
    a.startSec = 0.0;
    a.endSec = 0.04;
    a.kind = "grid";
    workspace_.setSlicesForTest({a}, {true});
    const QString outDir = QString::fromStdString(dir.string());
    auto first = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                         0.0, false, QStringLiteral("overwrite"));
    ASSERT_TRUE(first.value(QStringLiteral("ok")).toBool()) << first.value("error").toString().toStdString();
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_000.wav"));
    EXPECT_NE(first.value(QStringLiteral("raw")).toString().indexOf(QStringLiteral("#WAV03")), -1);

    auto second = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                          0.0, false, QStringLiteral("continue"));
    ASSERT_TRUE(second.value(QStringLiteral("ok")).toBool()) << second.value("error").toString().toStdString();
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_000.wav"));
    EXPECT_TRUE(std::filesystem::exists(dir / "slice_001.wav"));
    EXPECT_NE(second.value(QStringLiteral("raw")).toString().indexOf(QStringLiteral("#WAV03")), -1);
}

TEST_F(SliceWorkspaceTest, PartialSelectionExportsFromZeroNotTableIndex) {
    const auto dir = makeTempDir("partial");
    const auto src = dir / "src.wav";
    ASSERT_TRUE(workspace_.loadAudioFileSyncForTest(QString::fromStdString(writeSineWav(src))));
    std::vector<beatbench::slice::Slice> slices;
    for (int i = 0; i < 5; ++i) {
        beatbench::slice::Slice s;
        s.index = i;
        s.startSec = 0.01 * i;
        s.endSec = 0.01 * i + 0.01;
        s.kind = "manual";
        slices.push_back(s);
    }
    workspace_.setSlicesForTest(std::move(slices), {false, false, false, true, false});
    const QString outDir = QString::fromStdString(dir.string());
    touchFile(dir / "slice_000.wav", "OLD000");
    touchFile(dir / "slice_003.wav", "OLD003");

    auto preview = workspace_.previewExportFiles(outDir, QStringLiteral("slice"));
    ASSERT_TRUE(preview.value(QStringLiteral("ok")).toBool());
    const auto collisions = preview.value(QStringLiteral("collisions")).toStringList();
    ASSERT_EQ(collisions.size(), 1);
    EXPECT_TRUE(collisions.contains(QStringLiteral("slice_000.wav")));
    EXPECT_FALSE(collisions.contains(QStringLiteral("slice_003.wav")));
    EXPECT_EQ(preview.value(QStringLiteral("nextContinueIndex")).toInt(), 4);

    auto overwritten = workspace_.exportSlices(120, 4, 4, 1, 1, outDir, QStringLiteral("slice"),
                                               0.0, false, QStringLiteral("overwrite"));
    ASSERT_TRUE(overwritten.value(QStringLiteral("ok")).toBool())
        << overwritten.value("error").toString().toStdString();
    EXPECT_NE(readFile(dir / "slice_000.wav"), "OLD000");
    EXPECT_EQ(readFile(dir / "slice_003.wav"), "OLD003");
    const auto files = overwritten.value(QStringLiteral("files")).toStringList();
    ASSERT_EQ(files.size(), 1);
    EXPECT_EQ(files[0], QStringLiteral("slice_000.wav"));
}

}  // namespace
