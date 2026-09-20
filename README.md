# BeAtBench

面向 BMS 的开源跨平台谱面编辑器

技术栈: C++20 + Qt 6 Quick/QML + PortAudio + CMake

目前还没支持ASIO

**许可：GPL-3.0**（见 `LICENSE`）。

## 功能特性

### 已完成（M1-M6，2026-09；0.3.0 发布准备）

- **BMS 读写**：完整解析/写出 .bms/.bme/.pms，支持 UTF-8/SJIS 编码、base62 id、#BASE 62 大小写敏感、iBMSC 式输出（定义表顺序/分割线注释）
- **谱面编辑**：note 放置/移动/删除、拖拽/框选/点选、LN（长音，LNTYPE 1/2）/地雷、单点<->LN 转换、量化/镜像/旋转变换、undo/redo、剪贴板（BMS 原始行，外部工具兼容）
- **时间轴**：BPM/STOP/节拍事件编辑（点放/列表/改值/删除）、可视化竖向时间轴、BGM 轨展开分列（按 ch01 行序）、BGA 图层列（更多轨道）
- **元信息编辑**：TITLE/ARTIST/BPM/PLAYER 等全字段（含"更多字段"/"扩展代码"raw 兜底）、下拉可选（label/value 分离）、#BASE 62
- **采样管理**：#WAV/#BMP/#BPM/#STOP 定义表管理、采样面板（搜索/分组/排序/索引/缺失徽标）、note 引用采样切换
- **BGA/BMP**：BGA 图层事件编辑、#BMP 定义管理
- **多文档会话**：SessionRegistry，多标签页前瞻
- **lint**：解析诊断 + 音符检查（缺失采样/重叠 note/悬挂 LN 等），打开即显示
- **快捷键/动作注册表**：动作走 UiActionRegistry（`invoke` 为菜单/工具条/Shortcut 入口），菜单/工具条按注册表枚举；用户快捷键落盘到 QSettings（优先于皮肤 `keymap.json`），设置页确定后立即生效；2026-09 补 Space / Ctrl+R / Ctrl+, / Esc 与**作用域隔离**（编辑页/切音页同键位各绑一次，全局动作与页面动作互斥）
- **皮肤（L1）**：theme.json token 覆写（颜色/字号/字体/圆角/note 样式/键轨着色）、内置皮肤（Aurora/Linear/OsuLight 浅色/Win10 直角）、运行时切换（菜单"视图->皮肤"）、皮肤可携带 keymap
- **音频（M4）**：采样列表点击试听 + 音频设置页 + 采样解码缓存 + 离线渲染（ChartRenderer / `cli render` / 编辑器 Space，写 `.render.wav`）+ 波形显示（右侧垂直波形条 + 秒标尺）+ 编辑增量重渲染（PortAudio WASAPI 输出 + miniaudio 解码 wav/ogg/mp3/flac）
- **随时播放（M5）**：Space 播放/暂停、PcmEngine 零拷贝、播放时钟、编辑即停；播放头红线（视口光标）+ 视口跟随 + A-B 循环 + seek（点秒标尺/波形条拖动 scrub）；note 放置/移动鼠标预览 ghost
- **编辑增强（小节/变拍）**：BMS 02 通道**小节长度编辑**（时间轴「小节长」页：添加/改值/删除每小节拍数）+ **变拍高渲染**（按拍数等比，4/4 基准，左标尺非 4/4 标 `×N`）+ **「加一小节」**追加编辑小节 + **File→新建谱面**（空谱面从零编辑）；播放/跟随/seek/秒标尺同步正确；编辑/撤销后右侧波形不再消失
- **命令即接口**：GUI/CLI/脚本共用 JSON 命令协议（doc/06 §3），40+ 命令
- **切音工作台（M6）**：参考音频/MIDI 导入（offset 微调、播控）→ 切片（网格/MIDI 源、「到下一起点」模式、手动切分点）→ 导出（fade 分片、`#WAV` id 分配、可复制 BMS raw、撞名须选覆盖/续号/取消）+ 切片编辑（双击行改边界）+ 切音页独立撤销 + 编辑页 Ctrl+V 整段粘贴（含 `#WAV` 定义/小节长）+ 切音页状态栏摘要（音频/播放/视口/光标/切片/MIDI）+ **Base62 粘贴保护**（raw 带 `#BASE 62` 标记，粘进未声明 Base62 的谱面前确认）

### 计划中

> **状态与排期的唯一权威是 [`doc/04` §7 里程碑速览](doc/04-开发手册.md)**（含 `⏳/🚧/—` 口径与前置）。
> 本节只列下一个方向，不重复里程碑表，避免两处维护漂移。

- **M7 项目化工作流**：文件夹即项目——一首歌多个谱面一次导入；项目面板、多谱面会话、谱面对比、批量操作、打包校验、`#WAV` 占用视图。设计见 [`doc/07`](doc/07-M7项目化工作流设计.md)。
- **M8 边界与协议**：SliceWorkspace 边界拆分、CSV 时标导出、MIDI tempo 接入、codec 能力声明、跨平台 CI。
- **M9 / M10+**：bmson、项目/工作区持久化、`#RANDOM/#IF` 结构化 AST；i18n、试玩判定、性能基准、外部预览适配器、L2 布局皮肤。

## 状态

**M1-M6 已完成**（2026-09）：BMS codec + timing + CLI + QML 编辑器（编辑/时间轴/元信息/采样/BGA/lint/剪贴板/多文档/动作注册表/皮肤 L1）→ **M4 音频**（单发试听/设置页/解码缓存/离线渲染/波形/秒标尺/增量重渲染）→ **M5 随时播放**（Space 播放/暂停、PcmEngine 零拷贝、播放时钟、编辑即停）+ 播放头红线/视口跟随/A-B 循环/seek + note 编辑增强（鼠标预览 ghost、多选拖动、BGM 相对距离、`bgm_line→sub_line` 泛化）+ 编辑增强（**02 小节长度编辑 + 变拍高 + 加一小节 + 新建谱面**、编辑/撤销波形不消失）→ **M6 切音工作台**（导入/切分/导出/手动切分点/切片编辑/剪贴板整段粘贴，见 doc/04 §6）。
**0.3.0 已发布**（tag [`v0.3.0`](https://github.com/wufe8/BeAtBench/releases/tag/v0.3.0)）：发布包 M6/bmson 文案与过期本机路径注释修正、未来功能状态表、`command/` 与 `commands/` 遗留头核实并删除、JSON 命令契约测试（含 CLI↔GUI 信封一致性）、版本推进与干净构建验证。
**0.3.1 已发布**（tag [`v0.3.1`](https://github.com/wufe8/BeAtBench/releases/tag/v0.3.1)）：修复 v0.1.0–v0.3.0 发布包未随包分发内置皮肤 `skins/`（「视图→皮肤」四项失效）；皮肤目录解析增加 exe 目录回退，不再依赖工作目录；新增拖拽入窗口 / 拖到 exe 图标 / 双击关联打开谱面与音频/MIDI。详见 `CHANGELOG.md`。
测试以源码 `TEST()` 计数为准（core 317 + 音频 55 + Qt 桥层 54）；真实谱面集缺失时部分 SKIP，不要把某一天的 PASS 数写死。详见 `doc/04` §6。

当前里程碑任务见 `doc/04-开发手册.md`（简版现状；开发历史/踩坑细节在 `local/doc/04-开发手册-完整版.md`，gitignore）；皮肤系统后续计划见 `local/doc/10-主题与皮肤路线.md`（gitignore）。

## 文档导航

| 文档 | 内容 |
|---|---|
| [`doc/README.md`](doc/README.md) | **文档地图**：按任务查该读哪份文档、doc/local 分布约定 |
| [`doc/04-开发手册.md`](doc/04-开发手册.md) | **新会话引导（简版）**：仓库布局、构建命令、代码约定、当前状态；开发历史见 `local/doc/04-开发手册-完整版.md` |
| [`doc/02-核心模型与调用架构.md`](doc/02-核心模型与调用架构.md) | **模型/架构权威**：格式无关模型、命令即接口、切音工作台设计、note 移动机制 |
| [`doc/06-插件体系与时间单位设计.md`](doc/06-插件体系与时间单位设计.md) | **命令 JSON 协议正式规格**、插件分层、时间单位边界 |
| [`doc/07-M7项目化工作流设计.md`](doc/07-M7项目化工作流设计.md) | **M7 设计**：文件夹即项目、多谱面对比/批量/打包、`project.*` 命令 |
| [`doc/08-QML技术选型与皮肤系统设计.md`](doc/08-QML技术选型与皮肤系统设计.md) | GUI 栈决策（Qt Quick/QML）+ 分层皮肤系统（L1/L2/L3） |
| [`doc/09-操作注册设计.md`](doc/09-操作注册设计.md) | UI 动作注册表（换肤/快捷键前置） |
| [`doc/05-前端界面设计构思.md`](doc/05-前端界面设计构思.md) | 页面式信息架构、区域设计、设计 token、术语 |
| [`doc/BMS文件分析笔记.md`](doc/BMS文件分析笔记.md) | BMS 格式逆向笔记 |

> `local/` 目录（个人笔记、样本谱面）已 gitignore，不上传远程。

## 快速构建

### CLI + 测试（MSVC）

```powershell
# 配置 + 构建（需联网拉 GoogleTest）
cmake -S . -B build -DBEATBENCH_BUILD_TESTS=ON
cmake --build build --config Debug --parallel

# 运行测试
ctest --test-dir build -C Debug --output-on-failure

# 快速回归（跳过真实谱面测试，<1s）
set BB_SKIP_REAL=1
build\tests\Debug\beatbench_tests.exe
```

### GUI（MinGW + Qt 6）

> 需自备 Qt 6（含 Quick/QuickControls2）与匹配的 MinGW 编译器。下面的 `QT_PREFIX`/
> `MINGW_BIN` 是**你自己的安装路径**，换成你的实际位置即可（示例值来自本机，仅作格式参考）。

```bash
# 改为你自己的路径（Windows 下可用 /c/... 或 C:/... 写法）
export QT_PREFIX=/c/Qt/6.8.0/mingw_64
export MINGW_BIN=/c/Qt/Tools/mingw1310_64/bin

# 配置（Git Bash；用 `-G "MinGW Makefiles"` 或 Ninja 均可）
cmake -S . -B build-gui -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX" \
  -DCMAKE_CXX_COMPILER="$MINGW_BIN/g++.exe" \
  -DCMAKE_MAKE_PROGRAM="$MINGW_BIN/ninja.exe" \
  -DBEATBENCH_BUILD_TESTS=OFF

# 构建
cmake --build build-gui --target beatbench

# 部署 Qt DLL 到 exe 旁（--qmldir 必须指向 QML 源码，否则缺 QuickControls 插件）
"$QT_PREFIX/bin/windeployqt.exe" --qmldir app/qml build-gui/app/beatbench.exe

# 运行
build-gui/app/beatbench.exe
```

## 仓库布局

```
core/       格式无关模型 + BMS codec + timing + JSON 命令框架（零 Qt 依赖）
audio/      音频层（零 Qt）：PortAudio 后端 + miniaudio 解码 + SamplePlayer 混音内核（M4.1）
cli/        beatbench-cli 批处理入口（info/check/convert/version + --json 协议）
app/        Qt Quick/QML GUI（C++ bridge + QML 界面）
  bridge/   CommandDispatcher + ChartSession + ThemeManager + UiActionRegistry + AudioEngine
  qml/      Main.qml + pages/ + components/
tests/      GoogleTest 单元测试
third_party/  vendored 头文件（miniaudio.h）
doc/        设计文档与开发手册
```

## 快速上手（CLI）

```powershell
# 人类可读子命令
build/cli/Debug/beatbench-cli.exe info 谱面.bms      # 元信息/定义表/事件统计
build/cli/Debug/beatbench-cli.exe check 谱面.bms     # 解析诊断 + lint
build/cli/Debug/beatbench-cli.exe convert 输入.bms 输出.bms --encoding utf8

# 命令 JSON 协议（GUI/脚本/插件同款入口，契约见 doc/06 §3）
build/cli/Debug/beatbench-cli.exe --json '{"command":"info","args":{"path":"谱面.bms"}}'
```

## 调试参数

GUI 支持以下命令行参数（配合 `--screenshot` 做视觉验收）：

```bash
build-gui/app/beatbench.exe \
  --open <bms文件>           # 启动即打开谱面
  --screenshot <png>         # 截图后退出
  --page 0|1|2               # 切换到指定页面（0=编辑 1=切音 2=测试）
  --tool pan|select|note|ln|mine  # 设置编辑工具（pan 为默认）
  --click <x> <y>            # 模拟点击
  --probe <x> <y>            # 诊断探针
```

## 生态参照

- [BmsTWO](https://github.com/Roganis/BmsTWO) - Qt6/GPL-3.0，头号参照
- [imbms](https://github.com/dfroji/imbms) - C++/Linux 向
- [beatoraja](https://github.com/exch-bms2/beatoraja) - 播放器事实标准
- [raindrop](https://github.com/zardoru/raindrop) - 外部预览集成候选
