import QtQuick
import Minesolver

Item {
    id: gameBoard

    property int hoveredRow: -1
    property int hoveredColumn: -1

    AspectPreserver {
        aspectRatio: 1.0 * minefieldTable.columns / minefieldTable.rows

        anchors.centerIn: parent

        TableView {
            id: minefieldTable

            boundsMovement: Flickable.StopAtBounds

            anchors.fill: parent

            keyNavigationEnabled: false
            pointerNavigationEnabled: false
            selectionBehavior: TableView.SelectionDisabled

            property int cellSize: Math.max(0, Math.min(width / columns, height / rows))

            Timer {
                id: forceLayoutTimer

                interval: 500

                onTriggered: minefieldTable.forceLayout()
            }

            onCellSizeChanged: forceLayoutTimer.running = true

            function assignModel() {
                model = AppState.showInteractiveGameBoard? AppState.minefieldModel : null
            }

            Component.onCompleted: {
                minefieldTable.assignModel()
            }

            Connections {
                target: AppState

                function onShowInteractiveGameBoardChanged() {
                    minefieldTable.assignModel()
                }
            }

            delegate: Rectangle {
                color: "gray"
                border.color: "black"

                implicitWidth: minefieldTable.cellSize
                implicitHeight: minefieldTable.cellSize

                MouseArea {
                    id: clickCatcher
                    hoverEnabled: true
                    acceptedButtons: !AppState.minefieldModel.gameWon && !AppState.minefieldModel.gameLost?
                                         Qt.LeftButton | Qt.RightButton : 0
                    anchors.fill: parent

                    onEntered: {
                        gameBoard.hoveredRow = row
                        gameBoard.hoveredColumn = column
                    }
                }

                Rectangle {
                    border.color: "blue"
                    border.width: 3
                    color: "transparent"
                    anchors.fill: parent

                    visible: hoveredRow === row && hoveredColumn === column
                }

                Image {
                    source: "qrc:/icons/exploded.png"
                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit

                    visible: isExplodedMine
                }

                Image {
                    source: "qrc:/icons/mine.png"
                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit

                    visible: isMine
                }

                Image {
                    source: "qrc:/icons/smiley.png"
                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit

                    visible: isUnexplodedMine
                }

                Image {
                    source: "qrc:/icons/flag.png"
                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit

                    visible: guessMine
                }

                Image {
                    source: "qrc:/icons/mine.png"

                    opacity: chanceMine * 0.95 + 0.05

                    fillMode: Image.PreserveAspectFit

                    anchors.fill: parent

                    visible: chanceMine > 0 && AppState.showSolution && !guessMine &&
                             !AppState.minefieldModel.gameLost && !AppState.minefieldModel.gameWon
                }

                Rectangle {
                    color: "light gray"

                    Text {
                        color: "black"
                        text: count
                        anchors.centerIn: parent
                        visible: count > 0
                    }

                    anchors.fill: parent
                    anchors.margins: 2

                    visible: count >= 0
                }

                Connections {
                    target: clickCatcher

                    function onClicked(mouseEvent) {
                        if(mouseEvent.button === Qt.LeftButton)
                        {
                            AppState.minefieldModel.reveal(row, column)
                        }
                        else
                        {
                            AppState.minefieldModel.toggleGuessMine(row, column)
                        }
                    }

                    function onDoubleClicked(mouseEvent) {
                        AppState.minefieldModel.revealAdjacent(row, column)
                    }
                }
            }
        }
    }
}
