import QtQuick
import TpTheme.Hypr

WidgetWindow {
    width: 480; height: 480
    SvgClock {
        id: clock
        anchors.fill: parent
        anchors.margins: 20
    }
    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: clock.time = new Date()
    }
}
