import QtQuick
import QtQuick.Controls

WidgetWindow {
    width: 520; height: 340
    ScrollView {
        anchors.fill: parent; anchors.margins: 18
        clip: true
        TextArea {
            readOnly: true
            text: backend.journal
            color: "#d8d8d8"
            font.family: "monospace"; font.pixelSize: 12
            wrapMode: TextEdit.NoWrap
            background: Rectangle { radius: 12; color: "#0cffffff"; border.color: "#16ffffff" }
        }
    }
    Timer { interval: 5000; running: true; repeat: true; onTriggered: backend.refreshJournal() }
}
