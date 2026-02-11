import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

Window {
    id: rootWindow
    width: 1000
    height: 700
    visible: true
    title: "Bosch CCS 1000 D Simulator (Qt 6)"
    color: "#1e1e1e"

    // Property to bind the wire color
    property var activeList: boschServer.activeSeats

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // --- LEFT PANEL: ROOM VIEW ---
        Rectangle {
            id: leftPanel
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#252525"
            clip: true

            // 1. The Central Control Unit
            Rectangle {
                id: controlUnit
                width: 220
                height: 100
                color: "#111"
                border.color: "#444"
                border.width: 2
                radius: 6
                // Anchor to bottom center of this panel
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                z: 2

                Column {
                    anchors.centerIn: parent
                    spacing: 5
                    Text { text: "BOSCH Control Unit"; color: "white"; font.bold: true; font.pixelSize: 16 }
                    Text { text: "Listening on Port 8080"; color: "#888"; font.pixelSize: 12 }
                    Row {
                        spacing: 5
                        Rectangle { width: 10; height: 10; radius: 5; color: "#00ff00" }
                        Text { text: "System Online"; color: "#00ff00"; font.pixelSize: 10 }
                    }
                }
            }

            // 2. The Microphones (Grid)
            Grid {
                id: micGrid
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -50
                columns: 5
                spacing: 40
                z: 2

                Repeater {
                    id: micRepeater
                    model: 10
                    delegate: MicComponent {
                        seatId: index + 1
                        // Check if seatId is in the C++ list
                        isActive: activeList.indexOf(seatId) !== -1

                        isChairman: (index === 0)

                        onClicked: (id) => {
                            boschServer.toggleMic(id)
                        }

                        onPriorityClicked: (id) => {
                            boschServer.pressPriority(id)
                        }
                    }
                }
            }

            // 3. The Wires (Drawing lines behind everything)
            Shape {
                anchors.fill: parent
                z: 1 // Behind mics, above bg

                // Wire from Control Unit to First Mic
                ShapePath {
                    strokeWidth: 2
                    strokeColor: "#555"
                    fillColor: "transparent"

                    // Explicitly using IDs instead of parent
                    startX: controlUnit.x + controlUnit.width/2
                    startY: controlUnit.y

                    // Draw line up to the mic grid area
                    PathLine { x: leftPanel.width/2; y: micGrid.y + micGrid.height + 20 }
                }
            }

            Text {
                text: "Click a microphone to simulate a physical button press"
                color: "#666"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 20
            }
        }

        // --- RIGHT PANEL: SERVER LOGS ---
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: "#151515"
            border.color: "#333"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                Text {
                    text: "Server Activity Log"
                    color: "#eee"
                    font.bold: true
                    font.pixelSize: 14
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#333"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    TextArea {
                        text: boschServer.serverLog
                        color: "#00ff00"
                        font.family: "Courier New"
                        font.pixelSize: 11
                        readOnly: true
                        background: null
                        wrapMode: Text.Wrap

                        // Auto-scroll logic
                        onTextChanged: {
                            if(length > 0) cursorPosition = length - 1
                        }
                    }
                }
            }
        }
    }
}
