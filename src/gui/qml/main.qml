import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion

Window {
    width: 1200
    height: 800
    visible: true

    color: "light grey"

    title: qsTr("MineSolver")

    StackLayout
    {
        anchors.centerIn: parent

        id: itemStack

        GameView {
            id: view
        }
    }
}
