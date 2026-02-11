import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property int seatId: 0
    property bool isActive: false
    property bool isChairman: false // <--- New Property

    // Make the component taller if it's a Chairman unit to fit 2 buttons
    width: 80
    height: isChairman ? 150 : 120

    signal clicked(int id)
    signal priorityClicked(int id) // <--- New Signal

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        // --- MIC HEAD (Same as before) ---
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 30; height: 30; radius: 15
            color: root.isActive ? "#ff3333" : "#444"
            border.color: root.isActive ? "#ffaaaa" : "#222"
            border.width: 2

            // ... (keep your glow animation here) ...
        }

        // --- MIC STAND (Same as before) ---
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 4; height: 40; color: "#666"
        }

        // --- BASE UNIT (Modified) ---
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 70
            // Base is taller for Chairman
            height: root.isChairman ? 70 : 40
            color: "#333"
            radius: 4
            border.color: "#555"

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                // BUTTON 1: Speak (Standard)
                Button {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 25
                    background: Rectangle { color: parent.down ? "#222" : "#444"; radius: 2 }
                    contentItem: Text {
                        text: "TALK"; color: "white"; font.pixelSize: 9; horizontalAlignment: Text.AlignHCenter
                    }
                    onClicked: root.clicked(root.seatId)
                }

                // BUTTON 2: Priority (Only visible for Chairman)
                Button {
                    visible: root.isChairman // <--- MAGIC
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 25

                    background: Rectangle {
                        color: parent.down ? "#500" : "#900" // Dark Red Button
                        radius: 2
                        border.color: "red"
                    }
                    contentItem: Text {
                        text: "PRIO"; color: "white"; font.bold: true; font.pixelSize: 9; horizontalAlignment: Text.AlignHCenter
                    }

                    onClicked: root.priorityClicked(root.seatId)
                }
            }
        }

        // --- CONNECTOR (Same as before) ---
        Item {
            id: connector
            Layout.alignment: Qt.AlignHCenter
            width: 10; height: 10
            Rectangle { anchors.centerIn: parent; width: 6; height: 6; radius: 3; color: "black" }
        }
    }
}
