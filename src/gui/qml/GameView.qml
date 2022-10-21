import QtQuick
import QtQuick.Layouts
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
            anchors.verticalCenter: parent.verticalCenter

            implicitWidth: gameBoard.implicitWidth
            implicitHeight: gameBoard.implicitHeight

            GameBoard {
                id: gameBoard
            }

            Rectangle
            {
                anchors.centerIn: parent
                visible: AppState.minefieldModel.gameLost || AppState.minefieldModel.gameWon

                color: "black"

                radius: 70

                width: gameOverLayout.implicitWidth + 100
                height: gameOverLayout.implicitHeight + 10

                ColumnLayout
                {
                    id: gameOverLayout

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top

                    Text {
                        text: "Game Lost!"

                        color: "red"
                        style: Text.Sunken
                        styleColor: "white"

                        font.pixelSize: 72

                        visible: AppState.minefieldModel.gameLost
                    }

                    Text {
                        text: "Game Won!"

                        color: "green"
                        style: Text.Raised
                        styleColor: "white"

                        font.pixelSize: 72

                        visible: AppState.minefieldModel.gameWon
                    }
                }
            }
        }

        GameDashboard {
            row: gameBoard.hoveredRow
            column: gameBoard.hoveredColumn

            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
