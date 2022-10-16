import QtQuick
import Minesolver

TableView {
    keyNavigationEnabled: false

    model: AppState.minefieldModel

    anchors.fill: parent

    delegate: Rectangle {
        color: "gray"
        border.color: "black"

        implicitWidth: 20
        implicitHeight: 20

        MouseArea {
            id: clickCatcher
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            anchors.fill: parent
        }

        Rectangle {
            color: "red"
            anchors.fill: parent

            visible: isMine
        }

        Rectangle {
            color: "yellow"
            anchors.fill: parent

            visible: guessMine
        }

        Text {
            color: "black"
            text: count
            anchors.centerIn: parent

            visible: count > 0
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
