import QtQuick
import QtQuick.Controls

WidgetWindow {
    id: root
    width: 520; height: 340
    readonly property real windowScale:
        Math.max(0.25, Math.min(width / 520, height / 340))

    Item {
        width: 520
        height: 340
        x: (root.width - width * root.windowScale) / 2
        y: (root.height - height * root.windowScale) / 2
        scale: root.windowScale

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
}
