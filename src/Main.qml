 import QtQuick
import QtQuick.Controls
import QtCore
import TurtleModule
import ParserModule
import CLIModule 1.0
import SaveLoadManagerModule
import CanvasModule as CanvasControl

Window {
    id: mainWindow
    minimumWidth: 900
    minimumHeight: 1000
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth
    visible: true
    title: qsTr("Turtle Graphics")

    onWidthChanged: {
        canvas.should_clear_canvas = true;
        canvas.requestPaint();
    }
    onHeightChanged: {
        canvas.should_clear_canvas = true;
        canvas.requestPaint();
    }

    TurtleControl {
        id: turtleControl
    }

    CanvasControl.Canvas {
        id: canvasControl
        width: canvas.width
        height: canvas.height - 10 // -10 so that the turtle can't go completely under the UI
        onObstacles_changed: canvas.requestPaint()
    }

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height - topControlPanel.height - outputRect.height- controlPanel.height
        anchors.top: topControlPanel.bottom
        z: 0

        // JavaScript is used here for convenience, as QML integrates it directly for UI updates,
        // allowing dynamic rendering logic to remain close to the visual layer.

        property int last_line_count: 0
        property bool should_clear_canvas: false

        Image {
            source: "resources/images/grid_background.png"
            // sourceSize.width: 301
            // sourceSize.height: 301
            width: parent.width
            height: parent.height
            horizontalAlignment: Image.AlignLeft
            verticalAlignment: Image.AlignTop
            fillMode: Image.Tile
            z: -1
        }

        onPaint: {
            const ctx = canvas.getContext("2d");

            // Checks if a canvas reset is necessary
            if (should_clear_canvas) {
                ctx.clearRect(0, 0, width, height);
                last_line_count = 0;
                should_clear_canvas = false;
            }

            // Draw the new lines since the last repaint
            for (let i = last_line_count; i < turtleControl.line_count; i++) {
                const line = turtleControl.get_line(i);
                ctx.beginPath();
                ctx.moveTo(line.start.x, line.start.y);
                ctx.lineTo(line.end.x, line.end.y);
                ctx.strokeStyle = line.color.toString();
                ctx.lineWidth = line.width;
                ctx.stroke();
            }

            // Draw the obstacles
            for (let i = 0; i < canvasControl.obstacle_count; i++) {
                const points = canvasControl.get_obstacle_points(i);
                const color = canvasControl.get_obstacle_color(i);

                if (points && points.length >= 6) {  // Need at least 3 points (6 coordinates)
                    ctx.fillStyle = color;
                    ctx.beginPath();
                    ctx.moveTo(points[0], points[1]);
                    for (let j = 2; j < points.length; j += 2) {
                        ctx.lineTo(points[j], points[j + 1]);
                    }
                    ctx.closePath();
                    ctx.fill();
                }
            }
            last_line_count = turtleControl.line_count;
        }

        Turtle {
            id: turtle
            x: turtleControl.position.x - width / 2
            y: turtleControl.position.y - height / 2
            rotation: turtleControl.rotation
        }

        Connections {
            target: turtleControl
            function onLines_changed() { canvas.requestPaint() }
        }

        Connections {
            target: turtle
            function onClicked() { turtleControl.on_clicked() }
        }
    }

    Parser {
        id: parser
    }

    CLI {
        id: cli
        onRequestQuit: Qt.quit()
    }

    // Initialize SaveLoadManager
    SaveLoadManager {
        id: stateManager
        mainWindow: mainWindow
        buildFolder: ""
    }

    Component.onCompleted: {
        // Setting values after initialization
        cli.setParser(parser);
        turtleControl.set_canvas(canvasControl);
        stateManager.setTurtleControl(turtleControl); // Set TurtleControl after initialization
        stateManager.setMainWindow(mainWindow); // Set MainWindow after initialization
        stateManager.setCLI(cli); // Set CLI after initialization
    }

    property string savePromptTitle: ""


    property int commandCount: 0

    Connections {
        target: cli
        function onOutputChanged() {
            commandCount += 1;
            if (commandCount >= 5) {
                commandCount = 0;
                cli.clearOutput()// Clear the output area
            } else {
                outputArea.text = cli.getOutput();  // Display the output
            }
        }
    }

    Connections { // Connection from Turtle to Parser to report conclusion of animation
        target: turtleControl
        function onOn_movement_completed(movement_result) {parser.animation_done()}
    }

    Connections { // Connections from Parser to Turtle
        target: parser
        function onSetspeed(speed) { turtleControl.set_speed(speed) }
        function onSetsize(size) { turtleControl.set_pen_radius(size) }
        function onSetcolor(color) { turtleControl.set_pen_color(color) }
        function onForward(distance) { turtleControl.forward(distance) }
        function onTurn(angle) { turtleControl.turn(angle) }
        function onArc(radius, angle) { turtleControl.arc(radius, angle) }

        function onSetpos(pos) { turtleControl.set_position(pos) }
        function onSetrot(rot) { turtleControl.set_rotation(rot) }

        function onUp() { turtleControl.set_pen_down(false) }
        function onDown() { turtleControl.set_pen_down(true) }
    }

    // Bottom control panel
    ControlPanel {
        id: controlPanel
        turtleControl: turtleControl
        cli: cli
        anchors.bottom: parent.bottom
        onResetCanvas: {
            canvas.should_clear_canvas = true
            canvasControl.clear_obstacles()
            canvas.requestPaint()
        }
    }

    // Top control panel
    Rectangle {
        id: topControlPanel
        width: parent.width
        height: 75
        color: "#f0f0f0"
        anchors.top: parent.top
        anchors.bottomMargin: 60
        border.color: "gray"
        border.width: 1
        z: 1
    }

    Text {
        id: infoText
        anchors.left: parent.left
        anchors.top: parent.top
        leftPadding: 6
        topPadding: 6
        z: 2
        text: {
            let infoText = "Position: " + Math.round(turtleControl.position.x) + ", "
                         + Math.round(turtleControl.position.y) + "\nRotation: " + Math.round(turtleControl.rotation)
                         + "\nPen color: " + turtleControl.pen_color
                         + "\nObstacle count: " + canvasControl.obstacle_count;

            return infoText;
        }
        font.pixelSize: 12
    }

    ObstacleControls {
        id: obstacleControls
        canvasControl: canvasControl
        turtleControl: turtleControl
        canvas: canvas
        anchors.left: infoText.right
        z: 2
    }

    CLIButtons {
        id: cliButtons
        anchors.top: parent.top
        anchors.right: parent.right
        z: 2
        cli: cli
        stateManager: stateManager
        onResetCanvas: {
            canvas.should_clear_canvas = true
        }
    }

    // Output box
    Rectangle {
        id: outputRect
        width: parent.width
        height: 100
        color: "#f0f0f0"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        border.color: "gray"
        border.width: 1

        Text {
            id: outputArea
            anchors.fill: parent
            wrapMode: Text.Wrap
            text: cli.getOutput()  // Bind text to CLI output
            color: "blue"
            font.pixelSize: 14
            anchors.margins: 10
        }
    }
}
