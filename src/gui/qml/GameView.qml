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
            width: 900
            height: 600

            anchors.verticalCenter: parent.verticalCenter

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: panelPosHelper.top
                anchors.bottomMargin: 10

                spacing: 20

                GameStatusPanel {
                }

                Button {
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10

                    text: AppState.showInteractiveGameBoard? "Display Mode: Interactive" : "Display Mode: Image"

                    onClicked: AppState.showInteractiveGameBoard = !AppState.showInteractiveGameBoard
                }
            }

            Item {
                id: panelPosHelper

                anchors.centerIn: parent

                property double aspect: computeAspect()

                width: Math.min(900, 500 * aspect)
                height: Math.min(500, 900 / aspect)

                function computeAspect() {
                    return AppState.minefieldModel.columnCount(AppState.minefieldModel.nullIndex()) / AppState.minefieldModel.rowCount(AppState.minefieldModel.nullIndex())
                }

                Connections {
                    target: AppState.minefieldModel

                    function onModelReset() {
                        panelPosHelper.aspect = panelPosHelper.computeAspect()
                    }
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

                cache: false

                fillMode: Image.PreserveAspectFit

                anchors.fill: panelPosHelper

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
