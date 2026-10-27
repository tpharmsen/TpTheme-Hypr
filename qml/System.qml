import QtQuick

WidgetWindow {
    width: 370; height: 290
    function metricValue(index) {
        return [backend.cpu, backend.ram, backend.gpu, backend.vram][index]
    }
    Repeater {
        model: 4
        delegate: Rectangle {
            property int value: metricValue(index)
            readonly property string label: ["CPU", "RAM", "GPU", "VRAM"][index]
            x: index % 2 === 0 ? 20 : 190; y: index < 2 ? 20 : 116
            width: 160; height: 82
            radius: 10; color: "#12ffffff"
            Text { x: 9; y: 10; text: parent.label; color: textSecondary; font.bold: true; font.pixelSize: 12 }
            Text { x: 9; y: 32; width: 142; text: parent.value + "%"; horizontalAlignment: Text.AlignRight; color: textPrimary; font.bold: true; font.pixelSize: 22 }
            Rectangle { x: 9; y: 67; width: 142; height: 4; radius: 2; color: "#18ffffff" }
            Rectangle { x: 9; y: 67; width: 142 * parent.value / 100; height: 4; radius: 2; color: accent }
        }
    }
    Text { x:20; y:220; text:"TEMP     "+backend.temperature+"°C"; color:textSecondary; font.bold:true }
    Text { x:133; y:220; text:"BATTERY"; color:textSecondary; font.bold:true }
    Text { x:246; y:220; text:"UPTIME"; color:textSecondary; font.bold:true }
    Text { x:20; y:245; text:backend.batteryAvailable ? backend.battery+"%" : "N/A"; color:textPrimary; font.pixelSize:18 }
    Text { x:246; y:245; text:backend.uptime; color:textPrimary; font.pixelSize:18 }
}
