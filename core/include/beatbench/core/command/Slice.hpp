// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "beatbench/core/command/Command.hpp"

namespace beatbench::cmd {

/// 注册切片相关命令（M6.2：slice.detect——位置源 → 切片表）。
/// 由 register_builtin_commands 统一调用；亦可单独调用（测试注入）。
void register_slice_commands(Registry& registry);

}  // namespace beatbench::cmd
