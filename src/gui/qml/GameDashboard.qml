import QtQuick
import QtQuick.Controls
import Minesolver

Rectangle {
    color: "black"
    border.color: "gray"

    width: 200
    height: 600

    property int row: -1
    property int column: -1

    function modelData(roleName) {
        return AppState.minefieldModel.data(AppState.minefieldModel.index(row, column), AppState.minefieldModel.roleForName(roleName))
    }

    GameOverOverlay {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.topMargin: 10
    }

    Column {

        anchors.centerIn: parent

        spacing: 20

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "New Game"

            onClicked: { AppState.createNewMinefield() }

            visible: true
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Customize Minefield"

            onClicked: newBoard.open()
        }

        Button {
            text: "Toggle show solution"

            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: { AppState.showSolution = !AppState.showSolution }
        }

        Item {

            anchors.horizontalCenter: parent.horizontalCenter

            width: 180
            height: 50

            Column {
                anchors.centerIn: parent

                spacing: 5

                visible: AppState.showSolution

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    color: "white"
                    text: AppState.minefieldModel.recalculationInProgress? "Calculating chances..." : "Calculation complete."
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    color: "white"
                    text: AppState.minefieldModel.recalculationStep
                }

                ProgressBar {
                    width: 150

                    anchors.horizontalCenter: parent.horizontalCenter

                    indeterminate: AppState.minefieldModel.recalculationInProgress && AppState.minefieldModel.maxRecalculationProgress === 0

                    from: 0
                    to: AppState.minefieldModel.maxRecalculationProgress
                    value: AppState.minefieldModel.currentRecalculationProgress
                }
            }
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 10

            spacing: 20

            Text {
                anchors.left: parent.left

                color: "white"
                text: "Legal minefield count: 2^" + AppState.minefieldModel.logLegalFieldCount

                visible: AppState.showSolution
            }

            Text {
                anchors.left: parent.left

                color: "white"
                text: "Mine Chance: " + (modelData("chanceMine") * 100).toFixed(2) + "%"

                visible: AppState.showSolution
            }

            Text {
                anchors.left: parent.left

                color: "white"

                text: "Best mine chance: " + (AppState.minefieldModel.bestMineChance * 100).toFixed(2) + "%"

                visible: AppState.showSolution
            }

            Text {
                anchors.left: parent.left

                color: "white"

                text: "Cumulative risk of loss: " + (AppState.minefieldModel.cumulativeRiskOfLoss * 100).toFixed(2) + "%"
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Reveal lowest risk cell(s)"

            onClicked: { AppState.minefieldModel.revealLowestRiskCells() }

            visible: AppState.showSolution
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Auto solve"

            onClicked: { AppState.minefieldModel.autoSolve = true }

            visible: AppState.showSolution
        }
    }

    NewBoardDialog {
        id: newBoard
    }
}
