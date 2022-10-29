import QtQuick
import QtQuick.Controls
import Minesolver

Popup {
    id: newBoardDialog

    spacing: 20

    property int startingMineCount
    property int startingFieldWidth
    property int startingFieldHeight

    width: 750
    height: 200

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

        Text {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            anchors.topMargin: 10

            font.pixelSize: 20

            text: "Configure Minefield"

            id: title
        }

        Rectangle {
            color: "dark gray"
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: title.bottom
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.topMargin: 5
        }


        Row {
            height: 100
            anchors.centerIn: parent

            spacing: 20

            Column {
                anchors.verticalCenter: parent.verticalCenter

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
                anchors.verticalCenter: parent.verticalCenter

                TextField {
                    id: widthField

                    anchors.horizontalCenter: parent.horizontalCenter

                    Component.onCompleted: text = AppState.minefieldWidth
                    validator: IntValidator{ bottom: 1 }

                    onTextChanged: AppState.minefieldWidth = parseInt(text)
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Width"
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter

                TextField {
                    id: heightField

                    anchors.horizontalCenter: parent.horizontalCenter

                    Component.onCompleted: text = AppState.minefieldHeight
                    validator: IntValidator{ bottom: 1 }

                    onTextChanged: AppState.minefieldHeight = parseInt(text)
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Height"
                }
            }

            Grid {
                columns: 2
                spacing: 10

                flow: Grid.TopToBottom

                anchors.verticalCenter: parent.verticalCenter

                Button {
                    text: "Default"

                    onClicked: {
                        mineCountField.text = "99"
                        widthField.text = "30"
                        heightField.text = "16"
                    }
                }

                Button {
                    text: "Large"

                    onClicked: {
                        mineCountField.text = "500"
                        widthField.text = "50"
                        heightField.text = "50"
                    }
                }

                Button {
                    text: "Jumbo"

                    onClicked: {
                        mineCountField.text = "2000"
                        widthField.text = "100"
                        heightField.text = "100"
                    }
                }

                Button {
                    text: "Humongous"

                    onClicked: {
                        mineCountField.text = "8000"
                        widthField.text = "200"
                        heightField.text = "200"
                    }
                }
            }
        }

        Rectangle {
            color: "dark gray"
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: actionRow.top
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.bottomMargin: 5
        }

        Row {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20

            id: actionRow

            Text {
                anchors.verticalCenter: parent.verticalCenter

                visible: !AppState.fieldConfigValid

                text: "Field config is not valid (too many mines or too many states to count)"

                color: "red"
            }

            anchors.right: parent.right
            anchors.margins: 20

            spacing: 20

            Button {
                text: "Cancel"

                onClicked: {
                    // back to the previous state for rejection
                    // set the text fields so that the ui states also update
                    mineCountField.text = startingMineCount
                    widthField.text = startingFieldWidth
                    heightField.text = startingFieldHeight

                    newBoardDialog.close()
                }
            }

            Button {
                text: "New Game"

                enabled: AppState.fieldConfigValid

                onClicked: {
                    if(widthField.text * heightField.text > 2500)
                    {// large board, default to the fast mode
                        AppState.showInteractiveGameBoard = false
                    }

                    AppState.createNewMinefield()

                    newBoardDialog.close()
                }
            }
        }
    }
}
