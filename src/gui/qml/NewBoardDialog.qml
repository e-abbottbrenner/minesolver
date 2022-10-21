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
                        anchors.horizontalCenter: parent.horizontalCenter

                        id: numMines

                        Component.onCompleted: text = AppState.mineCount
                        validator: IntValidator{ bottom: 1; top: 4999; }

                        onTextChanged: AppState.mineCount = parseInt(text)
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Number of Mines"
                    }
                }

                Column {
                    TextField {
                        anchors.horizontalCenter: parent.horizontalCenter

                        id: fieldWidth

                        Component.onCompleted: text = AppState.minefieldWidth
                        validator: IntValidator{ bottom: 10; top: 100; }

                        onTextChanged: AppState.minefieldWidth = parseInt(text)
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Width"
                    }
                }

                Column {
                    TextField {
                        anchors.horizontalCenter: parent.horizontalCenter

                        id: fieldHeightg

                        Component.onCompleted: text = AppState.minefieldHeight
                        validator: IntValidator{ bottom: 10; top: 50; }

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
                        AppState.mineCount = 99
                        AppState.minefieldWidth = 30
                        AppState.minefieldHeight = 16
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
