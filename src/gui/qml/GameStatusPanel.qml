import QtQuick
import QtQuick.Controls
import Minesolver

Row {
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10

    Text {
        text: "Flags Remaining: " + AppState.minefieldModel.flagsRemaining

        font.pixelSize: 20
        color: "white"

        anchors.bottom: parent.bottom
    }

    spacing: 20

    Text {
        id: timePlayedText

        property int timePlayed

        Timer {
            running: !AppState.minefieldModel.gameWon && !AppState.minefieldModel.gameLost
            interval: 1000
            repeat: true

            onTriggered: timePlayedText.timePlayed++
        }

        text: "Time Played: " + timePlayed + " seconds"

        font.pixelSize: 20
        color: "white"

        anchors.bottom: parent.bottom

        Connections {
            target: AppState.minefieldModel
            function onNewMinefieldStarted() { timePlayedText.timePlayed = 0 }
        }
    }
}
