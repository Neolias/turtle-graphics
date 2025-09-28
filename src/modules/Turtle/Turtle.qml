import QtQuick 2.15

Item {
    id: turtle
    width: 25
    height: 25

    signal clicked()

    Image {
        id: image
        source: "cursor_turtle.png"
        width: turtle.width
        height: turtle.height
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            turtle.clicked();
        }
    }
}
