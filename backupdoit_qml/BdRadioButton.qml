// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    x: 0
    y: 0
    width: 120
    height: 20
    color: "#00000000"
    radius: 0

    property string label
    property bool ticked
    property int buttonId

    signal buttonClick()
    onButtonClick: {
        parent.clearChildToggles();
        ticked = 1;
        parent.announceSelected(buttonId);
    }

    signal clearToggle()
    onClearToggle: {
        ticked = 0;
    }

    Text {
        id: buttonLabel
        text: label
        anchors.left: togglebox.right
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        font.family: "FreeSans"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: buttonClick()
    }

    Rectangle {
        id: togglebox
        x: 4
        y: 4
        width: height
        height: parent.height - 8
        color: "#ffffff"
        radius: 2
        border.width: 2
        border.color: "#000000"

        Rectangle {
            id: togglemark
            x: 2
            y: 2
            width: parent.width - 4
            height: parent.height - 4
            color: "#091b4d"
            radius: 2
            visible: ticked
        }
    }
}
