// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "beatbench/core/command/Command.hpp"

namespace beatbench::cmd {

/// 注册 MIDI 相关命令（M6.1：midi.parse）。
/// 由 register_builtin_commands 统一调用，注册表共享；亦可单独调用（测试注入）。
void register_midi_commands(Registry& registry);

}  // namespace beatbench::cmd
