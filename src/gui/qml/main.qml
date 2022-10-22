import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import Minesolver

ApplicationWindow {
    width: 1200
    height: 760
    visible: true

    color: "light grey"

    title: qsTr("MineSolver")

    GameView {
        anchors.centerIn: parent

        id: view
        Layout.alignment: Qt.AlignCenter
    }

    Shortcut {
        sequences: [StandardKey.Quit, "Ctrl+Q"]

        onActivated: Qt.quit()
    }

    Shortcut {
        sequence: StandardKey.New

        onActivated: AppState.createNewMinefield()
    }
}
