import QtQuick

WidgetWindow {
    width: 380
    height: 380
    property real phase: 0

    Timer {
        interval: 32
        running: true
        repeat: true
        onTriggered: {
            phase += 0.012
            aurora.requestPaint()
        }
    }

    Canvas {
        id: aurora
        anchors.fill: parent

        onPaint: {
            const p = getContext("2d")
            const cx = width / 2
            const cy = height / 2
            const colors = ["#6df5cf", "#5d9dff", "#9d75ff", "#e36dce"]

            p.reset()
            p.translate(cx, cy)

            const background = p.createRadialGradient(0, -25, 10, 0, 0, 240)
            background.addColorStop(0, "#263e51")
            background.addColorStop(0.48, "#151f36")
            background.addColorStop(1, "#080d1b")
            p.fillStyle = background
            p.fillRect(-190, -190, 380, 380)

            // Fine dust follows the same current as the ribbons.
            for (let i = 0; i < 150; ++i) {
                const seed = i * 13.731
                const distance = 25 + (i * 31) % 180
                const drift = phase * (0.25 + (i % 5) * 0.035)
                const x = Math.cos(seed + drift) * distance
                    + Math.sin(seed * 0.7 + phase) * 14
                const y = Math.sin(seed + drift) * distance * 0.62
                    + Math.cos(seed * 0.4 + phase * 1.4) * 12
                const depth = (i % 9) / 9
                p.globalAlpha = 0.12 + depth * 0.38
                p.fillStyle = colors[i % colors.length]
                p.beginPath()
                p.arc(x, y, 0.5 + depth * 1.5, 0, 2 * Math.PI)
                p.fill()
            }
            p.globalAlpha = 1

            // Broad translucent aurora curtains.
            for (let band = 0; band < 7; ++band) {
                const offset = (band - 3) * 24
                const hue = colors[band % colors.length]
                p.beginPath()
                for (let x = -230; x <= 230; x += 5) {
                    const y = offset
                        + Math.sin(x * 0.014 + phase * (0.8 + band * 0.06)) * 30
                        + Math.sin(x * 0.035 - phase * 0.55 + band) * 12
                    if (x === -230)
                        p.moveTo(x, y)
                    else
                        p.lineTo(x, y)
                }
                for (let x = 230; x >= -230; x -= 5) {
                    const y = offset + 13
                        + Math.sin(x * 0.014 + phase * (0.8 + band * 0.06)) * 30
                        + Math.sin(x * 0.035 - phase * 0.55 + band) * 12
                    p.lineTo(x, y)
                }
                p.closePath()
                p.fillStyle = hue
                p.globalAlpha = 0.035 + (band % 3) * 0.012
                p.fill()
            }
            p.globalAlpha = 1

            // Bright, fine strands give the field a flowing silk texture.
            for (let strand = 0; strand < 12; ++strand) {
                p.beginPath()
                for (let x = -215; x <= 215; x += 4) {
                    const y = (strand - 6) * 15
                        + Math.sin(x * 0.017 + phase * 1.15 + strand * 0.48) * 28
                        + Math.sin(x * 0.045 - phase * 0.8) * 9
                    if (x === -215)
                        p.moveTo(x, y)
                    else
                        p.lineTo(x, y)
                }
                p.strokeStyle = colors[strand % colors.length]
                p.lineWidth = strand % 4 === 0 ? 2.2 : 0.8
                p.globalAlpha = strand % 4 === 0 ? 0.35 : 0.16
                p.stroke()
            }
            p.globalAlpha = 1

            // A soft central bloom anchors the otherwise weightless field.
            const pulse = 1 + Math.sin(phase * 1.7) * 0.08
            p.save()
            p.scale(pulse, pulse)
            const bloom = p.createRadialGradient(0, 0, 2, 0, 0, 82)
            bloom.addColorStop(0, "#eaffff")
            bloom.addColorStop(0.08, "#93f7e4")
            bloom.addColorStop(0.32, "#657dff88")
            bloom.addColorStop(1, "#00000000")
            p.fillStyle = bloom
            p.beginPath()
            p.arc(0, 0, 82, 0, 2 * Math.PI)
            p.fill()
            p.restore()

            // Slow foreground motes emphasize depth.
            for (let mote = 0; mote < 9; ++mote) {
                const a = phase * (0.18 + mote * 0.025) + mote * 0.9
                const r = 65 + mote * 12
                p.fillStyle = colors[(mote + 1) % colors.length]
                p.globalAlpha = 0.45
                p.beginPath()
                p.arc(Math.cos(a) * r, Math.sin(a) * r * 0.52,
                      1.5 + (mote % 3), 0, 2 * Math.PI)
                p.fill()
            }
            p.globalAlpha = 1
        }
    }
}
