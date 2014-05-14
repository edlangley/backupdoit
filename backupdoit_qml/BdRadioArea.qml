import QtQuick 1.1

Rectangle {
    x: 0
    y: 0
    width: 120
    height: 120
    color: "#00000000"
    radius: 4
    border.width: 2
    border.color: "#ffffff"

    property string label
    property int currentSelectedButtonIx

    signal clearChildToggles()
    onClearChildToggles: {
        for(var i = 0; i < children.length; i++) {
            children[i].clearToggle()
        }
    }

    // on completed, connect children button clicked to announce chosen toggle signal
    signal announceSelected(int buttonIx)
    onAnnounceSelected: {
        currentSelectedButtonIx = buttonIx;
    }
    function getCurrentSelected() {
        return currentSelectedButtonIx;
    }


    Text {
        id: titletext
        height: 14
        color: "#ffffff"
        text: label
        anchors.left: parent.left
        anchors.leftMargin: 4
        z: 1
        anchors.top: parent.top
        anchors.topMargin: 4
        font.family: "FreeSans"
    }
}
