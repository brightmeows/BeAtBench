# Changelog

本文件记录 BeAtBench 对各发布版本的用户可见变更。版本号遵循语义化版本（major.minor.patch）；仓库规范见 `doc/04`。

## [0.5.0](https://github.com/brightmeows/BeAtBench/compare/v0.4.0...v0.5.0) (2026-10-15)


### Features

* **audio:** M4 试听/设置页/解码缓存/离线渲染（M4.1-M4.3b） ([d1bdfdb](https://github.com/brightmeows/BeAtBench/commit/d1bdfdb39e3f468c3e6c5195d86df178625f283f))
* **audio:** M4.3c 多线程后台渲染 + 波形显示 + 秒标尺 + 增量重渲染 ([33345e0](https://github.com/brightmeows/BeAtBench/commit/33345e02efe6234a33f553bcccc67392cadd1b99))
* BGM 多行展开——bgm_line + 按行序分列 ([5580fba](https://github.com/brightmeows/BeAtBench/commit/5580fbad8a3d9bda0b984b8fc83fdf64a55cc493))
* BGM 背景轨结构化（ch01 → LaneKind::Bgm）+ 采样面板视觉精简 + lint 自动换行 ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* BPM/STOP hover 状态栏显示 ([4537d5e](https://github.com/brightmeows/BeAtBench/commit/4537d5e99a50d97d6afbf04856b8062a429e6475))
* **clipboard:** 编辑页 Ctrl+V 支持系统剪贴板 BMS 原始行整段粘贴 ([b1addea](https://github.com/brightmeows/BeAtBench/commit/b1addeaae41352b4bef3fa26e056c25114c566f7))
* **codec:** CodecRegistry 重构 + 通道映射/模式表模块化（M3） ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* core 支持 #BASE 62 大小写敏感 id（base62 62x62=3844）+ info 采样使用统计 ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* **core:** BGM 多行展开——Note.bgm_line + parser 行号 + writer 按行写回（M3 扩权） ([5580fba](https://github.com/brightmeows/BeAtBench/commit/5580fbad8a3d9bda0b984b8fc83fdf64a55cc493))
* **core:** BGM 子轨 FIFO 重排 + LN 多选成对移动 + 保存空行（iBMSC 式） ([5a00bd5](https://github.com/brightmeows/BeAtBench/commit/5a00bd54a37657a7a991adc1b79cddfe0baf0424))
* **core:** M6.1 midi 解析器 + midi.parse 命令（10 项单测） ([0f6b849](https://github.com/brightmeows/BeAtBench/commit/0f6b849312f69a5b1a6853aa59279035e69c95f5))
* **debug:** 新增 --probe 诊断探针（noteAt/laneAtX/hitTest + 列布局） ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* **edit:** GUI 编辑接线 + timing 命令（BPM/STOP/节拍） ([a42919d](https://github.com/brightmeows/BeAtBench/commit/a42919d21eb3c6be65c72b20342fa3ab3cc59dc3))
* **edit:** GUI 编辑接线——放置/框选复制/撤销重做/保存（M3 协议命令） ([a42919d](https://github.com/brightmeows/BeAtBench/commit/a42919d21eb3c6be65c72b20342fa3ab3cc59dc3))
* **edit:** LN/地雷放置——note.put 加 kind 语义（M3） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **edit:** M3 编辑内核补全——move/变换量化/元数据/LN 放置/区域平移 + 前端对齐 ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **edit:** note.move 批量（moves 数组）· note.moveRegion 区域平移（M3 扩权） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **edit:** note.move 跨通道（to.lane）+ 批量移动支持（M3） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **editor:** 02 通道小节长度 + 加一小节 + 新建谱面 ([83bfa46](https://github.com/brightmeows/BeAtBench/commit/83bfa46eecd8fcd4c3198da49ee40cfdd944ce37))
* **editor:** M5 收尾 + 播放轨连续平移 ([28035b9](https://github.com/brightmeows/BeAtBench/commit/28035b9a4170e9651955d66161665168cfc318a4))
* **editor:** note 放置/移动 ghost + 踏板 + 背景tile对齐 + 多选预览 ([191b14f](https://github.com/brightmeows/BeAtBench/commit/191b14ff61dd3234a5fc5c0be48c4c2d5b9f635b))
* **edit:** select newly pasted notes ([df02d78](https://github.com/brightmeows/BeAtBench/commit/df02d788386fce10a80bb27c7094992ed7cbff61))
* **edit:** session.save——会话文档写回 + 修 note.put/delete lane 解析（M3） ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* **edit:** SessionRegistry 多文档会话——多标签页前瞻（M3） ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* **edit:** timing 命令——BPM/STOP/节拍编辑（M3） ([a42919d](https://github.com/brightmeows/BeAtBench/commit/a42919d21eb3c6be65c72b20342fa3ab3cc59dc3))
* **edit:** 元数据编辑 + lint 扩充（M3） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **edit:** 前端批修——2P 列序/BGM 可放/选取删除/控件风格统一/文件名标签性能 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **edit:** 剪贴板（BMS 原始行）+ 选择集 + 批量命令（M3） ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* **edit:** 单 note 移动 LN 重连 + 拖动移动交互 ([40a3642](https://github.com/brightmeows/BeAtBench/commit/40a3642d2883d67722899a285dec852d426cc627))
* **edit:** 单 note 移动 LN 重连【阶段2 LN】——移动端找最近连，原端原位断开（M3 扩权） ([40a3642](https://github.com/brightmeows/BeAtBench/commit/40a3642d2883d67722899a285dec852d426cc627))
* **edit:** 单点↔LN 转换（ToggleLnCommand + note.toggleLn） ([acb26f1](https://github.com/brightmeows/BeAtBench/commit/acb26f1b91b6ddc0e51eec8e5348ee2be35dcd9c))
* **edit:** 变换/量化 + 自动保存（M3） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **edit:** 平移模式（checkbox 开关）——拖拽选中 note 沿时间轴移动 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **edit:** 手势分发抽函数 + --click 模拟点击验收通道（修复 QML 类型错误） ([a42919d](https://github.com/brightmeows/BeAtBench/commit/a42919d21eb3c6be65c72b20342fa3ab3cc59dc3))
* **edit:** 编辑内核——命令体系/剪贴板/会话保存 ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* **edit:** 编辑命令体系——note.put/move/delete + undo/redo（M3） ([41a2df3](https://github.com/brightmeows/BeAtBench/commit/41a2df3ce3ff3baa6f75376c0d0911e152f0de5a))
* **edit:** 跨命名空间移动 + BGM 子轨互移（note→BGA/BPM/STOP、bgm_line 消歧） ([3dbd361](https://github.com/brightmeows/BeAtBench/commit/3dbd3612cf57cb7c533562b6f0788dbdc49cbb6e))
* **gui:** BGM 展开预留空虚拟子通道（PAD=8，只增不减） ([46ad50d](https://github.com/brightmeows/BeAtBench/commit/46ad50d11a779c0abf0fb4b120c90a8a214314c1))
* M0 skeleton and design docs ([a808b36](https://github.com/brightmeows/BeAtBench/commit/a808b360a4a620cff24e42ae522668832a03fbe2))
* M0 项目骨架与设计文档 ([a808b36](https://github.com/brightmeows/BeAtBench/commit/a808b360a4a620cff24e42ae522668832a03fbe2))
* M1 —— BMS codec 读写 + note 事件化（通道映射/LN 配对/重建写出） ([61738dc](https://github.com/brightmeows/BeAtBench/commit/61738dc391befc5520c9b9e8ab24319dbf91ef73))
* M1 —— 选择块展开 + TimingEngine + CLI convert/lint（含文档地图） ([0bfa36b](https://github.com/brightmeows/BeAtBench/commit/0bfa36b63582562ce3f5a7d2a9635907c77d12a1))
* M1 BMS codec 第一步（文件读写 + 头部字段 + CLI info/check） ([61738dc](https://github.com/brightmeows/BeAtBench/commit/61738dc391befc5520c9b9e8ab24319dbf91ef73))
* M1 note 事件化（通道映射表 + LN 配对 + 事件重建写出） ([61738dc](https://github.com/brightmeows/BeAtBench/commit/61738dc391befc5520c9b9e8ab24319dbf91ef73))
* M1 收尾——命令框架最小 JSON 协议（--json + 注册表 + 内建五命令） ([0a5f7c5](https://github.com/brightmeows/BeAtBench/commit/0a5f7c57d76c64be8682eff1cc0249e5c6c2f5c8))
* M1 收尾——命令框架最小 JSON 协议（--json 入口 + 命令注册表 + 内建五命令） ([0a5f7c5](https://github.com/brightmeows/BeAtBench/commit/0a5f7c57d76c64be8682eff1cc0249e5c6c2f5c8))
* M1 选择块展开 + TimingEngine + CLI convert/lint ([0bfa36b](https://github.com/brightmeows/BeAtBench/commit/0bfa36b63582562ce3f5a7d2a9635907c77d12a1))
* M2 主题/字体基线（+解析器变量清理） ([2c8ed1d](https://github.com/brightmeows/BeAtBench/commit/2c8ed1da57238cf3ff6d0488aee7dd731c4b8714))
* M2 主题基线——ThemeManager token + Fusion 深色 + 控件样式基线 + 页面真正切页 ([2c8ed1d](https://github.com/brightmeows/BeAtBench/commit/2c8ed1da57238cf3ff6d0488aee7dd731c4b8714))
* M2 第 4 步——采样面板（搜索确认框）+ lint 面板（缺失/诊断 + 双向往返） ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* M2 第 5 步——时间轴视图（ChartSession + ChartViewItem 自绘） ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* M2 起步——QML 页面式外壳 + core↔QML 桥接 ([8bd0977](https://github.com/brightmeows/BeAtBench/commit/8bd09779cdf6ca831be2ae647d3c8618d3e6eb59))
* M2 起步——QML 页面式外壳 + core↔QML 桥接 + 第一条真链路 ([8bd0977](https://github.com/brightmeows/BeAtBench/commit/8bd09779cdf6ca831be2ae647d3c8618d3e6eb59))
* **M5.1:** 编辑期间随时播放——Space 播放/暂停、载入自动渲染内存化、PcmEngine 零拷贝播放、播放时钟、编辑即停 ([82f235a](https://github.com/brightmeows/BeAtBench/commit/82f235a5ccc5c64d4c04066e35db505dd8941f18))
* **M5.2:** 播放头红线 + 视口跟随 + A-B 循环 + 循环点红线打点 ([6713922](https://github.com/brightmeows/BeAtBench/commit/6713922cdc0cbac33f7a5d28a7c66efe8d144c2f))
* **M5.2:** 红线视口光标语义 + 开头留白 + A/B 标记扣留白 ([5a994af](https://github.com/brightmeows/BeAtBench/commit/5a994af65ea62ee560db105f8e683a070387e451))
* **meta:** 元信息保存按钮+排序+下拉注释; feat(sample): 双击编辑采样 id ([e16959b](https://github.com/brightmeows/BeAtBench/commit/e16959b879814f977fe868ff152b10445107fe35))
* **pkg:** Linux deb/rpm/arch 与 Windows 发布打包流水线 ([b914aae](https://github.com/brightmeows/BeAtBench/commit/b914aaeb26ecd396401b404ace674fe5b8970aff))
* **pkg:** Linux 三格式打包脚本与安装冒烟 ([9829900](https://github.com/brightmeows/BeAtBench/commit/98299006e310334acf06b23a2f809989e106a526))
* **pkg:** 发行版安装布局、皮肤数据目录兜底与桌面素材 ([b2a707f](https://github.com/brightmeows/BeAtBench/commit/b2a707f2884feb12b78d6d8313a86b41e71dd436))
* **pms/9key:** .pms 恒 pms9k + 宽松 9key 表（16-19 与 22-25 均→键6-9）+ 列头按模式映射 ([38d4cee](https://github.com/brightmeows/BeAtBench/commit/38d4ceecd167283df6eacbb07a43acfba1b15f14))
* **qml/cmd:** 右 Dock 时间轴新增 #BPM/#STOP 定义表区（分离 id 定义与时间轴使用） ([c2a3a23](https://github.com/brightmeows/BeAtBench/commit/c2a3a23346bbec322d4f6e2818f64c1b96dae3a8))
* **qml:** LN 工具提示显示当前 LNTYPE/LNOBJ 解析值（诊断「仍走 LNTYPE1」） ([e1af5ba](https://github.com/brightmeows/BeAtBench/commit/e1af5bafb5a706ece73e13247b77d2fbc391d089))
* **release:** tag 触发发布流水线 + 本地 release.sh（复用同一打包脚本） ([9794ac6](https://github.com/brightmeows/BeAtBench/commit/9794ac6e03d34e889a573dab28cdf4f5f7f756a1))
* **right-dock:** 属性检查器 + 时间轴面板 ([5d715e2](https://github.com/brightmeows/BeAtBench/commit/5d715e26d324f686650257ca0bef9cefc13c7d37))
* **slice:** add export base and standalone mode ([a6b2c9b](https://github.com/brightmeows/BeAtBench/commit/a6b2c9bff12c20388594517b01d8205326935cdf))
* **slice:** M6.1 导入工作台（参考音频/MIDI/offset + 播控分槽） ([f4b43d8](https://github.com/brightmeows/BeAtBench/commit/f4b43d8437503dd03d71987db994a7253aed5b13))
* **slice:** M6.2 切分（网格/MIDI 位置源 + 切片线 + 切片列表） ([9206ab0](https://github.com/brightmeows/BeAtBench/commit/9206ab039f2fd17b1a1d377da2966e1264e4d45c))
* **slice:** M6.3 分片导出布局 + 音频 fade 原语（headless 可测） ([6c3d290](https://github.com/brightmeows/BeAtBench/commit/6c3d2902881cfbaf15b17bcc8e0f2640b8bb2184))
* **slice:** M6.3 导出 UI 细化——导出起始ID（36进制文本显示）+ 导出前缀 ([fba522a](https://github.com/brightmeows/BeAtBench/commit/fba522a697ea5b917b133684f73e94f3de134778))
* **slice:** M6.3 导出分片 + #WAV id 分配 + 可复制 raw（GUI） ([c982535](https://github.com/brightmeows/BeAtBench/commit/c982535bc5072bddfdf8617711b991adf67b299d))
* **slice:** M6.4a 换行视口——波形按每行时长换行 + 整行滚动/缩放 ([ecc245d](https://github.com/brightmeows/BeAtBench/commit/ecc245d695ebb5139847f3cd0a7dc85345cf4db3))
* **slice:** M6.4c 手动切分点——双击切换/右键删除 + 2拍缩放档 ([d1091ee](https://github.com/brightmeows/BeAtBench/commit/d1091ee7a84c3e46e11075bf3de2c4d746f9148f))
* **slice:** M6.4e 纯手动切片模式——无需先生成切片 ([f5695b7](https://github.com/brightmeows/BeAtBench/commit/f5695b77244e06271edb37ddb192c0fde49ff275))
* **slice:** MIDI 切片右边界改为下一起始/音频末尾（与手动切片一致） ([757a336](https://github.com/brightmeows/BeAtBench/commit/757a33692c13cbf5f22e7e3aa651b36ef92809e3))
* **slice:** support selectable wav id bases ([88b771f](https://github.com/brightmeows/BeAtBench/commit/88b771f03b505b2e80ad225d10196ff5abaea416))
* **slice:** workspace undo for manual points and page routing ([e0f0cb2](https://github.com/brightmeows/BeAtBench/commit/e0f0cb2f143dad5c7a2c19e3f8f43f00af9db66b))
* **slice:** 切片编辑对话框（双击行）+ 删除网格页（默认页=切片） ([1c52476](https://github.com/brightmeows/BeAtBench/commit/1c524763955a5eaad41a574b805544d6e526070e))
* **slice:** 切音页 UI 改良（M6.3c）——三栏 dock + 起始小节 + MIDI 线 Ctrl 开关 ([6f8c253](https://github.com/brightmeows/BeAtBench/commit/6f8c25357bc195af893f121223f6d392afdde1bf))
* **slice:** 切音页键盘（woslicer 系）+ 焦点区域高亮（PR 式） ([ca8cd6b](https://github.com/brightmeows/BeAtBench/commit/ca8cd6beb31a60bc1645872ffcd91b782cd961c5))
* **slice:** 导出「同时铺入编辑区」开关——ch01 子行接续（不挤旧行、跨小节同列） ([743f639](https://github.com/brightmeows/BeAtBench/commit/743f639700a87c99f19df6bab6e9e76b7c6367ea))
* **slice:** 导出结果附铺放起点信息（第几小节起/至） ([ea1c993](https://github.com/brightmeows/BeAtBench/commit/ea1c99328449f5abe99162d0f1dccc070149fe87))
* **slice:** 瞬态检测 audio 源（OnsetDetector + 桥层落手动点） ([7806d0c](https://github.com/brightmeows/BeAtBench/commit/7806d0cba8957161ea5071fbb668830ceefcb246))
* **slice:** 瞬态检测切片源（M6.5，按音频变化自动选切音点） ([096f0dd](https://github.com/brightmeows/BeAtBench/commit/096f0dd618442636c68c822f5e78b497e6882685))
* **slice:** 瞬态检测源接入切音页 + 文档同步 ([7d5897a](https://github.com/brightmeows/BeAtBench/commit/7d5897a9c75b98d5a3461cd22f7bbf2a3a7a5a83))
* **slicing:** 采样槽位全枚举+文件名编辑; note 双击改引用采样 id; 下拉去省略随dock宽 ([018e87f](https://github.com/brightmeows/BeAtBench/commit/018e87fbaf3e98b0a1b14d9b6476ee18e413496b))
* **timing:** BPM/STOP 值放置（路线A）+ 独立选中（方案2） ([ea1d424](https://github.com/brightmeows/BeAtBench/commit/ea1d424bc896dec7e957cf6863ee003732d8a6e6))
* UI 动作注册表 + 工具条迁移 + 网格线增强 ([a7485f1](https://github.com/brightmeows/BeAtBench/commit/a7485f122a9bf7129801fa00685bc1e05d4de591))
* UI 动作注册表（doc/09）+ 网格线可见性增强 ([792cfdc](https://github.com/brightmeows/BeAtBench/commit/792cfdce4ee1cda5f25fd4b6926557ad73b83963))
* UI 动作注册表完成 + 网格线增强 + 菜单修复 ([78e770c](https://github.com/brightmeows/BeAtBench/commit/78e770cd9e4c7db91d5783a0c58fe9231f98eea9))
* **ui:** --drag 调试参数 + 文件对话框 .bml/.bme + 同步 ([e75667e](https://github.com/brightmeows/BeAtBench/commit/e75667e37b976837399c6813cad2814f0ed6b619))
* **ui:** BGM 展开按行序分列（bgm1/bgm2/…，含空行占位） ([5580fba](https://github.com/brightmeows/BeAtBench/commit/5580fbad8a3d9bda0b984b8fc83fdf64a55cc493))
* **ui:** checked 状态与注册表打通（doc/09 §12） ([d224c69](https://github.com/brightmeows/BeAtBench/commit/d224c691e852996b96d53ba4ed5b6e3e5ccd4cc1))
* **ui:** keymap.json 快捷键覆写（皮肤可携带） ([4b4fe59](https://github.com/brightmeows/BeAtBench/commit/4b4fe59b559d421df162a7eebadaf7a26c9bf45e))
* **ui:** L1 控件形状细分（buttonRadius/boxRadius）+ 亮色内置皮肤 OsuLight ([37fed98](https://github.com/brightmeows/BeAtBench/commit/37fed98e425699514af1f1a161c74abc76b937d2))
* **ui:** L1 皮肤样例（--skin 加载 theme.json 颜色覆写）+ skins/Aurora ([0bca709](https://github.com/brightmeows/BeAtBench/commit/0bca709f30968949ba47cf42ff3c5923404a189d))
* **ui:** L1 运行时换肤——Theme.token 改 NOTIFY + 皮肤菜单运行时切换 ([8624a30](https://github.com/brightmeows/BeAtBench/commit/8624a30cacfa43183718a7bc4fc485e1a361417e))
* **ui:** lint 删除刷新 + snap ×2/÷2 + 缩放 500%/光标锚点 + note 固定尺寸/颜色收敛 ([0072ca8](https://github.com/brightmeows/BeAtBench/commit/0072ca87894b9fb5a187ed6235937e4bcbbfb1e6))
* **ui:** note 高度缩放浮动区间 + 缩放无漂移等比级别 + note id 标签描边 + snap 刷新 ([e8bef68](https://github.com/brightmeows/BeAtBench/commit/e8bef6883ead49bdcd8d8703f3132d2eca22e633))
* **ui:** page-scoped shortcut registry for playback, render, settings, cancel ([0d04a07](https://github.com/brightmeows/BeAtBench/commit/0d04a07151638df90ce4399b2aa57eb5b5003510))
* **ui:** runtime keymap rebind and shortcut settings page ([9afa621](https://github.com/brightmeows/BeAtBench/commit/9afa6218d61c2e7105af16c79405ced9b898d722))
* **ui:** show and reveal shortcut settings location ([9f69e20](https://github.com/brightmeows/BeAtBench/commit/9f69e202178d1f91dd2d69d7ab8a09ab4cdf7e61))
* **ui:** slice page status bar summary ([b2f9c97](https://github.com/brightmeows/BeAtBench/commit/b2f9c97da68446466092bbbf448436fb955cf6b0))
* **ui:** slice workflow shortcuts and shared -/= zoom ([4c0b81c](https://github.com/brightmeows/BeAtBench/commit/4c0b81c192778a4f22762887b5322b9759e26389))
* **ui:** UI 动作注册表最小可信版——invoke 真实化 + setEnabled + 单测 ([38663e7](https://github.com/brightmeows/BeAtBench/commit/38663e7476fef160f078994365af0a2df001e5ef))
* **ui:** 元信息可编辑表单 + 量化/变换/网格按钮 + 视口头 snap 动态 ([a066cf2](https://github.com/brightmeows/BeAtBench/commit/a066cf296a426c181bff3c96bdac6d59580bd160))
* **ui:** 前端对齐 note.moveRegion·LN/地雷放置·snap 分子分母·采样框修复（前端批修） ([16d9826](https://github.com/brightmeows/BeAtBench/commit/16d9826be9d72da466ad0b678a85dee480e97b6c))
* **ui:** 前端批修 1-3 + 平移模式 + 控件修复 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **ui:** 多选跨通道移动同距离 + 元信息下拉/扩展代码 + 保存收敛到文件保存 ([cd10f71](https://github.com/brightmeows/BeAtBench/commit/cd10f7133998f89058f37cdb697e932fdefb72cb))
* **ui:** 工具条动作注册化——注册表新增 toolbar/control/value/prefix/tooltip 元数据 ([39bc1ee](https://github.com/brightmeows/BeAtBench/commit/39bc1eea66f47d509df797034f883f67b4513409))
* **ui:** 拖动 note=移动（编辑工具）+ 内容变化重建列【阶段1 交互】 ([40a3642](https://github.com/brightmeows/BeAtBench/commit/40a3642d2883d67722899a285dec852d426cc627))
* **ui:** 拖动分发跨命名空间转换 + 默认关闭平移限制 ([6de7d2c](https://github.com/brightmeows/BeAtBench/commit/6de7d2cd7fe17e05a8144c2e036e81e30876d4b5))
* **ui:** 拖拽入窗口 / 拖到 exe 图标 / 双击关联打开文件 ([25546cd](https://github.com/brightmeows/BeAtBench/commit/25546cd93ffb40ff4f099268ed72c4dce4fe8c5c))
* **ui:** 状态栏格式/编码 + hover 实际值 + LN 尾高亮 + 同轨拖动保护 ([9382f1f](https://github.com/brightmeows/BeAtBench/commit/9382f1fd994afd1a5a7eab822f3c718d14659a53))
* **ui:** 皮肤 L2——theme.json 非颜色 token 覆写（密度/圆角/字体）+ 样例皮肤 ([78ea526](https://github.com/brightmeows/BeAtBench/commit/78ea526d622ff80bdea798816712d11fc1a9b539))
* **ui:** 皮肤菜单勾选修复 + spinbox 箭头对比度 + Win10 直角皮肤 ([d71d1e0](https://github.com/brightmeows/BeAtBench/commit/d71d1e052e2c35e1dc3caf5d72b0d7f55f586d76))
* **ui:** 视口标尺/note 字体 token 化 + 键轨每轨轻着色 token（keyLaneTintAlpha） ([ef1b44d](https://github.com/brightmeows/BeAtBench/commit/ef1b44dc78bfc86063c63caeb5641854385767ba))
* **ui:** 视口跨命名空间列语义 + LN 尾 hover 显示实际通道 ([d578506](https://github.com/brightmeows/BeAtBench/commit/d57850696384bab4ed26bc839723df981aa88bdf))
* **ui:** 移除 BGA 图层列移动/放置限制（问题1 前半） ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* **ui:** 编辑后刷新 lint 面板（内存 lint） ([d2ea347](https://github.com/brightmeows/BeAtBench/commit/d2ea347847d41ec388c65f3f6012a05e89912fa4))
* **ui:** 菜单枚举化渲染（doc/09 §7 验收 2）——文件/编辑菜单按注册表生成 + 分隔线建模 ([d1b168d](https://github.com/brightmeows/BeAtBench/commit/d1b168d6aace4e01dda45c5c5861336bb5a1ac21))
* **ui:** 运行时换肤同步应用皮肤 keymap（doc/10 §4 待改进 A） ([253a8e0](https://github.com/brightmeows/BeAtBench/commit/253a8e0089a68e5f55920433ffb8663a93c51b48))
* 元信息面板增强（BASE 字段 + 下拉特殊选项） ([f743c76](https://github.com/brightmeows/BeAtBench/commit/f743c763882d932656acc94193554bf9e9e7f102))
* 元信息面板添加 BASE 字段 ([c762628](https://github.com/brightmeows/BeAtBench/commit/c76262840943347a06374e3b3243f0e6e63f884f))
* 时间轴——BGM 轨点击展开分列 + 列头通道 id 显示（Alt 临时） ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* 时间轴交互完善——note 底对齐/拍子线/采样标签/悬停信息/缩放锚点 ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* 时间轴交互补全——Ctrl 键监控/note 内采样标签/瞬态状态/水平滚动/更多轨道 ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* 时间轴可视化补全——BPM/STOP 元事件轨/BGA 四列命名/描边标签/双滚动条/缩放锚点修复 ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* 时间轴视图——自绘/交互/可视化补全 ([1a0d83b](https://github.com/brightmeows/BeAtBench/commit/1a0d83be9185388f15560fb00a449ad67e4ef388))
* 界面字体/密度基线——黑体 + 字号 token + SplitView 可拖拽三栏 + 系统控件本地化 ([2c8ed1d](https://github.com/brightmeows/BeAtBench/commit/2c8ed1da57238cf3ff6d0488aee7dd731c4b8714))
* 采样/lint 面板体系 + base62 + BGM 背景轨 + 面板批修 ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* 采样面板排序/分组调整（引用/最近/键→皿→背景）+ 测试耗时分档（BB_SKIP_REAL） ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* 采样面板检索增强——显式排序 + 动态分组 + 索引条；lint 扩展名回退（wav→ogg 生态） ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* 采样面板添加 #WAV + BPM/STOP 编辑 id ([882f659](https://github.com/brightmeows/BeAtBench/commit/882f659d2002c4dc22455905e58be088d3cc8768))


### Bug Fixes

* 2026-09 代码审查修复批 + .pms 往返链路核验修正 ([367ff39](https://github.com/brightmeows/BeAtBench/commit/367ff39eb014414debac0633bf15b78d9a5067ca))
* **app:** lambda 移除对 static 变量的按引用捕获 ([12ce279](https://github.com/brightmeows/BeAtBench/commit/12ce279fb90ec06998492eedce76237321d648f5))
* **app:** QML 入口加载兼容 Qt 6.4（无 loadFromModule） ([b38deeb](https://github.com/brightmeows/BeAtBench/commit/b38deeb8d3576f7c1468750003104b1bdc62a322))
* **audio:** std::min 显式钉 uint64_t，修 Apple 平台类型冲突 ([460e3fb](https://github.com/brightmeows/BeAtBench/commit/460e3fb5e2fe63f0214fa924d8f54094915df129))
* BASE 字段移到折叠页 + 下拉空白选项 ([990d928](https://github.com/brightmeows/BeAtBench/commit/990d92873fdd5a257c7cceee7607c2824488424f))
* Base36/Base62 模式提示 + info 返回 id_base ([b2158c1](https://github.com/brightmeows/BeAtBench/commit/b2158c1cd18c56cdf51b15458e917ba91ff1e6e2))
* **bms:** ch08 BPM 通道保真 + LN 禁止放 BGM 轨（mapping 驱动） ([1cb4afd](https://github.com/brightmeows/BeAtBench/commit/1cb4afddb0f1a098343a48bd7535567c5c31cfd3))
* **bms:** 修正 STOP 语义为「当前 BPM 全音符计数」+ BGA/BMP 编辑与采样删除 ([0988afe](https://github.com/brightmeows/BeAtBench/commit/0988afe8245ba15376476f678fce127c49ce0540))
* **bms:** 对齐通道映射定义并降级 wav_ext_mismatch 为信息级 ([4330189](https://github.com/brightmeows/BeAtBench/commit/4330189b84f161caacafe3c48099e44d76e3425b))
* **bms:** 通道映射对齐 + 小节口径修复 ([4330189](https://github.com/brightmeows/BeAtBench/commit/4330189b84f161caacafe3c48099e44d76e3425b))
* BPM/STOP hover + #BASE 62 保存修复 ([47510fe](https://github.com/brightmeows/BeAtBench/commit/47510fe005b521461402d7f39d8eff781a1fdba7))
* BPM/STOP 编辑 id 逻辑 + base62 测试 ([cf99e4b](https://github.com/brightmeows/BeAtBench/commit/cf99e4bbf9f945e01a6cc258173b2699b5719a3d))
* **build:** Qt 6.12 上跳过 QML 字节码预编译（qmlcachegen 在本模块 SIGSEGV） ([bc68b4d](https://github.com/brightmeows/BeAtBench/commit/bc68b4d21d46c6c9b0bffcee78571accc801cb40))
* **build:** 禁用 Qt 版本哨兵，发行版产物不再绑定 Qt 小版本 ([33b68bd](https://github.com/brightmeows/BeAtBench/commit/33b68bdedd26d6ea8734f37a09f7d39cf0b4276a))
* **build:** 禁用 Qt 版本哨兵，发行版包不再绑定 Qt 小版本（v0.3.2 arch 包无法启动的根因） ([cc093f2](https://github.com/brightmeows/BeAtBench/commit/cc093f23587eb2cfba1fea651833b4d662d0d53b))
* **build:** 钳制 CMAKE_POLICY_VERSION_MINIMUM 适配 CMake 4 ([0c26232](https://github.com/brightmeows/BeAtBench/commit/0c26232839cca088a76c14788751bb92debf27c4))
* ch02 小节长度改四分拍口径（parser ×4 / writer ÷4，往返无损） ([4330189](https://github.com/brightmeows/BeAtBench/commit/4330189b84f161caacafe3c48099e44d76e3425b))
* **ci:** ccache 统计步骤兼容 macOS bash 3.2 的多字节变量解析 ([83d312c](https://github.com/brightmeows/BeAtBench/commit/83d312c33f82dfebcc2bebc6fdc1e53debef70e3))
* **ci:** macos Build 显式 --parallel 3，杜绝 make -j 无限并发 ([0ff2d36](https://github.com/brightmeows/BeAtBench/commit/0ff2d36b6e6cbb40931e2aca746f7d1d4229bef2))
* **ci:** macos ccache 路径对齐 ccache 默认值，修正注释归因 ([90d06ba](https://github.com/brightmeows/BeAtBench/commit/90d06bac6fc8a291cb2f7bd16e906fa759c7dec2))
* **core/qml:** 共享 id 语义 — 改 ref 事件值=改定义值（引用者全同步）+ 空音槽位可放置 ([66b71bd](https://github.com/brightmeows/BeAtBench/commit/66b71bd0c484f7cece4a748be96bf7d1b99cbab6))
* **core:** Windows 日文/非 ASCII 路径打开 + BPM/STOP 保留 ref_id ([cb6f9f2](https://github.com/brightmeows/BeAtBench/commit/cb6f9f21951ace6d39b128877eca86f8a875d8ab))
* **core:** 清理编译警告——删除未用 lane_to_json、unused 指针声明、测试临时文件唯一名+删除容错 ([aa1e8e3](https://github.com/brightmeows/BeAtBench/commit/aa1e8e35337b106fbcada28cf4e5fca411094d08))
* **edit:** LN 异类 note 配对恢复「找最近未配对头」忽略中间其它通道（问题2，M3 扩权） ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* **edit:** LN 移动换轨也保持 LN（重连不再因 to_lane 断开）——LN 尾拖到游玩轨变普通 note 修复 ([9264d13](https://github.com/brightmeows/BeAtBench/commit/9264d13518a318cb902b846483e3eca08b5e22b6))
* **edit:** LN 自动配对改「紧邻前一个 note」才配（问题4，M3 扩权） ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* **edit:** LN 通道标记随移动更新 + ToggleLn 只切本 note + session.lint ([189bb07](https://github.com/brightmeows/BeAtBench/commit/189bb07b465c02b649ea988476ab48720c96d7cb))
* **editor:** 0.2.0 收尾修复（波形不消失/新建初始化/文本/皮肤/ghost） ([08b446d](https://github.com/brightmeows/BeAtBench/commit/08b446d403c358a6eb2d783207feb893d212322f))
* **editor:** BGM 多选跨通道移动保持相对距离（多轮根因修复） ([77ac328](https://github.com/brightmeows/BeAtBench/commit/77ac328176317720bc10fe67bf558d9a287f44bf))
* **editor:** pan 拖拽工具下 BGA/BPM/STOP 对象只点选、不移动 ([98546ee](https://github.com/brightmeows/BeAtBench/commit/98546ee38a2d880a66c6a5c1fc78987b6c1a7d78))
* **edit:** preserve clipboard note kinds and batch-delete undo ([38445d6](https://github.com/brightmeows/BeAtBench/commit/38445d638d77d1b4b9e9a834e2c15d4699c77660))
* **edit:** 前端反馈批修2——放置后视口跳001/snap拍子线合并/选取交互/弹层黑字 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **edit:** 前端反馈批修3——单点选中/Ctrl多选/拖拽工具/Del修复/snap手填 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **edit:** 移动只移动选中 note、ln_pair 保持（不自动重连/不成对随动） ([1108465](https://github.com/brightmeows/BeAtBench/commit/110846523916b06b1c7f9c67708a5b3e91783737))
* **gui:** 2026-09 GUI 人工测试修复批 + .pms 标准映射 + ch03 hex BPM ([936950c](https://github.com/brightmeows/BeAtBench/commit/936950c29845373dd5199d36cd1c2d0536d4eb61))
* **gui:** 底部水平滚动条不出现——rebuildColumns 未发 contentWidthChanged ([d13006f](https://github.com/brightmeows/BeAtBench/commit/d13006face589cd6d9848e7c364d3ce71afe22df))
* **gui:** 游玩轨无 tint 列填黑——QColor() 默认 alpha 255 误判为不透明 ([1f27bdc](https://github.com/brightmeows/BeAtBench/commit/1f27bdc844e0b426d78bd9a9781d016056c32d52))
* **gui:** 采样改名后 lint 灯消失 + iBMSC 分割线上下空行 ([a20c635](https://github.com/brightmeows/BeAtBench/commit/a20c635fdedbdaf38ae5111ca7cda914205a2ecb))
* **io:** atomically replace saved charts and exported wavs ([d9aac5d](https://github.com/brightmeows/BeAtBench/commit/d9aac5d0d2c476cd4d4e6a958ffaa5f9cbfe09f0))
* **lint/ui:** LN 显示按数据真值 + lint 分级/BGM 豁免/ext 聚合 ([20f1a86](https://github.com/brightmeows/BeAtBench/commit/20f1a86b887a67b2c2badfbec1063974bbbc18aa))
* **lint:** 打开谱面即在 lint 面板显示「重叠 note / 悬挂 LN」 ([8768137](https://github.com/brightmeows/BeAtBench/commit/8768137b56674696c17ce3d2d064a8ba1aea8a15))
* **meta/stop:** #RANK 区间 0-4 + 难度注释；STOP 改按普通 note 呈现（去掉时长段预览） ([5e23bcf](https://github.com/brightmeows/BeAtBench/commit/5e23bcfc852c34440f31da56fcf5b28daee3a6b9))
* **meta:** 下拉改非可编辑+箭头清楚可点; 左dock恢复240; 扩展框字体白色 ([10fcd57](https://github.com/brightmeows/BeAtBench/commit/10fcd5723e1240e556db2ea0894e14b5c09df646))
* **meta:** 下拉文字省略 + 移除最小宽度，随 dock 宽度自适应 ([502997e](https://github.com/brightmeows/BeAtBench/commit/502997e814902390f5f2538146af69c75bea540d))
* **meta:** 下拉箭头放大 + 重置后下拉显示不更新 ([3fab9e9](https://github.com/brightmeows/BeAtBench/commit/3fab9e97327abf059272219c562006d9a9b04ac6))
* **meta:** 字段填充 dock 宽随宽度; fix(采样): 编辑文件名 Esc取消/失焦+Enter保存/保存后停在原行 ([5f234c4](https://github.com/brightmeows/BeAtBench/commit/5f234c49210863b324405a4c57c56a8216c8f8ed))
* **package-linux:** arch 包 depends 显式 qt6-base&gt;=6.4 下限 ([9dbeee5](https://github.com/brightmeows/BeAtBench/commit/9dbeee504b33e818b8b25229c74b1e6f328a3dfe))
* **pkg:** --no-quick3dutils 改为探测后再传（无 qtquick3d 的 Qt 上它不存在，v0.4.0 首发因此失败） ([cf167ac](https://github.com/brightmeows/BeAtBench/commit/cf167acb4e57f226938d4b460d8e374683c3dc90))
* **pkg:** --no-quick3dutils 改为探测后再传（无 qtquick3d 的 Qt 上它不存在，v0.4.0 首发因此失败） ([af5704c](https://github.com/brightmeows/BeAtBench/commit/af5704ce15466db5fe8911b3a12a5b54af3817fb))
* **pkg:** arch 包维护者字段改为 env PACKAGER —— makepkg 不读 PKGBUILD 变量 ([7c4a67b](https://github.com/brightmeows/BeAtBench/commit/7c4a67bae660430859475cc050b8843513cfab66))
* **pkg:** PR [#5](https://github.com/brightmeows/BeAtBench/issues/5) arch 验收 follow-up（packager 字段 / 实机部分升级 / 悬停高亮可自动化） ([cb3d6fb](https://github.com/brightmeows/BeAtBench/commit/cb3d6fbba280dbd20d18b66bf43847c5eaf66283))
* **pkg:** 打包脚本显式指定 C 编译器（PortAudio 是 C，混用工具链时报假缺头） ([9f9837a](https://github.com/brightmeows/BeAtBench/commit/9f9837aa1b1f5b75f3f36b7e437127556278a7a3))
* **playhead:** 红线/A-B 标记换算委托 ChartViewItem——变拍高感知（x2/x0.5 小节错位） ([43f4bb0](https://github.com/brightmeows/BeAtBench/commit/43f4bb09a5adee3d58c63deeef434c4e9742b31e))
* **qml/bms:** LNTYPE/LNOBJ 实时生效 + LNOBJ 空音尾兜底 + lint 未绑定引用警告 + 各 UI 收尾 ([c4d864e](https://github.com/brightmeows/BeAtBench/commit/c4d864e671f4cab2a2761ed1b9cce8a7f605e06d))
* **qml/bms:** 状态栏 VS Code 式右对齐排版 + LNTYPE 1/2 双机制并存 ([657ebc8](https://github.com/brightmeows/BeAtBench/commit/657ebc85d3bc26e75e08908e9c5dcc4d342d0277))
* **qml/cmd:** 时间轴「事件/定义」彻底分离 + 交叉同步 + 定义值解耦（方案 B） ([1851dcf](https://github.com/brightmeows/BeAtBench/commit/1851dcfa9b486af6a74e9a4b60fa650cc7c99057))
* **qml/core:** #BPM/#STOP 定义编辑真实生效（sample_kind_arg 放行 bpm/stop）+ 模式显示 + DIFFICULTY 1-5 ([64109d6](https://github.com/brightmeows/BeAtBench/commit/64109d6f21bf0e1bfa7db2a79c63bd9182a57a2c))
* **qml/io:** 属性面板支持 BGA/BPM/STOP meta 对象详情 + 内联编辑提交 & 状态栏排版 ([3f32781](https://github.com/brightmeows/BeAtBench/commit/3f3278197e40aeb0dd9cfdc0f713edaa35ebbf40))
* **qml:** EditPage handle 嵌套层的 SplitHandle 附加属性限定到 handle 对象 ([c61e4f9](https://github.com/brightmeows/BeAtBench/commit/c61e4f9926800211abddf89b7de78e90ad610efc))
* **qml:** SplitView handle 悬停改用 SplitHandle.hovered ([e1b6005](https://github.com/brightmeows/BeAtBench/commit/e1b6005d1e60db32b1b8711350d94120e4191456))
* **qml:** 分隔条悬停改用 HoverHandler，兼容 Qt 6.4 ([c3e1559](https://github.com/brightmeows/BeAtBench/commit/c3e1559aaa554693c0522e64e93edfad3270f8a1))
* **qml:** 右 dock 布局 — 隐式宽覆写可收缩（对话框/属性面板不再撑宽） ([27fabf6](https://github.com/brightmeows/BeAtBench/commit/27fabf6a41681649287076827dfbb8c733728136))
* **qml:** 时间轴事件对话框右对齐窗口右缘（不依赖 dock 宽）+ 值列限长 + DP→DP7K ([24469ac](https://github.com/brightmeows/BeAtBench/commit/24469ac67a7ff0b9eb08332a954fbcac0cdbd0c9))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([554b111](https://github.com/brightmeows/BeAtBench/commit/554b1113208f68ed4183e6499e09fcae5149e183))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([999654a](https://github.com/brightmeows/BeAtBench/commit/999654a86ef2b9107cedd09ac5c8ad767afbed79))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([444d61f](https://github.com/brightmeows/BeAtBench/commit/444d61f81e8f0fa11799ca06b465842ef2e4be61))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([f8ef4d1](https://github.com/brightmeows/BeAtBench/commit/f8ef4d1ce1bbdf2653c39c92e2da0ea9f16d508e))
* **release:** 修好 release-please 首次发版会踩的两处坑（README 包名被改坏 + 门禁漏放） ([7c9330a](https://github.com/brightmeows/BeAtBench/commit/7c9330a9495d31daf4c41cf5e60186ee9c89bc48))
* **release:** 修好 release-please 首次发版会踩的两处坑（README 包名被改坏 + 门禁漏放） ([fe9241e](https://github.com/brightmeows/BeAtBench/commit/fe9241ed62ae569035df50bf758e6e96cf1a765c))
* **release:** 回归步骤补 PATH/offscreen 并放宽 ctest 超时 + .gitignore 通配 build-* ([f880193](https://github.com/brightmeows/BeAtBench/commit/f8801934b3d7612c971d5310b39959bd88aeb22f))
* **right-dock:** 时间轴事件「×」被行MouseArea拦截 + undo/redo后重取时间轴列表 ([31da3c3](https://github.com/brightmeows/BeAtBench/commit/31da3c38911fcf97dfa660e55100b3dd255307b1))
* **right-dock:** 时间轴对话框居中于右dock + 一次Esc关闭 + BbTextField支持escapeHandler ([4979fd3](https://github.com/brightmeows/BeAtBench/commit/4979fd38f32ecc2df29cf31f9edd2973c1f253ef))
* **scripts:** --qt-version 支持逗号分隔多 minor（Qt 6.12 绕过说明不再被判过期） ([19b487a](https://github.com/brightmeows/BeAtBench/commit/19b487a492e3eb5551f86b597e142ea0cf1759d6))
* **scripts:** check-version 增加 Qt 下限口径（--qt-floor） ([04cb764](https://github.com/brightmeows/BeAtBench/commit/04cb76463dcc0e30e0b6c1bc57cb95ba75e5509c))
* **scripts:** check.sh 把 Qt bin 加进 PATH（Windows 下 Qt 桥层测试报 0xc0000135） ([cbcc9f5](https://github.com/brightmeows/BeAtBench/commit/cbcc9f579f064bb9c0ac030dd0321954941331a7))
* **scripts:** Qt 版本矩阵收进 check-version 默认值（否则 pre-push 会挡住发版 tag） ([d55c3f4](https://github.com/brightmeows/BeAtBench/commit/d55c3f428708b665b5232f748f0fcd2d6e410c23))
* **scripts:** verify-linux-package.sh 的 arch 分支按容器/实机分流 ([ffd80f7](https://github.com/brightmeows/BeAtBench/commit/ffd80f7f6b092dc5bd3b20dcd99c67cda0f2e40c))
* **session:** invalidate timing and sample hashes on chart edits ([1460ab0](https://github.com/brightmeows/BeAtBench/commit/1460ab0976efa6cc6ae6e53a9081531c451c72ee))
* **skin:** 发布包随包分发内置皮肤，目录解析不再依赖工作目录 ([737f023](https://github.com/brightmeows/BeAtBench/commit/737f02345a727d3a30c7e6b44e10e62fe67f713a))
* **slice:** ch01 子行上限 12 → 64（beatoraja 默认 64 采样序列） ([cfc04a7](https://github.com/brightmeows/BeAtBench/commit/cfc04a7f9286e064bedc84598f907ecf2b6f9f07))
* **slice:** Ctrl 临时勾选改同路径 toggle + 导出起始ID 字母输入与连续导出递进 ([da5a961](https://github.com/brightmeows/BeAtBench/commit/da5a96193d557875adf97537610e37dd20e937c1))
* **slice:** Ctrl 临时勾选改直接写外部状态 + 导出崩溃埋点 ([ffcd845](https://github.com/brightmeows/BeAtBench/commit/ffcd845c136c3ffd5c2ea31411d17fa700c96bd0))
* **slice:** follow chart base in export controls ([b50128e](https://github.com/brightmeows/BeAtBench/commit/b50128ee1dd72bc3560394b9848da351eeabedd3))
* **slice:** guard base62 paste and stop false place success ([044f398](https://github.com/brightmeows/BeAtBench/commit/044f398e1bbdc2e7f87b3de0441327c58112e27d))
* **slice:** M6.2 后续修复——offset 实时生效 + 播放自然播完复位 ([563355f](https://github.com/brightmeows/BeAtBench/commit/563355f8f5d02697247e9fb73b5a43ae376fd862))
* **slice:** move cursor by row with viewport margin ([39e8b11](https://github.com/brightmeows/BeAtBench/commit/39e8b11754e81aed3c3ee478af509e9a7f144c63))
* **slice:** prompt before overwriting export wav names ([bcbfc26](https://github.com/brightmeows/BeAtBench/commit/bcbfc2691311c77fcfae95d360331ecf47e6f468))
* **slice:** refresh export id range with selected base ([aba7bc3](https://github.com/brightmeows/BeAtBench/commit/aba7bc382980c6d300f5ac6e9e70bbeeea1381d2))
* **slice:** reject base mismatch during chart placement ([f0c394d](https://github.com/brightmeows/BeAtBench/commit/f0c394dd4164944571825245ebdb44b42d02da25))
* **slice:** reject partial wav id export ([857957c](https://github.com/brightmeows/BeAtBench/commit/857957c37b45450bba092a958329f0f8bc6d2385))
* **slice:** stabilize free WAV ID lookup with bridge regression tests ([0a454dd](https://github.com/brightmeows/BeAtBench/commit/0a454ddd46e7ebcaae50ddb0c1caa32a08b411a3))
* **slice:** warn before base62 chart export ([7bf4b8c](https://github.com/brightmeows/BeAtBench/commit/7bf4b8c6a521b956ffca9f481a4b73a7140dcb62))
* **slice:** warn only when placing into loaded chart ([dc5e360](https://github.com/brightmeows/BeAtBench/commit/dc5e3605eaef49bd6668a9399b44f8b63b5d52e2))
* **slice:** 切音页列表列对齐 + 编辑对话框主题化排版 ([4517a52](https://github.com/brightmeows/BeAtBench/commit/4517a52a8c383f12bde069421c71bff14cd18b48))
* **slice:** 剪贴板子行硬限制取消 → lint 提示；铺入后采样面板即时刷新 ([cf8e582](https://github.com/brightmeows/BeAtBench/commit/cf8e582e5da682b52381682ef8ee7f277034e0af))
* **slice:** 手动切分交互改版——点击选中拍子 + 再击同一拍子切换（无双击阈值） ([73976c2](https://github.com/brightmeows/BeAtBench/commit/73976c282230c23826581aab5fccec931f11ab48))
* **slice:** 深档直读 PCM（正弦波不再糊成实心）+ 缩放加深到 1/8拍 + BPM 10-300 + seek 吸附拍子线 ([54a53ca](https://github.com/brightmeows/BeAtBench/commit/54a53ca1268c5024c443e72c65becd9912d7662d))
* **slice:** 滚轮缩放钳制（缩到全曲后继续缩小会拖住放大节奏） ([0f25c48](https://github.com/brightmeows/BeAtBench/commit/0f25c486d114b831ba54351ba6ab318d4f202b39))
* **slice:** 焦点描边被内容遮挡/贴边压字 + 快捷键提示（按钮/空态/tooltip） ([71fa78a](https://github.com/brightmeows/BeAtBench/commit/71fa78adda227f5cab7f232cd72d3f07239f16d2))
* **slice:** 网格参考线可见性重设计——四级层级 + 起点显眼标记 ([85027f6](https://github.com/brightmeows/BeAtBench/commit/85027f65376eb166e30377addf7c921eeadae3fe))
* **test:** Qt 测试目标接入常规构建——补 AUTOMOC 使 ui_action_registry_test 可链接运行 ([42764a1](https://github.com/brightmeows/BeAtBench/commit/42764a1c66be9203e6c51cd35ea80bdabd5ccd6a))
* **tests:** 补齐 GCC 下的缺失标准头（cmath/algorithm） ([bcac3a2](https://github.com/brightmeows/BeAtBench/commit/bcac3a24948b1c0e8dedb41590690e9e00672fae))
* **theme:** 启动即深色——窗口底/菜单栏显式绑主题 token（不再依赖平台 palette 继承） ([36c278e](https://github.com/brightmeows/BeAtBench/commit/36c278ee3634312c9377e22368aa79cd81fe81e6))
* **theme:** 菜单栏字体色 + 切音页输入框统一主题样式（BbSpinBox） ([f656ab6](https://github.com/brightmeows/BeAtBench/commit/f656ab613478f272a5faca4120f727072db6285d))
* **theme:** 菜单栏字体色改用 palette.text（Fusion MenuBarItem 标题取的是 text 角色） ([2f4db24](https://github.com/brightmeows/BeAtBench/commit/2f4db24e4eeb7c4653d8b5d95e2716c559a00f43))
* **ui:** BbSpinBox 上下箭头启用时全不透明——亮色皮肤(白底)不再几乎看不见 ([bd86fdc](https://github.com/brightmeows/BeAtBench/commit/bd86fdc55d5fa7a20667910d917e621492603be0))
* **ui:** BbSpinBox 上下箭头改走 Theme.text（原硬编码 #ffffff 在亮色皮肤不可见） ([6505890](https://github.com/brightmeows/BeAtBench/commit/6505890a89be570073464db990369385cd2df5b2))
* **ui:** BbSpinBox 上下箭头重叠成 X——indicator 需自行定位 ([f10c997](https://github.com/brightmeows/BeAtBench/commit/f10c9979cc6ae6bc8a11cfd5cd2d44af16ec21bb))
* **ui:** BbSpinBox 箭头改直接读 Theme.text（原 root.textColor 绑定在 Canvas paint 闭包里未随皮肤更新） ([4286fc5](https://github.com/brightmeows/BeAtBench/commit/4286fc595128e667faa3d2322d4bc2d35b940c70))
* **ui:** BGM 子轨放置传 bgm_line + 状态文案 + --drag/probe 坐标 ([402483f](https://github.com/brightmeows/BeAtBench/commit/402483fa76c23d62d5cf6c6021d45b6debdefecd))
* **ui:** BGM 子轨移动修复（bgm_line 字段一致化）+ LN 绘制/颜色 + 转换按钮 ([4856018](https://github.com/brightmeows/BeAtBench/commit/48560184e3011860ecfb6205445f087e13573166))
* **ui:** confirm unsaved charts before close, new, and open ([4536dc3](https://github.com/brightmeows/BeAtBench/commit/4536dc3f9325992d56500516758edea55926b0fa))
* **ui:** LN 跨通道断开不连线 + 深色残留（ln_channel）+ 测试更新 ([3f58fda](https://github.com/brightmeows/BeAtBench/commit/3f58fdaf98b66378e04862e195938d3a1d3eb8ed))
* **ui:** load user keymap after --skin and --keymap ([c58c0e6](https://github.com/brightmeows/BeAtBench/commit/c58c0e665f71eace3b83b4c9a81bd89f9d81044d))
* **ui:** note 拖到 BGA 图层列不再消失（columnFor 回退渲染） ([9264d13](https://github.com/brightmeows/BeAtBench/commit/9264d13518a318cb902b846483e3eca08b5e22b6))
* **ui:** theme base62 export warning dialog ([0b910f1](https://github.com/brightmeows/BeAtBench/commit/0b910f1b69e8e9a1fcb4ba71449915afad4d05e2))
* **ui:** 修复 Ctrl 多选 + LN 选取模式【问题1/5】 ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* **ui:** 多选跨通道移动只动拖起轨 + 移动/变换后保持选中 + 文本框焦点释放 + 元信息间距收窄 ([b5b90bb](https://github.com/brightmeows/BeAtBench/commit/b5b90bbe9ae5373c7d7888a196ee7ea2ef704f74))
* **ui:** 工具快捷键失效——textInputFocused 判定改为 inputMethodHints（用户反馈） ([9264d13](https://github.com/brightmeows/BeAtBench/commit/9264d13518a318cb902b846483e3eca08b5e22b6))
* **ui:** 快捷键/BGA 列保留修复 + LN 换轨保 LN ([9264d13](https://github.com/brightmeows/BeAtBench/commit/9264d13518a318cb902b846483e3eca08b5e22b6))
* **ui:** 量化/旋转/镜像/单点-LN 按钮解封 + 网格开关可视态 + 元信息面板收窄/去路径/更多字段折叠 ([f87a39f](https://github.com/brightmeows/BeAtBench/commit/f87a39ffe6cd696a7ecd1bf99cc633c2859ef958))
* **version:** Version.hpp / GUI 版本串与测试改为单一来源，补两道门禁 ([a513743](https://github.com/brightmeows/BeAtBench/commit/a513743e5f07ee1c728acd1c60cccd85ecf16e34))
* **version:** 修正 0.3.2 包自报版本 + 发布步骤修复（重发 v0.3.2） ([9fb427d](https://github.com/brightmeows/BeAtBench/commit/9fb427d2cdad9d77e9ac2a2f47605009cffeb0a0))
* **viewport:** 2P 皿移到 2P 键右侧（1P 皿在左、2P 皿在右） ([9977bf2](https://github.com/brightmeows/BeAtBench/commit/9977bf276445bd42ca7c59319ffd7695cd3b70f8))
* 下拉菜单特殊选项细化 ([2043330](https://github.com/brightmeows/BeAtBench/commit/2043330a62e0ab43da0e4fbb5b9f9023f0c34067))
* 修复采样面板空列表（QML String.arg 多参抛异常中止 openChart）+ 互斥单选组改外部激活态 ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* 左 Dock 标签条消失——BbTabStrip（Flickable）缺隐式高度在布局内为 0 宽不可见 ([c27f1f2](https://github.com/brightmeows/BeAtBench/commit/c27f1f2925a59bfb7d9dd95d100b0e35150a648d))
* 清理 bms_parser 未使用变量（消除 MinGW 警告，doc/07 §2 遗留项） ([2c8ed1d](https://github.com/brightmeows/BeAtBench/commit/2c8ed1da57238cf3ff6d0488aee7dd731c4b8714))
* 用户反馈批修（LN 配对/多选/配色/BGA 列）+ --probe 诊断探针 ([4da4139](https://github.com/brightmeows/BeAtBench/commit/4da41391db919129946636b6084f25484a05eda8))
* 跨平台编译修复（GCC / Clang / CMake 4 / Qt 6.11 QML） ([45fd83b](https://github.com/brightmeows/BeAtBench/commit/45fd83b43f82c51754984b7da6d7fe91e44497e1))
* 采样添加布局 + BPM/STOP 编辑值默认空 ([554ea1f](https://github.com/brightmeows/BeAtBench/commit/554ea1f794ee7480c98a2566e51cabb79db9e69a))


### Performance Improvements

* **pkg:** Windows 包裁剪多余控件样式与 Qt Quick 3D 残留（zip -35.5%） ([316e645](https://github.com/brightmeows/BeAtBench/commit/316e645ba61961acc0c9c5202921e70407d0b7b2))
* **pkg:** Windows 包裁剪多余控件样式与 Qt Quick 3D 残留（zip −35.5%） ([3c02911](https://github.com/brightmeows/BeAtBench/commit/3c02911da885733a8ae539c8275cc05522ac81e8))

## [0.4.0](https://github.com/wufe8/BeAtBench/compare/v0.3.2...v0.4.0) (2026-10-11)


### Features

* **slice:** 瞬态检测 audio 源（OnsetDetector + 桥层落手动点） ([7806d0c](https://github.com/wufe8/BeAtBench/commit/7806d0cba8957161ea5071fbb668830ceefcb246))
* **slice:** 瞬态检测切片源（M6.5，按音频变化自动选切音点） ([096f0dd](https://github.com/wufe8/BeAtBench/commit/096f0dd618442636c68c822f5e78b497e6882685))
* **slice:** 瞬态检测源接入切音页 + 文档同步 ([7d5897a](https://github.com/wufe8/BeAtBench/commit/7d5897a9c75b98d5a3461cd22f7bbf2a3a7a5a83))


### Bug Fixes

* **build:** 禁用 Qt 版本哨兵，发行版产物不再绑定 Qt 小版本 ([33b68bd](https://github.com/wufe8/BeAtBench/commit/33b68bdedd26d6ea8734f37a09f7d39cf0b4276a))
* **build:** 禁用 Qt 版本哨兵，发行版包不再绑定 Qt 小版本（v0.3.2 arch 包无法启动的根因） ([cc093f2](https://github.com/wufe8/BeAtBench/commit/cc093f23587eb2cfba1fea651833b4d662d0d53b))
* **ci:** ccache 统计步骤兼容 macOS bash 3.2 的多字节变量解析 ([83d312c](https://github.com/wufe8/BeAtBench/commit/83d312c33f82dfebcc2bebc6fdc1e53debef70e3))
* **ci:** macos Build 显式 --parallel 3，杜绝 make -j 无限并发 ([0ff2d36](https://github.com/wufe8/BeAtBench/commit/0ff2d36b6e6cbb40931e2aca746f7d1d4229bef2))
* **ci:** macos ccache 路径对齐 ccache 默认值，修正注释归因 ([90d06ba](https://github.com/wufe8/BeAtBench/commit/90d06bac6fc8a291cb2f7bd16e906fa759c7dec2))
* **package-linux:** arch 包 depends 显式 qt6-base&gt;=6.4 下限 ([9dbeee5](https://github.com/wufe8/BeAtBench/commit/9dbeee504b33e818b8b25229c74b1e6f328a3dfe))
* **pkg:** arch 包维护者字段改为 env PACKAGER —— makepkg 不读 PKGBUILD 变量 ([7c4a67b](https://github.com/wufe8/BeAtBench/commit/7c4a67bae660430859475cc050b8843513cfab66))
* **pkg:** PR [#5](https://github.com/wufe8/BeAtBench/issues/5) arch 验收 follow-up（packager 字段 / 实机部分升级 / 悬停高亮可自动化） ([cb3d6fb](https://github.com/wufe8/BeAtBench/commit/cb3d6fbba280dbd20d18b66bf43847c5eaf66283))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([554b111](https://github.com/wufe8/BeAtBench/commit/554b1113208f68ed4183e6499e09fcae5149e183))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([999654a](https://github.com/wufe8/BeAtBench/commit/999654a86ef2b9107cedd09ac5c8ad767afbed79))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([444d61f](https://github.com/wufe8/BeAtBench/commit/444d61f81e8f0fa11799ca06b465842ef2e4be61))
* **release:** 产物文件名统一为 beatbench-v 前缀连字符格式 ([f8ef4d1](https://github.com/wufe8/BeAtBench/commit/f8ef4d1ce1bbdf2653c39c92e2da0ea9f16d508e))
* **release:** 修好 release-please 首次发版会踩的两处坑（README 包名被改坏 + 门禁漏放） ([7c9330a](https://github.com/wufe8/BeAtBench/commit/7c9330a9495d31daf4c41cf5e60186ee9c89bc48))
* **release:** 修好 release-please 首次发版会踩的两处坑（README 包名被改坏 + 门禁漏放） ([fe9241e](https://github.com/wufe8/BeAtBench/commit/fe9241ed62ae569035df50bf758e6e96cf1a765c))
* **scripts:** verify-linux-package.sh 的 arch 分支按容器/实机分流 ([ffd80f7](https://github.com/wufe8/BeAtBench/commit/ffd80f7f6b092dc5bd3b20dcd99c67cda0f2e40c))


### Performance Improvements

* **pkg:** Windows 包裁剪多余控件样式与 Qt Quick 3D 残留（zip -35.5%） ([316e645](https://github.com/wufe8/BeAtBench/commit/316e645ba61961acc0c9c5202921e70407d0b7b2))
* **pkg:** Windows 包裁剪多余控件样式与 Qt Quick 3D 残留（zip −35.5%） ([3c02911](https://github.com/wufe8/BeAtBench/commit/3c02911da885733a8ae539c8275cc05522ac81e8))

## [0.3.2] - 2026-10-08

> 打包版：**谱面编辑功能与 0.3.1 完全一致**，本版把发布面铺开——新增 Linux 发行版包
> （deb / rpm / arch），GUI 的 Qt 支持面从 6.11+ 下调到 6.4+，发布流程全 CI 化（含
> Linux 包构建 + 目标发行版安装冒烟 + 自动挂载 Release）。

### 新增

- **Linux 发行版包（首次发布）**：提供 deb / rpm / arch 三种包，使用**发行版系统 Qt**
  （不捆绑运行时，Qt 6.4+）：
  - `beatbench_<ver>-1_amd64.deb`（基线 Debian 12，一份覆盖 Debian 12+ 与 Ubuntu 24.04+，含
    time_t 迁移改名的 `t64` 备选依赖）；
  - `beatbench-<ver>-1.fc43.x86_64.rpm`（基线 Fedora 43，覆盖 Fedora 43+）；
  - `beatbench-<ver>-1-x86_64.pkg.tar.zst`（Arch 滚动版）。
  每个包都在目标发行版容器内**实际安装并冒烟**（Debian 12/13、Ubuntu 24.04/26.04、
  Fedora 43/44、Arch）：跑 `beatbench-cli version`，并在空目录里离屏启动 GUI、断言
  截图与皮肤切换日志。安装布局遵循发行版惯例（QML 模块进 Qt 导入目录、皮肤进
  `/usr/share/beatbench/skins`），随包提供 `.desktop`、hicolor 多尺寸图标与 AppStream
  元数据。详见 `doc/11`。
- **发布流水线全 CI 化**：推 `v*` tag 即产出全部平台产物——`release.yml` 跑完整回归后
  产 Windows zip 并建 GitHub Release；`package.yml` 产 Linux 三格式并**自动挂到同一个
  Release**。不再依赖本地 Windows/Linux 环境打包。

### 变更

- **GUI 的 Qt 支持面扩大为 6.4+**：QML 入口加载改用 6.4 可用写法（`loadFromModule`
  是 Qt 6.5+ API，6.4 回退到资源路径导入）。CI 新增 `probe-qt64`（Ubuntu 24.04 系统
  Qt 6.4.2）持续钉住下限，防止新代码悄悄用上 6.5+ API 而破坏发行版覆盖。
  原口径「Qt 6.11+」更新为「6.4+（CI 另在 6.11.2 上跑全量；Windows 包内嵌 6.11.2）」。
- **分隔条悬停高亮**改用 `HoverHandler`（手测确认生效），避开附加属性的作用域坑与
  非官方的 `SplitView.hovered`。

### 修复（发布基础设施，不影响编辑功能）

- `release.yml` 挂 Release 时找的 zip 名少一个 `v`（脚本实产 `beatbench-v<ver>-win64.zip`），
  真实发版会直接失败——已修正并加产物存在性断言。
- `check-version.sh` 新增 `--qt-floor`：文档同时宣传「CI/打包 Qt minor」与「代码 Qt 下限」
  时两者都算有效，否则 Qt 6.4 下限表述会被 `--strict` 判为过期而**阻塞发版**。
- 去掉 `package.yml` 中与 `release.yml` 重复的 Windows 打包作业（同一 tag 不再构建两遍）。
- **Qt 6.12 上无法构建**：Qt 6.12.0 的 `qmlcachegen` 在编译本模块时会确定性 SIGSEGV
  （解析模块自生成的 `.qmltypes` 时崩溃，且无任何错误输出），使 Arch（系统 Qt 6.12）
  的包与源码构建都失败。现在 Qt 6.12.x 上跳过 QML 字节码预编译（`NO_CACHEGEN`，
  功能等价，仅损失一点首屏编译开销），绕过该上游缺陷；其余 Qt 版本行为不变。
  详见 `doc/11` §2。
- **版本一致性门禁补两块**：`check-version.sh` 新增硬性检查「`Version.hpp` 的 `kVersion`
  必须与 CMakeLists 一致」（它是 CLI `version`、JSON 协议、GUI「关于」的版本来源，
  与 CMakeLists 各写一份），`package-release.sh` 另断言包自报版本等于打包版本——
  两层把「包名是 X 而二进制自报 Y」挡在打包/发版阶段。

## [0.3.1] - 2026-09-20

> 补丁版：修复 v0.1.0–v0.3.0 三版发布包**未随包分发内置皮肤**（`skins/`）的问题，并加入
> 拖拽/双击打开文件（定位为「修复 + 小功能」，未按 minor 推进版本号）。

### 新增

- **拖拽入窗口 / 拖到 exe 图标 / 双击文件关联打开文件**：把文件拖进主窗口任意位置（拖拽悬停时
  显示「松开以打开」提示），或把文件拖到 `beatbench.exe` 图标（Explorer 把路径作为 `argv[1]`
  传入）、双击已关联的文件，按后缀自动分流——谱面（`.bms/.bml/.bme/.pms`）进编辑页；音频
  （`.wav/.ogg/.oga/.mp3/.flac`）与 MIDI（`.mid/.midi`）进切音工作台作参考音频 / MIDI 导入；
  不支持的类型在状态栏提示且不改动当前文档。后缀分类是 C++ 纯函数
  （`EditUtils::classifyExternalFile`，单测覆盖），窗口拖拽与 exe 命令行两条入口共用同一路由
  （`Main.qml::handleExternalFile`）。

### 修复

- **发布包内置皮肤缺失（自 v0.1.0 起）**：`scripts/package-release.sh` 从未把仓库根 `skins/` 拷进
  staging，`windeployqt` 也只部署 Qt 运行时。用户解压发布包后，菜单「视图→皮肤」的四项
  （Aurora / Linear / OsuLight / Win10）全部报「缺 theme.json（皮肤目录未找到）」而不生效，皮肤的
  `keymap.json` 亦随之失效（皮肤文件在仓库内开发运行时因 `../..` 回退可被找到，故此前未暴露）。
  现随包分发 `skins/`（与 `beatbench.exe` 同级），打包含校验，并在冒烟阶段以
  `--apply-skin Aurora` + 断言日志「皮肤已运行时切换」把漏打包钉死在打包阶段。
- **皮肤目录解析不再依赖工作目录**：`ThemeManager` 的搜索基准新增「可执行文件目录（及其上级）」，
  快捷方式/其他启动器的工作目录 ≠ exe 目录时仍能定位内置皮肤；启动参数 `--skin <dir>` 按同一规则
  回退解析，主题与伴生 `keymap.json` 共用一次解析结果。

## [0.3.0] - 2026-09-09

> 已发布（Windows 64 位包见 GitHub Release）。本节已并入原 `[Unreleased]` 的全部增量：
> M6 切音工作台与 2026-09 收尾。

### 新增（切音工作台，M6）

- **导入**：参考音频（stem.wav 解码 + 波形预览 + 播放/seek）+ MIDI 导入（note 表 + tempo/拍号）+ offset 全局微调（音频/MIDI 对齐）。
- **切分**：网格（BPM/细分）与 MIDI 两种位置源；「到下一起点」模式（右边界 = 下一唯一起始/音频末尾，与手动切片一致，和弦自动合并）；手动切分点（波形点选-再击添加/删除、右键删除、纯手动模式）。
- **切片列表**：放置开关（导出预选）、双击行编辑边界（起始/持续）、来源/和弦计数（×N）。
- **导出**：分片落盘（fade 防爆音、前缀可配置、起始 `#WAV` id 分配 + 连续导出递进）+ 可复制 BMS raw（`#WAV` 定义 + ch01 铺放行 + 小节长）。
- **「同时铺入编辑区」导出开关**（导出按钮旁）：导出后直接把 `#WAV` 定义 + ch01 铺放行应用进当前谱面（一个撤销步）；**子行接续**——新内容从目标小节段已有子行之后开始（同 tick 不挤旧行、旧行结构不动、新内容跨小节同列）。
- **编辑页 Ctrl+V 整段粘贴**：系统剪贴板 BMS 原始行（切音 raw / 手写 `#00101 AA` / 外部工具复制）直接粘进谱面，`#WAV` 定义/小节长一并应用（一个 undo 步）。
- **工作台交互**：换行视口（每行固定时长 + 整行滚动/缩放 + 深档 PCM 直读）、对话框统一主题化（BbDialog，皮肤可整体换肤）。
- **切音页键盘（woslicer 系）**：空格=播放/暂停参考音频、←→=选中拍子移动（1 网格细分，视口跟随）、↑↓=视口逐行、Z=放置/消去切分点、C=清除全部切分点、V/B=复制/粘贴切分点（替换式）；Ctrl+G 生成切片、Ctrl+Shift+G 清除切片、Ctrl+E 导出、Ctrl+I/M 导入音频/MIDI；编辑页与切音页 `-`/`=` 缩放。快捷键走动作注册表；按钮/复选框点击不抢焦点（TabFocus），文本编辑时快捷键让行。
- **焦点区域高亮（PR 式）**：点击编辑区/dock（编辑页与切音页，共 6 区）→ 区域边缘 `focusRing` 描边提示当前焦点所在；切页自动清除。

### 新增（快捷键与状态栏，2026-09）

- **快捷键注册表收尾**：Space（谱面播放/暂停）、Ctrl+R（渲染音频到文件）、Ctrl+,（首选项）、Esc（取消未完成的 LN）改为动作注册表统一入口，设置页可改绑（Ctrl+, 现可录键）。新增**作用域**：编辑页与切音页可各绑同一按键（当前两页 Space 各一次），全局动作与页面动作仍互斥。
- **切音页状态栏摘要**：底部全局状态栏在切音页显示参考音频（文件名/时长/采样率/偏移）、播放状态与播放头、视口（行 N/M · 时间范围 · 档位）、选中光标、切片总数/启用数、MIDI 音符数；与编辑页状态互斥。

### 修复

- **Base62 粘贴保护**：Base62 导出的 raw 现在带 `#BASE 62` 声明行（粘贴本身不改目标谱面进制）；Ctrl+V 把含 Base62 id（`#BASE 62` 声明或小写 id）的内容粘进未声明 `#BASE 62` 的谱面前先确认——两位 id 在两种进制下数值不同（如 `10`），避免按错误 id 覆盖 `#WAV` 定义。
- 「同时铺入编辑区」/ Ctrl+V 粘贴只在 `clipboard.paste` 真正应用成功时报告成功（此前只查协议顶层 ok，exec 失败会被误报为已铺入/已粘贴）。
- MIDI 切片右边界与手动切片语义对齐（此前按 note 结束切分：和弦重复片 + 尾音丢失）。
- 「同时铺入编辑区」后左 dock 采样列表即时刷新（#WAV 定义新入谱面 → 采样面板可见）。
- ch01 子行不再设硬上限（初版 12 → 64 都取消）：超出 beatoraja 默认 64 采样序列的同 tick 并发由 lint 提示「同时播放采样可能过多: [数量], [位置]」（`bgm_polyphony_overflow`），不阻止铺放。
- 切音页列表列对齐（RowLayout 列宽被布局系统忽略）；编辑对话框排版/主题化；MIDI 序号列 ReferenceError。
- 拖拽（pan）工具下 BGA/BPM/STOP 对象不再被拖动（与 note 的 pan 语义一致：只点选/试听）。
- 播放头红线/A-B 标记在改变小节长度（ch02 变拍）的小节处错位：换算改走内容同源的 `yForSec`（变拍高感知）。
- 焦点区域高亮：编辑页编辑区描边被铺满的子项覆盖（只剩底部可见）→ 描边改顶层；切音页/编辑区内容距边 4px，避免被 2px 描边遮挡。
- 快捷键提示：播放按钮文本显示「空格」、左 dock 空态说明含键盘一览（读注册表显示——将来改绑自动更新提示）。
- 切音导出同名前缀不再静默覆盖：无冲突仍从 `000` 起；有冲突必须选择覆盖 / 续号 / 取消。`--slice-export` 撞名失败且不写盘。文件序号只给勾选切片从 000 连续编号（未勾选不占号）。
- 保存 / 崩溃备份 / 切音 WAV 导出改为临时文件写完再替换：写入失败不损坏原文件、不报成功；备份失败会在保存状态中可见。
- 复制/粘贴保留地雷、LN 通道与 BGM 子行；半选 LN 拒绝且不改谱面。框选删除一次 Undo 全部恢复。粘贴后自动选中新 note。
- 切音页 Ctrl+Z/Y 撤销切片/手动点编辑，不再误撤隐藏谱面；文本框内仍撤文本。重建切片会清空手动点集合；手动拆分保留音高并标为手动。
- 未保存的谱面在关闭 / 新建 / 打开前会确认：保存、放弃或取消。保存失败不替换当前文档；无改动不多弹窗。窗口标题对脏文档加 `*`。切音工作区不并入谱面脏。Ctrl+N 新建走同一确认。
- 改初始 BPM、同 ID 换采样文件、切谱会刷新秒标尺/波形并丢掉过期渲染，避免旧谱音频回流。
- 首选项「快捷键」页可运行时改绑：确定后立即生效并落盘（Windows：`HKCU\Software\BeAtBench\BeAtBench\keymap`），取消还原；冲突拒绝。用户改绑优先于皮肤 keymap。启动 `--skin` 不再丢掉用户键位。Ctrl+N 新建已接上。文本框内快捷键仍让行。页内显示配置位置，可复制路径或打开（Windows 导出 `BeAtBench-keymap.reg` 便于迁移）。

## [0.2.0] - 2026-09

> 相对 v0.1.0（M1-M3）的重大版本：新增完整音频链路（M4）与随时播放（M5），并做了一批 note 编辑增强。

### 新增（音频，M4）

- **单发试听**：采样面板点击播放（PortAudio WASAPI 输出 + miniaudio 解码 wav/ogg/mp3/flac）。
- **音频设置页**：设备/API 筛选、采样率、缓冲、音量、测试音。
- **采样解码缓存**：LRU + 预算 + in-flight 并发合并（SampleCache）。
- **离线渲染**：`ChartRenderer`（区间渲染 + WAV 写出），暴露为 `cli render` 与编辑器 Space 渲染；带 `.render.wav` 缓存与后台线程（UI 不卡）、版本化丢弃过期结果。
- **波形显示**：min/max 多级金字塔（WaveformPyramid）+ 右侧垂直波形条（视口窗指示、点击/拖动跳转）+ 左列秒标尺（小节号 + 起点秒，支持变速/STOP）。
- **增量重渲染**：编辑 note 自动增量（脏区间重混 + 金字塔局部重建）；变速谱面全量兜底。
- 渲染兼容 ogg 扩展名 / 非 ASCII 路径；修复解码失败误报导致的渲染崩溃。

### 新增（播放/试听，M5）

- **M5.1 随时播放**：Space 播放/暂停；载入自动渲染（内存化，不写盘）；PcmEngine 零拷贝预渲染 PCM 播放；播放时钟；编辑即停；`Ctrl+R` 手动渲染写盘。
- **M5.2 播放头红线 + 视口跟随 + A-B 循环 + seek**：红线 = 视口光标（固定视口底部 10%）；跟随（默认开，用户滚动自动关）；A/B 打点（同点再点解除）+ 循环；点秒标尺 / 波形条拖动 scrub 定位；开头加 2 小节留白修底部起播跳变。

### 新增（note 编辑增强）

- 放置/移动鼠标预览 ghost（含多选通道平移、绝对 snap 吸附）。
- 多选/框选拖动按实际移动通道逻辑平移；BGM 多选全转 + 相对距离保持。
- 踏板（ch17）移入「更多轨道」；移动 note 波形条不重排。
- 背景 tile 对齐（topHigh）+ 水平滚动列底色重排。
- `bgm_line` 泛化为 `sub_line`（子通道/子行）+ `BmsChannelRule::allow_sub_lines` flag + lint 软警告；note 移动机制/数据模型抽象统一说明（doc/04 §6）。
- **02 通道小节长度（ch02）**：时间轴新增「小节长」页，可添加/编辑/删除每小节拍数（`timing.put/delete kind="measure"`）；编辑器按拍数**等比小节高**渲染（4/4=基准），并在左标尺非 4/4 小节标「×N」（N=相对 4/4 倍数）；播放/跟随/seek/秒标尺同步正确。

### 修复

- note 标签对齐/高度居中；BPM/STOP 轨秒行完整显示；波形条点击/拖动捕获。
- BGM 跨通道/误高亮/相对距离多轮修复（`noteRefKey`/`sourceBgmLine`/列秩 gap 等）。
- 播放/循环点与红线（视口光标）统一；只设 B 则到 B 暂停。
- 修复编辑（BPM/STOP/小节长/节拍等 timing 变化）或撤销后右侧波形图消失：timing 编辑此前不触发重渲染、内容变化会隐藏波形 → 现改为任何内容变化都自动后台重渲染、内容变化不再隐藏波形。

### 测试

- 完整回归全绿：core 263 用例（含真实谱面时序自洽）+ 音频 14 / 解码缓存 5 / 离线渲染 8 / 波形金字塔 8 / 播放 13。

### 已知限制（详见 doc/04 §6）

- 切音工作台（M6）与实时 keysound 调度（Phase D）未实现。
- `#RANDOM/#IF` 块内容保存后不保证数据一致。
- `.pms` 另存为 `.bms` 会按 sp/dp 重解释（跨扩展名另存属已知边界）。
- 增量重渲染的采样尾音余量按 8s 截断（更长采样为已知限制）。
