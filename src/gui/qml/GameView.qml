import QtQuick
import QtQuick.Layouts
import Minesolver

Rectangle
{
    color: "black"

    border.color: "gray"
    border.width: 20

    width: 1200
    height: 700

    Row
    {
        anchors.centerIn: parent

        spacing: 20

        Item {
            implicitWidth: gameBoard.implicitWidth
            implicitHeight: gameBoard.implicitHeight

            anchors.verticalCenter: parent.verticalCenter

            GameStatusPanel {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: gameBoard.top
                anchors.bottomMargin: 10
            }

            GameBoard {
                id: gameBoard

                anchors.centerIn: parent
            }

            GameOverOverlay {
                anchors.centerIn: parent
            }
        }

        GameDashboard {
            row: gameBoard.hoveredRow
            column: gameBoard.hoveredColumn

            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
