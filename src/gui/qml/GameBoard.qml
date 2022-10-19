import QtQuick
import Minesolver

TableView {
    id: minefieldTable

    keyNavigationEnabled: false
    pointerNavigationEnabled: false
    selectionBehavior: TableView.SelectionDisabled

    model: AppState.minefieldModel

    property int hoveredRow: -1
    property int hoveredColumn: -1
    property int hoveredPercentMine: -1
    property int hoveredChoiceColumnCount: -1
    property int hoveredSolverPathIndex: -1

    readonly property int cellSize: 30

    implicitWidth: columns * cellSize
    implicitHeight: rows * cellSize

    delegate: Rectangle {
        color: "gray"
        border.color: "black"

        implicitWidth: cellSize
        implicitHeight: cellSize

        MouseArea {
            id: clickCatcher
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            anchors.fill: parent

            onEntered: {
                minefieldTable.hoveredRow = row
                minefieldTable.hoveredColumn = column
                minefieldTable.hoveredPercentMine = 100 * chanceMine
                minefieldTable.hoveredChoiceColumnCount = choiceColumnCount
                minefieldTable.hoveredSolverPathIndex = solverPathIndex
            }
        }

        Rectangle {
            border.color: "blue"
            border.width: 3
            color: "transparent"
            anchors.fill: parent

            visible: hoveredRow === row && hoveredColumn === column
        }

        Rectangle {
            color: "red"
            anchors.fill: parent

            visible: isMine
            anchors.margins: 5
        }

        Rectangle {
            color: "yellow"
            anchors.fill: parent

            visible: guessMine
            anchors.margins: 5
        }

        Rectangle {
            color: Qt.rgba(1, 0.5, 0, chanceMine)
            anchors.fill: parent

            visible: true
            anchors.margins: 10
        }

        Rectangle {
            color: "light gray"

            Text {
                color: "black"
                text: count
                anchors.centerIn: parent
                visible: count > 0
            }

            anchors.fill: parent
            anchors.margins: 2

            visible: count >= 0
        }

        Connections {
            target: clickCatcher

            function onClicked(mouseEvent) {
                if(mouseEvent.button === Qt.LeftButton)
                {
                    AppState.minefieldModel.reveal(row, column)
                }
                else
                {
                    AppState.minefieldModel.toggleGuessMine(row, column)
                }
            }

            function onDoubleClicked(mouseEvent) {
                AppState.minefieldModel.revealAdjacent(row, column)
            }
        }
    }
}
