import QtQuick
import QtQuick.Window
import org.kde.layershell 1.0 as LayerShell

Window {
    id: root
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.WindowStaysOnBottomHint
    LayerShell.Window.scope: "tp-theme-hypr"
    LayerShell.Window.anchors: commandLinePositionSet
        ? LayerShell.Window.AnchorTop | LayerShell.Window.AnchorLeft
        : LayerShell.Window.AnchorBottom
    LayerShell.Window.margins.left: commandLinePositionSet ? commandLinePositionX : 0
    LayerShell.Window.margins.top: commandLinePositionSet ? commandLinePositionY : 0
    x: commandLinePositionSet ? commandLinePositionX : 0
    y: commandLinePositionSet ? commandLinePositionY : 0
    LayerShell.Window.layer: LayerShell.Window.LayerBottom
    LayerShell.Window.exclusionZone: -1
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
    visible: true
    title: "TpTheme Hypr"
    property color accent: "#c87137"
    property color textPrimary: "#eef0f4"
    property color textSecondary: "#9da2b1"
    property int backgroundRadius: 32
    property bool transparentBackground: false
    property color backgroundColor: "#80000000"

    Rectangle {
        anchors.fill: parent
        radius: backgroundRadius
        color: transparentBackground ? "transparent" : backgroundColor
        border.color: "#26ffffff"
        border.width: 0
        visible: !transparentBackground
    }
}
