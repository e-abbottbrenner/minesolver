import QtQuick
import QtQuick.Controls
import Minesolver

Rectangle {
    color: "black"

    implicitWidth: 200
    implicitHeight: 300

    property int row: -1
    property int column: -1
    property int percentMine: -1
    property int choiceColumnCount: -1
    property int solverPathIndex: -1

    Column{
        anchors.top: parent.top
        anchors.topMargin: 30

        spacing: 20

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
                text: "Cell Info:\nPosition: " + row + ", " + column + "\nMine Chance: " + percentMine + "%\nLegal minefield count: 2^" + AppState.minefieldModel.logLegalFieldCount

                visible: row >= 0 && column >= 0
            }

            Text {
                anchors.left: parent.left

                color: "white"

                text: "Solver metrics:\nChoice columns: " + choiceColumnCount + "\nPath Index: " + solverPathIndex

                visible: row >= 0 && column >= 0
            }
        }
    }
}
