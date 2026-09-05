// SPDX-License-Identifier: GPL-3.0-only
// 切音页（M6.1 导入工作台）：参考音频（stem.wav）+ MIDI（notes.mid）导入、
// 波形预览 + 播放/seek + offset 微调（全局）。M6.2 起叠加切片线/列表。
// M6.3c UI 改良（用户 2026-09）：控件瘦身 + 左 dock（MIDI 音符/切片/网格三页签，
// 网格页留待手动切片）、raw 右 dock（IDE 式输出）、「MIDI 线」Ctrl 临时开关
// （网格模式参考 MIDI 线，为手动切片做准备）、导出加「起始小节」。
// 数据侧全部在 SliceWorkspace（C++）；本页只做装配与交互（doc/08 §2 双语言纪律）。
// woslicerII 参考（键盘 + 网格节拍 + 末端 fade + 無音切）→ M6.2 切分交互时实现。
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import BeatBench

Item {
    id: root

    /// 当前播放头（秒；<0 = 无）。播放中由下方 Timer 刷新（audioEngine 20Hz 信号在此聚合）。
    property real playheadSec: -1
    /// M6.3 导出结果（exportSlices 返回 map）+ 可复制 raw。
    property var exportResult: null
    property string rawText: ""
    /// 「MIDI 线」开关实际态（网格模式默认关；Ctrl 临时勾选 = 经 checkbox.toggle() 同路径翻
    /// 转，松开还原——与正常点击走同一条 onToggled 链路，checkbox 视觉同步真实状态）。
    property bool midiLinesOn: false
    /// 左 dock 页签：0 = MIDI 音符 / 1 = 切片（放置开关）/ 2 = 网格（手动切片占位）。
    property int dockTab: 0
    /// Ctrl 临时勾选状态机：保存按下前状态；松开仍处翻转态才还原（期间用户点过 = 以其为准）。
    property bool _midiCtrlActive: false
    property bool _midiCtrlSave: false

    function defaultOutDir() {
        var cPath = (typeof chartSession !== "undefined" && chartSession.path) ? chartSession.path : ""
        var base = cPath.length ? cPath : sliceWorkspace.audioPath
        var i = Math.max(base.lastIndexOf("/"), base.lastIndexOf("\\"))
        return i >= 0 ? base.substring(0, i) : ""
    }
    // #WAV id 文本（36 进制、2 位大写）：1 → "01"，10 → "0A"，1295 → "ZZ"
    function idTextOf(v) {
        var s = parseInt(v, 10).toString(36).toUpperCase()
        while (s.length < 2) s = "0" + s
        return s
    }
    function idValueOf(text) {
        var t = ("" + text).trim().toUpperCase()
        for (var i = 0; i < t.length; ++i) {
            var c = t.charAt(i)
            var code = c.charCodeAt(0)
            var ok = (c >= "0" && c <= "9") || (c >= "A" && c <= "Z")
            if (!ok) return -1
        }
        var v = parseInt(t, 36)
        if (isNaN(v) || v < 1) v = 1
        if (v > 1295) v = 1295   // ZZ 上限（与 SpinBox to 一致）
        return v
    }
    function doExport() {
        var dir = defaultOutDir()
        var prefix = prefixBox.text.length ? prefixBox.text : "slice"
        var r = sliceWorkspace.exportSlices(bpmBox.value, subBox.value, 4,
                                            exportIdBox.value, startMeasureBox.value,
                                            dir, prefix, 1.0)
        exportResult = r
        rawText = (typeof r.raw === "string") ? r.raw : ""
        if (r.ok) {
            // 连续导入导出：起始 id = 本次分配的最大 id + 1（跳过已占用；无谱面也是连续）
            exportIdBox.value = (typeof r.nextStartId === "number" && r.nextStartId >= 1)
                                ? r.nextStartId : sliceWorkspace.nextFreeWavId()
        }
    }
    // ---- 调试（main.cpp --slice-detect/--slice-export 同路径）----
    /// 切换切片源 UI（MIDI/网格；连带 MIDI 线默认态），供 --slice-detect 注入。
    function debugSetSource(source) {
        sliceSourceBox.currentIndex = (source === "midi") ? 1 : 0
    }
    /// 以指定起始 #WAV id 导出（与页面「导出分片」同一路径；raw 进右 dock）。
    function debugExport(startId) {
        exportIdBox.value = startId
        doExport()
    }

    Timer {
        interval: 100
        running: audioEngine.refPlaying
        repeat: true
        onTriggered: root.playheadSec = audioEngine.refPositionSec
    }

    function urlToPath(url) {
        var s = url.toString()
        s = s.replace(/^file:\/\//, "")
        if (s.charAt(0) === "/" && /^\/[A-Za-z]:/.test(s))
            s = s.slice(1)
        return decodeURIComponent(s)
    }

    function fmtTime(sec) {
        if (typeof sec !== "number" || !isFinite(sec) || sec < 0) sec = 0
        var m = Math.floor(sec / 60)
        var s = sec - m * 60
        var ss = s < 10 ? "0" + s.toFixed(2) : s.toFixed(2)
        return m + ":" + ss
    }

    Component.onCompleted: {
        // 起始小节默认 = 下一空小节（当前谱面已用小节数 + 1；无谱面 = 1）
        startMeasureBox.value = sliceWorkspace.suggestedStartMeasure()
        // MIDI 线默认随切片源（MIDI = 显示；网格 = 隐藏）
        root.midiLinesOn = sliceSourceBox.currentIndex === 1
    }

    // Ctrl 临时勾选：按下 → 直接翻转外部状态 midiLinesOn（与用户点击同一条状态链路：
    // checkbox 经 `checked:` 绑定跟随显示；midiVisible 直读 midiLinesOn——⚠️ 不要用
    // checkbox.toggle()：实测 Qt 6.11 它只改内部 checked 不发 toggled，外部状态不更新）；
    // 松开 → 仍处翻转态则还原。按住期间用户点过 checkbox = 用户意图优先（松开不还原）。
    Connections {
        target: keyMonitor
        function onCtrlHeldChanged() {
            if (keyMonitor.ctrlHeld && !root._midiCtrlActive && midiLinesBox.enabled) {
                root._midiCtrlActive = true
                root._midiCtrlSave = root.midiLinesOn
                root.midiLinesOn = !root.midiLinesOn
            } else if (!keyMonitor.ctrlHeld && root._midiCtrlActive) {
                root._midiCtrlActive = false
                if (root.midiLinesOn === !root._midiCtrlSave)
                    root.midiLinesOn = root._midiCtrlSave
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        // ---- 工具条：导入 / 清除 / offset / 播放控制 ----
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            BbToolButton {
                text: qsTr("导入音频…")
                enabled: !sliceWorkspace.busy
                onClicked: audioFileDialog.open()
            }
            BbToolButton {
                text: qsTr("导入 MIDI…")
                onClicked: midiFileDialog.open()
            }
            BbToolButton {
                text: qsTr("清除")
                onClicked: {
                    sliceWorkspace.clearAll()
                    root.playheadSec = -1
                }
            }
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                color: Theme.border
            }
            Label {
                text: qsTr("偏移(ms)")
                color: Theme.textMuted
            }
            BbSpinBox {
                id: offsetBox
                from: -5000
                to: 5000
                value: Math.round(sliceWorkspace.offsetSec * 1000)
                editable: true
                onValueModified: sliceWorkspace.setOffsetSec(value / 1000.0)
            }
            Item { Layout.fillWidth: true }
            BbToolButton {
                text: audioEngine.refPlaying ? qsTr("暂停") : qsTr("播放")
                enabled: audioEngine.refHasPcm
                onClicked: audioEngine.refTogglePlay()
            }
            BbToolButton {
                text: qsTr("停止")
                onClicked: {
                    audioEngine.refStop()
                    root.playheadSec = audioEngine.refPositionSec
                }
            }
            Label {
                text: root.playheadSec >= 0
                      ? (fmtTime(root.playheadSec) + " / " + fmtTime(sliceWorkspace.audioDurationSec))
                      : fmtTime(sliceWorkspace.audioDurationSec)
                color: Theme.text
                font.family: Theme.fontMono
            }
        }

        // ---- M6.2 切片控制：位置源（网格/MIDI）+ BPM/细分 + 生成/清除 + MIDI 线开关 ----
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Label { text: qsTr("切片源"); color: Theme.textMuted }
            BbComboBox {
                id: sliceSourceBox
                model: [qsTr("网格"), qsTr("MIDI")]
                implicitWidth: 84
                onCurrentIndexChanged: root.midiLinesOn = (currentIndex === 1)
            }
            Label {
                text: qsTr("BPM")
                color: Theme.textMuted
                visible: sliceSourceBox.currentIndex === 0
            }
            BbSpinBox {
                id: bpmBox
                from: 40
                to: 300
                value: Math.round(sliceWorkspace.midiTempoBpm)
                editable: true
                visible: sliceSourceBox.currentIndex === 0
            }
            Label {
                text: qsTr("细分/拍")
                color: Theme.textMuted
                visible: sliceSourceBox.currentIndex === 0
            }
            BbSpinBox {
                id: subBox
                from: 1
                to: 16
                value: 4
                editable: true
                visible: sliceSourceBox.currentIndex === 0
            }
            BbToolButton {
                text: qsTr("生成切片")
                onClicked: {
                    if (sliceWorkspace.detectSlices(
                            sliceSourceBox.currentIndex === 0 ? "grid" : "midi",
                            bpmBox.value, subBox.value, sliceWorkspace.audioDurationSec)) {
                        root.dockTab = 1   // 生成成功 → 切到「切片」页签核对放置开关
                    }
                }
            }
            BbToolButton {
                text: qsTr("清除切片")
                enabled: sliceWorkspace.hasSlices
                onClicked: sliceWorkspace.clearSlices()
            }
            // M6.3c：网格模式下显示 MIDI 线参考（Ctrl 按住临时取反；同编辑页「通道序号」）
            BbCheckBox {
                id: midiLinesBox
                text: qsTr("MIDI 线")
                checked: root.midiLinesOn
                enabled: sliceWorkspace.hasMidi
                ToolTip.visible: hovered
                ToolTip.text: qsTr("显示 MIDI 音符线（Ctrl 按住临时切换；网格模式下手动切片参考）")
                onToggled: root.midiLinesOn = checked
            }
            Item { Layout.fillWidth: true }
            Label {
                text: sliceWorkspace.hasSlices
                      ? qsTr("切片 %1 个").arg(sliceWorkspace.slices.length)
                      : qsTr("（未生成切片——网格需 BPM/细分，MIDI 需已导入）")
                color: Theme.textMuted
                elide: Text.ElideRight
            }
        }

        // ---- 三栏：左 dock（MIDI 音符/切片/网格页签）｜中央波形｜右 dock（raw） ----
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 6

            handle: Rectangle {
                implicitWidth: 4
                color: SplitView.hovered ? Theme.accent : Theme.border
            }

            // ================= 左 dock =================
            Item {
                SplitView.preferredWidth: 250
                SplitView.minimumWidth: 180
                SplitView.maximumWidth: 360

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4

                    BbTabStrip {
                        id: dockTabs
                        Layout.fillWidth: true
                        model: [qsTr("MIDI 音符"), qsTr("切片"), qsTr("网格")]
                        currentIndex: root.dockTab
                        onIndexRequested: (i) => root.dockTab = i
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Theme.surface
                        border.width: 1
                        border.color: Theme.border
                        radius: Theme.radiusSm
                        clip: true

                        // ---- 页签 0：MIDI 音符表（无 MIDI → 提示） ----
                        Label {
                            anchors.fill: parent
                            visible: root.dockTab === 0 && !sliceWorkspace.hasMidi
                            text: qsTr("（未导入 MIDI——点上方「导入 MIDI…」）")
                            color: Theme.textFaint
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 4
                            model: sliceWorkspace.midiNotes
                            clip: true
                            visible: root.dockTab === 0 && sliceWorkspace.hasMidi
                            delegate: RowLayout {
                                required property var modelData
                                width: ListView.view.width
                                spacing: 4
                                Label { text: modelData.pitch; width: 32; color: Theme.text; font.family: Theme.fontMono }
                                Label { text: modelData.channel + "ch"; width: 26; color: Theme.textMuted }
                                Label { text: "T" + modelData.track; width: 26; color: Theme.textMuted }
                                Label {
                                    text: (modelData.startSec + sliceWorkspace.offsetSec).toFixed(3)
                                    width: 62; color: Theme.accent2; font.family: Theme.fontMono
                                }
                                Label {
                                    text: (modelData.endSec - modelData.startSec).toFixed(3) + "s"
                                    color: Theme.textMuted; font.family: Theme.fontMono
                                }
                            }
                        }

                        // ---- 页签 1：切片表（放置开关；无切片 → 提示） ----
                        Label {
                            anchors.fill: parent
                            visible: root.dockTab === 1 && !sliceWorkspace.hasSlices
                            text: qsTr("（先生成切片）")
                            color: Theme.textFaint
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 4
                            model: sliceWorkspace.slices
                            clip: true
                            visible: root.dockTab === 1 && sliceWorkspace.hasSlices
                            delegate: RowLayout {
                                required property var modelData
                                width: ListView.view.width
                                spacing: 4
                                CheckBox {
                                    checked: modelData.enabled
                                    onToggled: sliceWorkspace.setSliceEnabled(modelData.index, checked)
                                    implicitHeight: 20
                                }
                                Label { text: modelData.index; width: 28; color: Theme.textMuted }
                                Label {
                                    text: modelData.startSec.toFixed(3)
                                    width: 62; color: Theme.accent2; font.family: Theme.fontMono
                                }
                                Label {
                                    text: modelData.durationSec.toFixed(3) + "s"
                                    width: 50; color: Theme.textMuted; font.family: Theme.fontMono
                                }
                                Label {
                                    text: modelData.kind === "midi"
                                          ? ("MIDI " + modelData.note)
                                          : qsTr("网格")
                                    color: Theme.text
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        // ---- 页签 2：网格页（手动切片划分占位；M6.4 实现） ----
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8
                            visible: root.dockTab === 2

                            Label {
                                text: qsTr("网格 / 手动切片")
                                color: Theme.text
                                font.bold: true
                            }
                            Label {
                                text: qsTr("当前均分: BPM %1 · 细分 %2/拍 · 每小节 4 拍")
                                          .arg(bpmBox.value).arg(subBox.value)
                                color: Theme.textMuted
                            }
                            BbToolButton {
                                text: qsTr("手动切片划分…")
                                enabled: false
                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("开发中（M6.4）：点击波形/拖动范围手动分区")
                            }
                            Label {
                                Layout.fillWidth: true
                                text: qsTr("目前仅支持 BPM+拍子固定均分。规划：在波形上拖选区间生成手动切片；"
                                           + "「MIDI 线」开关（Ctrl 临时显示）可作对位参考。")
                                color: Theme.textFaint
                                wrapMode: Text.WordWrap
                            }
                            Item { Layout.fillHeight: true }
                        }
                    }
                }
            }

            // ================= 中央：波形 =================
            ColumnLayout {
                SplitView.fillWidth: true
                SplitView.minimumWidth: 320
                spacing: 6

                // 波形 + note 刻度 + 播放头 + 切片线 + 实时拍子网格参考
                SliceWaveformItem {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    workspace: sliceWorkspace
                    theme: Theme
                    playheadSec: root.playheadSec
                    gridVisible: sliceSourceBox.currentIndex === 0
                    gridBpm: bpmBox.value
                    gridSubdivision: subBox.value
                    // Ctrl 临时勾选已写入 midiLinesOn（checkbox.toggle 同路径），此处直连
                    midiVisible: root.midiLinesOn
                    onSeekRequested: {
                        audioEngine.refSeek(seconds)
                        root.playheadSec = audioEngine.refPositionSec
                    }
                }

                // ---- 状态行 ----
                Label {
                    Layout.fillWidth: true
                    text: sliceWorkspace.statusText
                    color: Theme.textFaint
                    elide: Text.ElideRight
                }
            }

            // ================= 右 dock：WAV 定义 · ch01（raw） =================
            Item {
                SplitView.preferredWidth: 250
                SplitView.minimumWidth: 180
                SplitView.maximumWidth: 400

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Label {
                            text: qsTr("WAV 定义 · ch01")
                            color: Theme.textMuted
                        }
                        Item { Layout.fillWidth: true }
                        BbToolButton {
                            text: qsTr("复制 raw")
                            enabled: root.rawText.length > 0
                            onClicked: sliceWorkspace.copyToClipboard(root.rawText)
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Theme.surface
                        border.width: 1
                        border.color: Theme.border
                        radius: Theme.radiusSm
                        clip: true

                        ScrollView {
                            anchors.fill: parent
                            anchors.margins: 2
                            visible: root.rawText.length > 0
                            clip: true
                            TextArea {
                                text: root.rawText
                                readOnly: true
                                wrapMode: TextEdit.NoWrap
                                font.family: Theme.fontMono
                                font.pixelSize: Theme.fsTiny
                                color: Theme.text
                                background: Rectangle { color: "transparent" }
                            }
                        }
                        Label {
                            anchors.fill: parent
                            anchors.margins: 10
                            visible: root.rawText.length === 0
                            text: qsTr("导出后此处显示 #WAV 定义 + ch01 铺放行（可复制到编辑区）")
                            color: Theme.textFaint
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }

        // ---- M6.3 导出：起始 #WAV id + 起始小节 + 前缀 + 导出分片（raw 在右 dock） ----
        RowLayout {
            Layout.fillWidth: true
            spacing: 6
            Label { text: qsTr("导出起始ID"); color: Theme.textMuted }
            BbSpinBox {
                id: exportIdBox
                from: 1
                to: 1295
                value: sliceWorkspace.nextFreeWavId()
                editable: true
                // 36 进制 id 输入：默认 IntValidator 只放行数字 → 覆盖为字母可入（A0-ZZ/a0-zz）
                // ⚠️ Qt 6.11 起 RegExpValidator 已移除 → 用 RegularExpressionValidator
                validatorOverride: RegularExpressionValidator { regularExpression: /^[0-9A-Za-z]{0,3}$/ }
                textFromValue: function(value) { return root.idTextOf(value) }
                valueFromText: function(text, locale) { return root.idValueOf(text) }
                ToolTip.visible: hovered
                ToolTip.text: qsTr("起始 #WAV id（36 进制：01-99/A0-ZZ；可填字母）")
            }
            Label { text: qsTr("起始小节"); color: Theme.textMuted }
            BbSpinBox {
                id: startMeasureBox
                from: 1
                to: 999
                value: 1
                editable: true
                ToolTip.visible: hovered
                ToolTip.text: qsTr("ch01 起始小节（第 N 小节 = 文件 #(N-1)01:\n默认 = 下一空小节）")
            }
            Label { text: qsTr("前缀"); color: Theme.textMuted }
            BbTextField {
                id: prefixBox
                text: "slice"
                placeholderText: qsTr("slice 或 slices/slice")
                implicitWidth: 130
            }
            BbToolButton {
                text: qsTr("导出分片")
                enabled: sliceWorkspace.hasSlices && sliceWorkspace.hasAudio
                onClicked: root.doExport()
            }
            Item { Layout.fillWidth: true }
            Label {
                text: root.exportResult && root.exportResult.ok
                      ? (qsTr("已导出 %1 片").arg(root.exportResult.count)
                         + (root.exportResult.placementText
                            ? (" · " + root.exportResult.placementText) : ""))
                      : (root.exportResult
                         ? (qsTr("导出失败：") + root.exportResult.error)
                         : (sliceWorkspace.hasSlices ? "" : qsTr("（先生成切片）")))
                color: (root.exportResult && root.exportResult.ok) ? Theme.success : Theme.warning
                elide: Text.ElideRight
                Layout.maximumWidth: 420
            }
        }
    }

    FileDialog {
        id: audioFileDialog
        title: qsTr("导入参考音频")
        nameFilters: [qsTr("音频文件 (*.wav *.ogg *.mp3 *.flac)"), qsTr("所有文件 (*)")]
        onAccepted: {
            sliceWorkspace.loadAudioFile(urlToPath(selectedFile))
            root.playheadSec = audioEngine.refPositionSec
        }
    }
    FileDialog {
        id: midiFileDialog
        title: qsTr("导入 MIDI")
        nameFilters: [qsTr("MIDI 文件 (*.mid *.midi)"), qsTr("所有文件 (*)")]
        onAccepted: sliceWorkspace.loadMidiFile(urlToPath(selectedFile))
    }
}
