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

                ProgressBar {
                    width: 150

                    anchors.horizontalCenter: parent.horizontalCenter

                    indeterminate: AppState.minefieldModel.recalculationInProgress

                    from: 0
                    to: 1
                    value: AppState.minefieldModel.recalculationInProgress? 0 : 1
                }
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter

            color: "white"
            text: row + ", " + column + "\n Mine Chance: " + percentMine + "%"

            visible: row >= 0 && column >= 0
        }
    }
}
