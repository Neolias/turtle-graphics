import QtQuick
import QtQuick.Controls

Row {
    id: obstacleControls
    anchors.top: parent.top
    anchors.topMargin: 17.5
    anchors.leftMargin: 50
    spacing: 8

    property var canvasControl
    property var turtleControl
    property var canvas

    Button {
        text: "Generate Obstacles"
        width: 120
        height: 40
        onClicked: {
            obstacleControls.canvasControl.generate_obstacles(3, obstacleControls.turtleControl.position)
        }
    }

    Button {
        text: "Clear Obstacles"
        width: 100
        height: 40
        onClicked: {
            obstacleControls.canvasControl.clear_obstacles()
            obstacleControls.canvas.should_clear_canvas = true
        }
    }
}
