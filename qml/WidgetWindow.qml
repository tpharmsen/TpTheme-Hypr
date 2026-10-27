import QtQuick
import QtQuick.Window

Window {
    id: root
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.WindowStaysOnBottomHint
    visible: true
    title: "TpTheme Hypr"
    property color accent: "#c87137"
    property color textPrimary: "#eef0f4"
    property color textSecondary: "#9da2b1"

    Rectangle {
        anchors.fill: parent
        radius: 24
        color: "#80242424"
        border.color: "#26ffffff"
        border.width: 1
    }
}
