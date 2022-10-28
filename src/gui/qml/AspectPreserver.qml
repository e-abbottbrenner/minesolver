import QtQuick

Item {
    property real aspectRatio

    id: aspectPreserver

    anchors.centerIn: parent

    function fitInParent() {
        aspectPreserver.width = Math.min(aspectPreserver.parent.height * aspectRatio, aspectPreserver.parent.width)
        aspectPreserver.height = Math.min(aspectPreserver.parent.height, aspectPreserver.parent.width / aspectRatio)
    }

    Connections {
        target: parent

        function onHeightChanged() {
            aspectPreserver.fitInParent()
        }

        function onWidthChanged() {
            aspectPreserver.fitInParent()
        }
    }

    onAspectRatioChanged: aspectPreserver.fitInParent()
}
