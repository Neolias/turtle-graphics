import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    id: cliButtons
    property var cli  // Expose CLI instance
    property var stateManager  // Expose state manager
    property string fileDialogType  // Add this line to define fileDialogType

    signal resetCanvas

    width: parent.width  // Adjust the width as needed
    height: parent.height  // Match the height of the parent
    anchors.topMargin: 17.5
    anchors.rightMargin: 17.5
    anchors.top: parent.top

    Row {
        spacing: 8
        anchors.right: parent.right

        // Save button
        Button {
            text: "Save"
            width: 100
            height: 40
            onClicked: {
                saveStateDialog.visible = true;
            }
        }

        // Load button
        Button {
            text: "Load"
            width: 100
            height: 40
            onClicked: {
                cliButtons.fileDialogType = "loadState"
                loadDialog.title = "Load Turtle State"
                loadDialog.nameFilters = ["Text Files (*.txt)", "All Files (*)"]
                loadDialog.open()
            }
        }

        // Screenshot button
        Button {
            text: "Screenshot"
            width: 100
            height: 40
            onClicked: {
                stateManager.saveScreenshot();
            }
        }

        // Load Script button
        Button {
            text: "Load Script"
            width: 100
            height: 40
            onClicked: {
                cliButtons.fileDialogType = "loadScript"
                loadDialog.title = "Load Script"
                loadDialog.nameFilters = ["Text Files (*.txt)", "All Files (*)"]
                loadDialog.open()
            }
        }
    }

    // Load dialog
    FileDialog {
        id: loadDialog
        visible: false
        currentFolder: stateManager.buildFolder
        nameFilters: ["Text Files (*.txt)", "All Files (*)"]
        onAccepted: {
            const filePath = loadDialog.selectedFiles[0]
            if (cliButtons.fileDialogType === "loadState") {
                cliButtons.stateManager.loadState(filePath)
            } else if (cliButtons.fileDialogType === "loadScript") {
                cliButtons.cli.loadScript(filePath)
            }
            cliButtons.resetCanvas()
        }
    }

    // Save dialog for entering file name
    Dialog {
        id: saveStateDialog
        title: "Enter State File Name"
        modal: true
        width: 400
        height: 150
        standardButtons: Dialog.Ok | Dialog.Cancel
        visible: false
        onVisibleChanged: if (visible) {
            centerDialog();
        }
        onAccepted: {
            var fileName = fileNameInput.text.trim()
            stateManager.saveState(fileName)
        }

        ColumnLayout {
            width: parent.width

            Text {
                text: "Enter file name:"
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: fileNameInput
                placeholderText: "File name"
                Layout.alignment: Qt.AlignHCenter
                width: 0.8 * parent.width
            }
        }
    }

    function centerDialog() {
        saveStateDialog.x = (parent.width - saveStateDialog.width) / 2;
        saveStateDialog.y = (parent.height - saveStateDialog.height) / 2;
    }

    Component.onCompleted: {
        saveStateDialog.x = (parent.width - saveStateDialog.width) / 2;
        saveStateDialog.y = (parent.height - saveStateDialog.height) / 2;
    }
}
