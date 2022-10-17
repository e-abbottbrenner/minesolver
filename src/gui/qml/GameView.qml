import QtQuick
import QtQuick.Layouts

Row
{
    // just using the row layout to get the behavior of a preferred size expanding up to a point
    RowLayout
    {
        id: boardLayout

        GameBoard {
            id: gameBoard

            Layout.maximumWidth: 900
            Layout.maximumHeight: 600
        }
    }

    CellDetailsDisplay {
        anchors.top: boardLayout.top
        anchors.bottom: boardLayout.bottom

        row: gameBoard.hoveredRow
        column: gameBoard.hoveredColumn
        percentMine: gameBoard.hoveredPercentMine
    }
}
