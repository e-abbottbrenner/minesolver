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

    property int cellSize: Math.min(900 / columns, 600 / rows)

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
            }
        }

        Rectangle {
            border.color: "blue"
            border.width: 3
            color: "transparent"
            anchors.fill: parent

            visible: hoveredRow === row && hoveredColumn === column
        }

        Image {
            source: "qrc:/icons/exploded.png"
            anchors.fill: parent

            fillMode: Image.PreserveAspectFit

            visible: isMine
            anchors.margins: 1
        }

        Rectangle {
            color: "yellow"
            anchors.fill: parent

            visible: guessMine
            anchors.margins: 1
        }

        Image {
            source: "qrc:/icons/mine.png"

            opacity: chanceMine * 0.95 + 0.05

            fillMode: Image.PreserveAspectFit

            anchors.fill: parent

            visible: chanceMine > 0 && AppState.showSolution && !isMine

            anchors.margins: 3
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
