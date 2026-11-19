import QtQuick

WidgetWindow {
    id: root
    width: 380
    height: 380
    backgroundRadius: 24
    transparentBackground: false
    backgroundColor: "#80000000"

    readonly property real windowScale:
        Math.max(0.25, Math.min(width / 380, height / 380))

    property real phase: 0

    Item {
        width: 380
        height: 380
        x: (root.width - width * root.windowScale) / 2
        y: (root.height - height * root.windowScale) / 2
        scale: root.windowScale

        Timer {
            interval: 50
            running: true
            repeat: true
            onTriggered: {
                phase = (phase + 0.006) % (4 * Math.PI)
                planetary.requestPaint()
            }
        }

        Canvas {
            id: planetary
            anchors.fill: parent

            onPaint: {
                const p = getContext("2d")
                p.reset()
                p.translate(190, 190)

                // 140 warm, low-opacity stars in the KDE 380-pixel scene.
                for (let i = 0; i < 140; ++i) {
                    const seed = i * 47.73
                    const x = (Math.sin(seed * 1.13) * 185)
                    const y = (Math.cos(seed * 0.71) * 185)
                    const starSize = 0.65 + (i % 4) * 0.35
                    p.globalAlpha = (32 + (i * 37) % 60) / 255
                    p.fillStyle = "#c87137"
                    p.beginPath()
                    p.arc(x, y, starSize, 0, 2 * Math.PI)
                    p.fill()
                }
                p.globalAlpha = 1

                // Exact KDE cache orbit ellipses.
                const tracks = [
                    [88, 0.42, -24],
                    [124, 0.55, 38],
                    [158, 0.34, 67],
                    [178, 0.48, -52]
                ]
                p.strokeStyle = "#c8713726"
                p.lineWidth = 1
                for (let i = 0; i < tracks.length; ++i) {
                    p.save()
                    p.rotate(tracks[i][2] * Math.PI / 180)
                    p.beginPath()
                    p.ellipse(0, 0, tracks[i][0], tracks[i][0] * tracks[i][1],
                              0, 0, 2 * Math.PI)
                    p.stroke()
                    p.restore()
                }

                // Planets follow the KDE local-point and orbit-transform rules.
                const orbits = [
                    [88, 0.42, -24, 1.0, 7, 0.2],
                    [124, 0.55, 38, -1.0, 8, 2.0],
                    [158, 0.34, 67, 0.5, 13, 4.1],
                    [178, 0.48, -52, -0.5, 8, 5.4]
                ]

                for (let i = 0; i < orbits.length; ++i) {
                    const orbit = orbits[i]
                    const angle = phase * orbit[3] + orbit[5]
                    const lx = Math.cos(angle) * orbit[0]
                    const ly = Math.sin(angle) * orbit[0] * orbit[1]

                    p.save()
                    p.rotate(orbit[2] * Math.PI / 180)
                    p.translate(lx, ly)

                    p.globalAlpha = 1

                    p.fillStyle = "#c87137"
                    p.globalAlpha = 0.16
                    p.beginPath()
                    p.arc(0, 0, orbit[4] + 5, 0, 2 * Math.PI)
                    p.fill()
                    p.globalAlpha = 1
                    p.beginPath()
                    p.arc(0, 0, orbit[4], 0, 2 * Math.PI)
                    p.fill()

                    p.restore()

                    // KDE moons are transformed with the orbit angle,
                    // independently of the planet's ring rotation.
                    p.save()
                    p.rotate(orbit[2] * Math.PI / 180)
                    p.fillStyle = "#c87137"
                    p.globalAlpha = 0.67
                    const moonAngle = phase * (3 + i) + i
                    p.beginPath()
                    p.arc(
                        Math.cos(angle) * orbit[0]
                            + Math.cos(moonAngle) * (orbit[4] + 9),
                        Math.sin(angle) * orbit[0] * orbit[1]
                            + Math.sin(moonAngle) * (orbit[4] + 9) * 0.62,
                        2 + i * 0.35,
                        0,
                        2 * Math.PI)
                    p.fill()
                    if (i === 2) {
                        const extra = -phase * 5 + 1.7
                        p.beginPath()
                        p.arc(
                            Math.cos(angle) * orbit[0]
                                + Math.cos(extra) * (orbit[4] + 15),
                            Math.sin(angle) * orbit[0] * orbit[1]
                                + Math.sin(extra) * (orbit[4] + 15) * 0.62,
                            2.5,
                            0,
                            2 * Math.PI)
                        p.fill()
                    }
                    p.restore()
                }

                // Ringed planet: render the rings from the planet's exact
                // final center, not from the local orbit coordinate system.
                const ringOrbit = orbits[1]
                const ringAngle = phase * ringOrbit[3] + ringOrbit[5]
                const ringLocalX = Math.cos(ringAngle) * ringOrbit[0]
                const ringLocalY = Math.sin(ringAngle) * ringOrbit[0] * ringOrbit[1]
                const ringRotation = ringOrbit[2] * Math.PI / 180
                const ringX = ringLocalX * Math.cos(ringRotation)
                            - ringLocalY * Math.sin(ringRotation)
                const ringY = ringLocalX * Math.sin(ringRotation)
                            + ringLocalY * Math.cos(ringRotation)

                p.save()
                p.translate(ringX, ringY)
                p.rotate(ringAngle)
                p.strokeStyle = "#c87137"
                p.globalAlpha = 0.49
                p.lineWidth = 2
                p.beginPath()
                p.ellipse(0, 0, ringOrbit[4] + 15, 4.2,
                          0, 0, 2 * Math.PI)
                p.stroke()
                p.globalAlpha = 0.41
                p.lineWidth = 1
                p.beginPath()
                p.ellipse(0, 0, ringOrbit[4] + 20, 5.8,
                          0, 0, 2 * Math.PI)
                p.stroke()
                p.restore()
                p.globalAlpha = 1

                // Central pulsing planet.
                const pulse = 1 + (Math.sin(phase * 2) + 1) * 0.012
                p.save()
                p.scale(pulse, pulse)
                p.strokeStyle = "#c87137"
                p.lineWidth = 2.4
                p.fillStyle = "#c87137"
                p.beginPath()
                p.arc(0, 0, 20, 0, 2 * Math.PI)
                p.fill()
                p.stroke()
                p.globalAlpha = 0.59
                p.lineWidth = 1.2
                p.beginPath()
                p.arc(0, 0, 24, 0, 2 * Math.PI)
                p.stroke()
                p.restore()
                p.globalAlpha = 1
            }
        }

        // The KDE original places the signature at (32, 104), 150 x 72.
        Image {
            x: 32
            y: 104
            width: 150
            height: 72
            source: "qrc:/TpTheme/Hypr/assets/signatureTP.svg"
            opacity: 0.72
            fillMode: Image.PreserveAspectFit
        }
    }
}
