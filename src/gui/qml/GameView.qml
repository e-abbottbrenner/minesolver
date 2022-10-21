import QtQuick
import QtQuick.Layouts

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

        GameBoard {
            id: gameBoard

            anchors.verticalCenter: parent.verticalCenter
        }

        GameDashboard {
            row: gameBoard.hoveredRow
            column: gameBoard.hoveredColumn

            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
