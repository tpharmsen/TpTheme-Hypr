import QtQuick

WidgetWindow {
    width: 560; height: 50
    Text {
        id: marquee
        anchors.centerIn: parent
        text: backend.specs
        color: accent
        font.family: "monospace"; font.bold: true; font.pixelSize: 13
    }
}
