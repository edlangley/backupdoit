import QtQuick 1.1

Rectangle {
    x: 0
    y: 0
    width: 120
    height: 120
    color: "#00000000"
    radius: 4
    border.width: 2
    border.color: "#000000"

    property string label

    signal clearChildToggles()
    onClearChildToggles: {
        for(var i = 0; i < children.length; i++) {
            children[i].clearToggle()
        }
    }

    // on completed, connect children button clicked to announce chosen toggle signal
    signal announceSelected(int buttonIx)

    Text {
        id: titletext
        height: 14
        text: label
        anchors.left: parent.left
        anchors.leftMargin: 4
        z: 1
        anchors.top: parent.top
        anchors.topMargin: 4
        font.family: "FreeSans"
    }
}
