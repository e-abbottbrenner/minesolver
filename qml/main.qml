import QtQuick

Window {
    width: 800
    height: 600
    visible: true
    title: qsTr("MineSolver")

    GameView {
        anchors.fill: parent
    }
}
