import QtQuick
import QtQuick.Controls
import Minesolver

Popup {
    id: newBoardDialog

    spacing: 20

    property int startingMineCount
    property int startingFieldWidth
    property int startingFieldHeight

    width: 600
    height: 150

    modal: true
    anchors.centerIn: Overlay.overlay

    onAboutToShow: {
        // store previous state in case of rejection
        startingMineCount = AppState.mineCount
        startingFieldWidth = AppState.minefieldWidth
        startingFieldHeight = AppState.minefieldHeight
    }

    Rectangle {
        color: "light grey"

        anchors.fill: parent

        Column {
            anchors.fill: parent

            spacing: 20

            Text {
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 20

                text: "Configure Minefield"
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 20

                Column {
                    TextField {
                        id: mineCountField

                        anchors.horizontalCenter: parent.horizontalCenter

                        Component.onCompleted: text = AppState.mineCount
                        validator: IntValidator{ bottom: 1; top: 10000; }

                        onTextChanged: AppState.mineCount = parseInt(text)
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Number of Mines"
                    }
                }

                Column {
                    TextField {
                        id: widthField

                        anchors.horizontalCenter: parent.horizontalCenter

                        Component.onCompleted: text = AppState.minefieldWidth
                        validator: IntValidator{ bottom: 10; top: 200; }

                        onTextChanged: AppState.minefieldWidth = parseInt(text)
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Width"
                    }
                }

                Column {
                    TextField {
                        id: heightField

                        anchors.horizontalCenter: parent.horizontalCenter

                        Component.onCompleted: text = AppState.minefieldHeight
                        validator: IntValidator{ bottom: 10; top: 100; }

                        onTextChanged: AppState.minefieldHeight = parseInt(text)
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Height"
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter

                    text: "Set Default"

                    onClicked: {
                        mineCountField.text = "99"
                        widthField.text = "30"
                        heightField.text = "16"
                    }
                }
            }

            Row {
                Text {
                    anchors.verticalCenter: parent.verticalCenter

                    visible: !AppState.fieldConfigValid

                    text: "Field config is not valid"

                    color: "red"
                }

                anchors.right: parent.right
                anchors.margins: 20

                spacing: 20

                Button {
                    text: "Cancel"

                    onClicked: {
                        // back to the previous state for rejection
                        AppState.mineCount = startingMineCount
                        AppState.minefieldWidth = startingFieldWidth
                        AppState.minefieldHeight = startingFieldHeight

                        newBoardDialog.close()
                    }
                }

                Button {
                    text: "New Game"

                    enabled: AppState.fieldConfigValid

                    onClicked: {
                        AppState.createNewMinefield()

                        newBoardDialog.close()
                    }
                }
            }
        }
    }
}
