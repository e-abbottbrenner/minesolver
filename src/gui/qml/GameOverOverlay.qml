import QtQuick
import QtQuick.Layouts
import Minesolver

ColumnLayout
{
    visible: AppState.minefieldModel.gameLost || AppState.minefieldModel.gameWon

    Text {
        text: "Game Lost!"

        color: "red"
        style: Text.Sunken
        styleColor: "white"

        font.pixelSize: 36

        visible: AppState.minefieldModel.gameLost
    }

    Text {
        text: "Game Won!"

        color: "green"
        style: Text.Raised
        styleColor: "white"

        font.pixelSize: 36

        visible: AppState.minefieldModel.gameWon
    }
}
