import QtQuick

WidgetWindow {
    id: root
    width: 560; height: 50
    property real scrollOffset: width
    property real scrollSpeed: 8
    readonly property real windowScale:
        Math.max(0.25, height / 50)

    Item {
        id: viewport
        anchors.fill: parent
        anchors.margins: 8
        clip: true

        TextMetrics {
            id: textMetrics
            font.family: "monospace"
            font.bold: true
            font.pixelSize: 15 * root.windowScale
            text: "  " + backend.specs + "  "
        }

        readonly property real segmentWidth:
            textMetrics.width + (24 + 26 + 26) * root.windowScale

        Row {
            x: scrollOffset
            anchors.verticalCenter: parent.verticalCenter
            spacing: 26 * root.windowScale

            Repeater {
                model: Math.max(2, Math.ceil(viewport.width / viewport.segmentWidth) + 2)
                delegate: Row {
                    required property int index
                    spacing: 26 * root.windowScale

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: textMetrics.text
                        color: accent
                        font: textMetrics.font
                    }

                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24 * root.windowScale
                        height: 24 * root.windowScale
                        source: "qrc:/TpTheme/Hypr/assets/archlinux-logo.svg"
                        sourceSize.width: 24 * root.windowScale
                        sourceSize.height: 24 * root.windowScale
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
        }

        Timer {
            interval: 200
            running: root.visible && viewport.visible
            repeat: true
            onTriggered: {
                scrollOffset -= root.windowScale * root.scrollSpeed
                if (scrollOffset <= -viewport.segmentWidth)
                    scrollOffset += viewport.segmentWidth
            }
        }
    }
}
