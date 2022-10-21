import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion

ApplicationWindow {
    width: 1200
    height: 700
    visible: true

    color: "light grey"

    title: qsTr("MineSolver")

    StackLayout
    {
        anchors.centerIn: parent

        id: itemStack

        GameView {
            id: view
            Layout.alignment: Qt.AlignCenter

            Layout.minimumHeight: 700
            Layout.minimumWidth: 1200
        }
    }
}
