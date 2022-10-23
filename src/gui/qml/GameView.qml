import QtQuick
import QtQuick.Controls
import Minesolver

Rectangle
{
    color: "black"

    border.color: "gray"
    border.width: 20

    width: 1200
    height: 700

    Row
    {
        anchors.centerIn: parent

        spacing: 20

        Item {
            implicitWidth: gameBoard.implicitWidth
            implicitHeight: gameBoard.implicitHeight

            anchors.verticalCenter: parent.verticalCenter

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: gameBoard.top
                anchors.bottomMargin: 10

                GameStatusPanel {
                }

                Button {
                    anchors.bottom: parent.bottom

                    text: AppState.showInteractiveGameBoard? "Display Mode: Interactive" : "Display Mode: Image"

                    onClicked: AppState.showInteractiveGameBoard = !AppState.showInteractiveGameBoard
                }
            }

            GameBoard {
                id: gameBoard

                anchors.centerIn: parent

                visible: AppState.showInteractiveGameBoard
            }

            Image {
                // all ids return the same image
                property string previousSource: "image://minefields/1"
                property string nextSource: "image://minefields/2"

                id: gameImage

                width: 900
                height: 600

                cache: false

                fillMode: Image.PreserveAspectFit

                anchors.centerIn: parent

                visible: !AppState.showInteractiveGameBoard

                function reloadSource() {
                    // just cycle through source names so it reloads
                    gameImage.source = gameImage.nextSource
                    gameImage.nextSource = gameImage.previousSource
                    gameImage.previousSource = gameImage.source
                }

                Connections {
                    target: AppState.minefieldModel

                    function onDataChanged() {
                        gameImage.reloadSource()
                    }

                    function onModelReset() {
                        gameImage.reloadSource()
                    }
                }
            }

            GameOverOverlay {
                anchors.centerIn: parent
            }
        }

        GameDashboard {
            row: gameBoard.hoveredRow
            column: gameBoard.hoveredColumn

            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
