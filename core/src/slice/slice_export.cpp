// SPDX-License-Identifier: GPL-3.0-only
// SliceExport 实现（见 hpp 注释）。纯计算：不含音频解码/文件 IO（那些在 audio/ 层，
// 由 GUI 工作台组合）。build_placement_raw 复用 bms::write_bms 保证输出正确性。
#include "beatbench/core/slice/SliceExport.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <set>
#include <string>
#include <vector>

#include "beatbench/core/bms/BmsCodec.hpp"
#include "beatbench/core/Chart.hpp"
#include "beatbench/core/Event.hpp"
#include "beatbench/core/Lane.hpp"

namespace beatbench::slice {

namespace {
/// BMS 36 进制两位数上限（ZZ = 1295）；超此需 #BASE 62 或 3 位 id（暂不支持自动化）。
constexpr std::uint32_t kMaxWavId = 1295;
}  // namespace

std::vector<std::uint32_t> allocate_wav_ids(
    const std::vector<std::uint32_t>& occupied, std::uint32_t start_id,
    int count) {
    std::vector<std::uint32_t> out;
    if (count <= 0) return out;
    std::set<std::uint32_t> taken(occupied.begin(), occupied.end());
    std::uint32_t cand = start_id;
    if (cand == 0 || cand > kMaxWavId) cand = 1;
    while (static_cast<int>(out.size()) < count && cand <= kMaxWavId) {
        if (taken.insert(cand).second) out.push_back(cand);
        ++cand;
    }
    return out;
}

std::vector<SliceExportItem> build_export_layout(
    const std::vector<Slice>& slices, const std::vector<bool>& enabled,
    const std::vector<std::uint32_t>& occupied, std::uint32_t start_id,
    const std::string& baseName, double bpm, int beatsPerMeasure,
    int subdivision, double offset, int width) {
    std::vector<SliceExportItem> items;
    // 启用切片数 → 分配 #WAV id（只分配给「放置开关=开」的切片；未启用仍列出但 wavId=0）
    int need = 0;
    for (std::size_t i = 0; i < slices.size(); ++i)
        if ((i < enabled.size()) && enabled[i]) ++need;
    const auto ids = allocate_wav_ids(occupied, start_id, need);
    int id_i = 0;

    const int beatsPerMeas = std::max(1, beatsPerMeasure);
    const int sub = std::max(1, subdivision);
    const std::int64_t den = static_cast<std::int64_t>(beatsPerMeas) * sub;

    for (std::size_t i = 0; i < slices.size(); ++i) {
        const bool en = (i < enabled.size()) && enabled[i];
        const Slice& s = slices[i];
        SliceExportItem it;
        it.sliceIndex = static_cast<int>(i);
        it.enabled = en;
        if (en) {
            it.wavId = (id_i < static_cast<int>(ids.size())) ? ids[static_cast<std::size_t>(id_i)] : 0;
            ++id_i;
        }
        char nameBuf[64];
        std::snprintf(nameBuf, sizeof(nameBuf), "%s_%0*d.wav", baseName.c_str(),
                      std::max(1, width), static_cast<int>(i));
        it.fileName = nameBuf;

        // 拍位换算：beat = (startSec - offset) * bpm / 60；吸附到细分网格
        double beat = (s.startSec - offset) * bpm / 60.0;
        if (beat < 0.0) beat = 0.0;
        std::int64_t measure = static_cast<std::int64_t>(std::floor(beat / beatsPerMeas + 1e-9));
        double fracBeat = beat - static_cast<double>(measure) * beatsPerMeas;
        if (fracBeat < 0.0) fracBeat = 0.0;
        std::int64_t num = static_cast<std::int64_t>(std::llround(fracBeat * sub));
        if (num >= den) {  // 恰落小节边界 → 归入下一小节 pos 0
            ++measure;
            num = 0;
        }
        it.measure = static_cast<int>(measure);
        it.pos = Rational(num, den);
        items.push_back(std::move(it));
    }
    return items;
}

std::string build_placement_raw(const std::vector<SliceExportItem>& items,
                                double bpm, int beatsPerMeasure) {
    Chart c;
    c.id_base = IdBase::Base36;
    char bpmBuf[32];
    std::snprintf(bpmBuf, sizeof(bpmBuf), "%.6g", bpm);
    c.meta["BPM"] = bpmBuf;
    const int beatsPerMeas = std::max(1, beatsPerMeasure);

    // 记录需要 ch02 小节长度的 measure
    std::set<std::uint32_t> needs_ch02;
    for (const auto& it : items) {
        if (it.wavId == 0) continue;
        char pathBuf[512];
        std::snprintf(pathBuf, sizeof(pathBuf), "%s", it.fileName.c_str());
        c.samples[{SampleKind::Wav, it.wavId}].file = it.fileName;
        if (!it.enabled) continue;
        Event<Note> n;
        n.measure = static_cast<std::uint32_t>(it.measure);
        n.pos = it.pos;
        n.value.lane = Lane{0, LaneKind::Bgm, 0};
        n.value.sample.id = it.wavId;
        n.value.sub_line = 0;
        c.notes.push_back(n);
        if (beatsPerMeas != 4) needs_ch02.insert(n.measure);
    }
    for (const std::uint32_t m : needs_ch02) {
        Event<MeasureLen> ml;
        ml.measure = m;
        ml.pos = Rational(0, 1);
        ml.value.beats = static_cast<double>(beatsPerMeas);
        c.measure_events.push_back(ml);
    }
    // 确保 (measure, pos) 有序（writer 依赖）
    std::sort(c.notes.begin(), c.notes.end());

    const std::string full = bms::write_bms(c);

    // 筛出 `#WAVxx file` 定义 + ch01/ch02 数据行
    // 数据行头 = `#` + 3 位小节 (idx 1-3) + 2 位通道 (idx 4-5) + `:` (idx 6)，
    // 如 `#00001:…`（ch01 BGM）/ `#00002:…`（ch02 小节长）。
    std::string out;
    std::size_t pos = 0;
    while (pos < full.size()) {
        const std::size_t nl = full.find('\n', pos);
        const std::string line =
            full.substr(pos, nl == std::string::npos ? std::string::npos : nl - pos);
        pos = (nl == std::string::npos) ? full.size() : nl + 1;
        if (line.size() < 8 || line[0] != '#') continue;
        const bool is_wav_def = line.rfind("#WAV", 0) == 0;
        // ch01 数据行：`#NNN01:`；ch02 小节长：`#NNN02:`
        const bool is_data = line[4] == '0' && (line[5] == '1' || line[5] == '2') &&
                             line[6] == ':';
        if (is_wav_def || is_data) {
            out += line;
            out.push_back('\n');
        }
    }
    return out;
}

}  // namespace beatbench::slice
