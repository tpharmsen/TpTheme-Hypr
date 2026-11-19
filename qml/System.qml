import QtQuick

WidgetWindow {
    id: root
    width: 300
    height: 260
    backgroundRadius: 32
    readonly property real windowScale:
        Math.max(0.01, Math.min(width / 300, height / 260))

    component MetricBar: Column {
        required property string label
        required property int value
        required property color barColor
        required property real contentScale
        property string unit: "%"
        property int maximum: 100
        property string displayText: ""
        property real animatedValue: value
        Behavior on animatedValue {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutCubic
            }
        }
        readonly property real cornerRadius: 27 * contentScale
        spacing: 7 * contentScale
        width: 64 * contentScale

        Canvas {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 64 * contentScale
            height: 150 * contentScale
            antialiasing: true
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
            onPaint: {
                if (width <= 0 || height <= 0)
                    return

                const context = getContext("2d")
                context.reset()
                context.beginPath()
                const radius = Math.min(cornerRadius, width / 2)
                context.moveTo(0, height)
                context.lineTo(0, radius)
                context.quadraticCurveTo(0, 0, radius, 0)
                context.lineTo(width - radius, 0)
                context.quadraticCurveTo(width, 0, width, radius)
                context.lineTo(width, height)
                context.closePath()
                context.fillStyle = "#18ffffff"
                context.fill()
            }

            Canvas {
                id: fill
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: Math.max(0, Math.min(parent.height,
                                              parent.height * Math.max(0, Math.min(animatedValue, maximum)) / maximum))
                antialiasing: true
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    if (width <= 0 || height <= 0)
                        return

                    const context = getContext("2d")
                    context.reset()
                    context.beginPath()
                    const radius = Math.min(cornerRadius, width / 2, height / 2)
                    context.moveTo(0, height)
                    context.lineTo(0, radius)
                    context.quadraticCurveTo(0, 0, radius, 0)
                    context.lineTo(width - radius, 0)
                    context.quadraticCurveTo(width, 0, width, radius)
                    context.lineTo(width, height)
                    context.closePath()
                    context.fillStyle = barColor
                    context.fill()
                }
            }
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: displayText.length > 0 ? displayText : value + unit
            color: textPrimary
            font.pixelSize: (displayText.length > 6 ? 10 : 14) * contentScale
            font.bold: true
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: label
            color: textSecondary
            font.pixelSize: 11 * contentScale
            font.bold: true
            font.letterSpacing: 1.1 * contentScale
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 10 * root.windowScale

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8 * root.windowScale

            MetricBar {
                label: "CPU"
                value: backend.cpu
                barColor: accent
                contentScale: root.windowScale
            }

            MetricBar {
                label: "RAM"
                value: backend.ram
                barColor: accent
                contentScale: root.windowScale
            }

            MetricBar {
                label: "GPU"
                value: backend.gpu
                barColor: accent
                contentScale: root.windowScale
            }
        }
    }
}
