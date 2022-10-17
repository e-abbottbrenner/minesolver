import QtQuick
import Minesolver

TableView {
    keyNavigationEnabled: false
    pointerNavigationEnabled: false
    selectionBehavior: TableView.SelectionDisabled

    model: AppState.minefieldModel

    delegate: Rectangle {
        color: "gray"
        border.color: "black"

        implicitWidth: 30
        implicitHeight: 30

        MouseArea {
            id: clickCatcher
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            anchors.fill: parent
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
