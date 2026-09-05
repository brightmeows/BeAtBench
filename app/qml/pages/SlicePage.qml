// SPDX-License-Identifier: GPL-3.0-only
// 切音页（M6.1 导入工作台）：参考音频（stem.wav）+ MIDI（notes.mid）导入、
// 波形预览 + 播放/seek + offset 微调（全局）。M6.2 起叠加切片线/列表。
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
        return isNaN(v) || v < 1 ? 1 : v
    }
    function doExport() {
        var dir = defaultOutDir()
        var prefix = prefixBox.text.length ? prefixBox.text : "slice"
        var r = sliceWorkspace.exportSlices(bpmBox.value, subBox.value, 4,
                                            exportIdBox.value, dir, prefix, 1.0)
        exportResult = r
        rawText = (typeof r.raw === "string") ? r.raw : ""
        if (r.ok) exportIdBox.value = sliceWorkspace.nextFreeWavId()
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
            SpinBox {
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

            // ---- M6.2 切片控制：位置源（网格/MIDI）+ BPM/细分 + 生成/清除 ----
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Label { text: qsTr("切片源"); color: Theme.textMuted }
                BbComboBox {
                    id: sliceSourceBox
                    model: [qsTr("网格"), qsTr("MIDI")]
                    implicitWidth: 84
                }
                Label {
                    text: qsTr("BPM")
                    color: Theme.textMuted
                    visible: sliceSourceBox.currentIndex === 0
                }
                SpinBox {
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
                SpinBox {
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
                        sliceWorkspace.detectSlices(
                            sliceSourceBox.currentIndex === 0 ? "grid" : "midi",
                            bpmBox.value, subBox.value, sliceWorkspace.audioDurationSec)
                    }
                }
                BbToolButton {
                    text: qsTr("清除切片")
                    enabled: sliceWorkspace.hasSlices
                    onClicked: sliceWorkspace.clearSlices()
                }
                Item { Layout.fillWidth: true }
                Label {
                    text: sliceWorkspace.hasSlices
                          ? qsTr("切片 %1 个").arg(sliceWorkspace.slices.length)
                          : qsTr("（未生成切片——网格需 BPM/细分，MIDI 需已导入）")
                    color: Theme.textMuted
                }
            }

            // ---- M6.3 导出：起始 #WAV id + 导出前缀 + 导出分片 + 复制 raw ----
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Label { text: qsTr("导出起始ID"); color: Theme.textMuted }
                SpinBox {
                    id: exportIdBox
                    from: 1
                    to: 1295
                    value: sliceWorkspace.nextFreeWavId()
                    editable: true
                    textFromValue: function(value) { return root.idTextOf(value) }
                    valueFromText: function(text, locale) { return root.idValueOf(text) }
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
                BbToolButton {
                    text: qsTr("复制 raw")
                    enabled: root.rawText.length > 0
                    onClicked: sliceWorkspace.copyToClipboard(root.rawText)
                }
                Item { Layout.fillWidth: true }
                Label {
                    text: root.exportResult && root.exportResult.ok
                          ? qsTr("已导出 %1 片").arg(root.exportResult.count)
                          : (root.exportResult
                             ? (qsTr("导出失败：") + root.exportResult.error)
                             : (sliceWorkspace.hasSlices ? "" : qsTr("（先生成切片）")))
                    color: (root.exportResult && root.exportResult.ok) ? Theme.success : Theme.warning
                    elide: Text.ElideRight
                    Layout.maximumWidth: 420
                }
            }
            // 可复制 BMS raw（#WAV 定义 + ch01 铺放行；粘贴到编辑区）
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: root.rawText.length > 0 ? 110 : 0
                visible: root.rawText.length > 0
                clip: true
                TextArea {
                    text: root.rawText
                    readOnly: true
                    wrapMode: TextEdit.NoWrap
                    font.family: Theme.fontMono
                    font.pixelSize: Theme.fsTiny
                    color: Theme.text
                    background: Rectangle { color: Theme.surface; border.color: Theme.border }
                }
            }

        // ---- 波形 + note 刻度 + 播放头 + 切片线 + 实时拍子网格参考 ----
        SliceWaveformItem {
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            workspace: sliceWorkspace
            theme: Theme
            playheadSec: root.playheadSec
            gridVisible: sliceSourceBox.currentIndex === 0
            gridBpm: bpmBox.value
            gridSubdivision: subBox.value
            onSeekRequested: {
                audioEngine.refSeek(seconds)
                root.playheadSec = audioEngine.refPositionSec
            }
        }

        // ---- 列表：有切片显示切片表（id/时长/位置/放置开关），否则 MIDI note 表 ----
        Label {
            text: sliceWorkspace.hasSlices
                  ? qsTr("切片表（%1 个）").arg(sliceWorkspace.slices.length)
                  : qsTr("MIDI 音符（%1 个）").arg(sliceWorkspace.midiNotes.length)
            color: Theme.textMuted
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surface
            border.width: 1
            border.color: Theme.border
            radius: Theme.radiusSm
            clip: true
            // 切片表行（model = QVariantList of maps；无切片 = 空模型，不创建 delegate）
            ListView {
                anchors.fill: parent
                anchors.margins: 2
                model: sliceWorkspace.slices
                clip: true
                visible: sliceWorkspace.hasSlices
                delegate: RowLayout {
                    required property var modelData
                    width: ListView.view.width
                    spacing: 8
                    CheckBox {
                        checked: modelData.enabled
                        onToggled: sliceWorkspace.setSliceEnabled(modelData.index, checked)
                        implicitHeight: 20
                    }
                    Label { text: modelData.index; width: 36; color: Theme.textMuted }
                    Label {
                        text: modelData.startSec.toFixed(3)
                        width: 76; color: Theme.accent2; font.family: Theme.fontMono
                    }
                    Label {
                        text: modelData.durationSec.toFixed(3) + "s"
                        width: 72; color: Theme.textMuted; font.family: Theme.fontMono
                    }
                    Label {
                        text: modelData.kind === "midi"
                              ? ("MIDI " + modelData.note)
                              : qsTr("网格")
                        width: 90; color: Theme.text
                    }
                    Item { Layout.fillWidth: true }
                }
            }
            // MIDI note 行（无切片时）
            ListView {
                anchors.fill: parent
                anchors.margins: 2
                model: sliceWorkspace.midiNotes
                clip: true
                visible: !sliceWorkspace.hasSlices
                delegate: RowLayout {
                    required property var modelData
                    width: ListView.view.width
                    spacing: 8
                    Label { text: modelData.pitch; width: 44; color: Theme.text; font.family: Theme.fontMono }
                    Label { text: modelData.channel + "ch"; width: 40; color: Theme.textMuted }
                    Label { text: "T" + modelData.track; width: 36; color: Theme.textMuted }
                    Label {
                        text: (modelData.startSec + sliceWorkspace.offsetSec).toFixed(3)
                        width: 72; color: Theme.accent2; font.family: Theme.fontMono
                    }
                    Label {
                        text: modelData.startSec.toFixed(3)
                        width: 72; color: Theme.textMuted; font.family: Theme.fontMono
                    }
                    Label {
                        text: (modelData.endSec - modelData.startSec).toFixed(3) + "s"
                        color: Theme.textMuted; font.family: Theme.fontMono
                    }
                }
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
