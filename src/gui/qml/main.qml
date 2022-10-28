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

    RowLayout {
        anchors.fill: parent

        GameView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 1200
            Layout.minimumHeight: 760

            id: view
        }

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
