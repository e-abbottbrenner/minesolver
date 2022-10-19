import QtQuick
import QtQuick.Controls
import Minesolver

Rectangle {
    color: "black"

    implicitWidth: 200
    implicitHeight: 300

    property int row: -1
    property int column: -1

    function modelData(roleName) {
        return AppState.minefieldModel.data(AppState.minefieldModel.index(row, column), AppState.minefieldModel.roleForName(roleName))
    }

    Column {
        anchors.top: parent.top
        anchors.topMargin: 30

        spacing: 20

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
            }

            Text {
                anchors.left: parent.left

                color: "white"
                text: "Cell Info:\nPosition: " + row + ", " + column +
                      "\nMine Chance: " + (modelData("chanceMine") * 100).toFixed(2) + "%"

                visible: AppState.showSolution
            }

//            Text {
//                anchors.left: parent.left

//                color: "white"

//                text: "Solver metrics:\nChoice columns: "
//                      + modelData("choiceColumnCount") + "\nPath Index: "
//                      + modelData("solverPathIndex")
//            }

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

            text: "Reveal optimal cell"

            onClicked: { AppState.minefieldModel.revealOptimalCell() }

            visible: AppState.showSolution
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Auto solve"

            onClicked: { AppState.minefieldModel.autoSolve = true }

            visible: AppState.showSolution
        }
    }
}
