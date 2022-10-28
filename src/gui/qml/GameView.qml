import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Minesolver

Rectangle
{
    color: "black"

    border.color: "gray"
    border.width: 20

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 40

        spacing: 20

        ColumnLayout {
            Row {
                id: topRow

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: 10

                spacing: 20

                GameStatusPanel {
                    id: statusPanel
                }

                Button {
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10

                    text: AppState.showInteractiveGameBoard? "Display Mode: Interactive" : "Display Mode: Image"

                    onClicked: AppState.showInteractiveGameBoard = !AppState.showInteractiveGameBoard
                }
            }

            GameBoard {
                id: gameBoard

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.minimumWidth: 900
                Layout.minimumHeight: 500

                Layout.maximumHeight: Number.POSITIVE_INFINITY
                Layout.maximumWidth: Number.POSITIVE_INFINITY

                Layout.alignment: Qt.AlignCenter

                visible: AppState.showInteractiveGameBoard
            }

            Image {
                // all ids return the same image
                property string previousSource: "image://minefields/1"
                property string nextSource: "image://minefields/2"

                id: gameImage

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.minimumWidth: 900
                Layout.minimumHeight: 500

                Layout.maximumHeight: Number.POSITIVE_INFINITY
                Layout.maximumWidth: Number.POSITIVE_INFINITY

                Layout.alignment: Qt.AlignCenter

                cache: false

                fillMode: Image.PreserveAspectFit

                visible: !AppState.showInteractiveGameBoard

                function reloadSource() {
                    // just cycle through source names so it reloads
                    gameImage.source = gameImage.nextSource
                    gameImage.nextSource = gameImage.previousSource
                    gameImage.previousSource = gameImage.source
                }

                Connections {
                    target: AppState.minefieldModel

                    function onDataChanged() {
                        gameImage.reloadSource()
                    }

                    function onModelReset() {
                        gameImage.reloadSource()
                    }
                }
            }
        }

        GameDashboard {
            row: gameBoard.hoveredRow
            column: gameBoard.hoveredColumn

            Layout.alignment: Qt.AlignVCenter
        }
    }
}
