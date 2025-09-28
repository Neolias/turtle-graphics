import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Rectangle {
    id: controlPanel
    width: parent.width
    height: 50
    color: "#f0f0f0"

    property var turtleControl
    property var cli
    property var canvasControl

    signal resetCanvas

    Row {
        anchors.fill: parent
        spacing: 10
        padding: 10

        // Text field for command inputs
        TextField {
            id: commandLine
            width: parent.width - 270
            height: 40
            anchors.top: parent.top
            anchors.leftMargin: 5
            anchors.topMargin: 5
            placeholderText: "Enter command (e.g., forward(100), turn(90))"
            onAccepted: {
                controlPanel.cli.processCommand(commandLine.text)
                commandLine.clear()
            }
        }


        // Pen width control
        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                text: "Pen width: " + controlPanel.turtleControl.pen_radius
            }

            Row {
                spacing: 5
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    height: 25
                    width: 25
                    text: "+"
                    onClicked: controlPanel.turtleControl.set_pen_radius(controlPanel.turtleControl.pen_radius + 1)
                }

                Button {
                    height: 25
                    width: 25
                    text: "-"
                    onClicked: controlPanel.turtleControl.set_pen_radius(controlPanel.turtleControl.pen_radius - 1)
                }
            }
        }

        // Pen color picker
        Button {
            height: 40
            width: 80
            anchors.top: parent.top
            anchors.rightMargin: 5
            anchors.topMargin: 5
            text: "Change Color"
            onClicked: colorPicker.open()
        }

        ColorDialog {
            id: colorPicker
            onAccepted: {
                controlPanel.turtleControl.set_pen_color(colorPicker.selectedColor);
            }
        }

        Button {
            height: 40
            width: 80
            anchors.top: parent.top
            anchors.leftMargin: 5
            anchors.topMargin: 5
            text: "Reset Canvas"
            onClicked: {
                controlPanel.turtleControl.reset_state()
                controlPanel.resetCanvas()
            }
        }

    }
}
