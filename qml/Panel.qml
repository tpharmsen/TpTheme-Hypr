import QtQuick
import QtQuick.Window
import org.kde.layershell 1.0 as LayerShell

Window {
    id: panel
    width: Screen.width
    height: 58
    x: commandLinePositionSet ? commandLinePositionX : 0
    y: commandLinePositionSet ? commandLinePositionY : 0
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.WindowStaysOnBottomHint
    LayerShell.Window.scope: "tp-theme-hypr"
    LayerShell.Window.anchors: commandLinePositionSet
        ? LayerShell.Window.AnchorTop | LayerShell.Window.AnchorLeft
        : LayerShell.Window.AnchorBottom
    LayerShell.Window.margins.left: commandLinePositionSet ? commandLinePositionX : 0
    LayerShell.Window.margins.top: commandLinePositionSet ? commandLinePositionY : 0
    LayerShell.Window.layer: LayerShell.Window.LayerBottom
    LayerShell.Window.exclusionZone: -1
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
    title: "TpTheme Panel"
    property date now: new Date()
    property color ink: "#edf4ff"
    property color muted: "#91a0b8"
    property color cyan: "#72e7d2"
    property color violet: "#b38cff"
    readonly property real panelScale: Math.max(0.25, height / 58)

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: panel.now = new Date()
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 6
        radius: 18
        color: "#e90b1220"
        border.color: "#264c6381"
        border.width: 1
    }

    Item {
        width: panel.width / panel.panelScale
        height: 58
        scale: panel.panelScale

        Row {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 14

        Rectangle {
            width: 108; height: 38; radius: 12
            anchors.verticalCenter: parent.verticalCenter
            color: "#1d72e7d2"
            Text {
                anchors.centerIn: parent
                text: "TP  /  HYPR"
                color: "#08131f"
                font.bold: true; font.pixelSize: 12; font.letterSpacing: 1.1
            }
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5
            Repeater {
                model: 10
                delegate: Rectangle {
                    required property int index
                    width: 28; height: 28; radius: 9
                    color: backend.activeWorkspace === index + 1 ? panel.cyan : "#142536"
                    border.color: backend.activeWorkspace === index + 1 ? panel.cyan : "#29445d"
                    Text {
                        anchors.centerIn: parent
                        text: index + 1
                        color: backend.activeWorkspace === index + 1 ? "#09151e" : panel.muted
                        font.bold: true; font.pixelSize: 11
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: backend.switchWorkspace(index + 1)
                    }
                }
            }
            }
        }

        Rectangle {
            LayoutMirroring.enabled: false
            width: 1; height: 24; color: "#30455b"
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            width: 220
            anchors.verticalCenter: parent.verticalCenter
            text: "WAYLAND  ·  HYPRLAND"
            color: panel.muted
            font.family: "monospace"; font.pixelSize: 11
            elide: Text.ElideRight
        }

        Item { width: Math.max(0, panel.width - 780); height: 1 }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 16
            Text {
                text: "CPU " + backend.cpu + "%"
                color: panel.muted
                font.family: "monospace"; font.pixelSize: 11
            }
            Text {
                text: "RAM " + backend.ram + "%"
                color: panel.muted
                font.family: "monospace"; font.pixelSize: 11
            }
            Text {
                text: backend.batteryAvailable ? "BAT " + backend.battery + "%" : "AC"
                color: backend.batteryAvailable && backend.battery < 20 ? "#ff8e9e" : panel.muted
                font.family: "monospace"; font.pixelSize: 11
            }
            Column {
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Qt.formatTime(panel.now, "HH:mm")
                    color: panel.ink
                    font.bold: true; font.pixelSize: 16
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Qt.formatDate(panel.now, "ddd, dd MMM")
                    color: panel.muted
                    font.pixelSize: 10
                }
            }
        }
    }
}
