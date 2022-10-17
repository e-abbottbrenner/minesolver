import QtQuick
import Minesolver

Rectangle {
    color: "black"

    implicitWidth: 200
    implicitHeight: 300

    property int row: -1
    property int column: -1
    property int percentMine: -1

    Text {
        color: "white"
        text: parent.row + ", " + parent.column + "\n Mine Chance: " + parent.percentMine + "%"

        visible: row >= 0 && column >= 0

        anchors.centerIn: parent
    }
}
