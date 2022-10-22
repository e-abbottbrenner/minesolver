import QtQuick
import QtQuick.Layouts
import Minesolver

Rectangle
{
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
