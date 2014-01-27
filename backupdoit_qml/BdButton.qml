// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 100
    height: 50
    radius: 8
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#4f68e6"
        }

        GradientStop {
            position: 1
            color: "#0b06aa"
        }
    }

    property string label

    Text {
        id: bdButtonLabel
        text: label
        anchors.centerIn: parent
        font.family: "FreeSans"
    }

    signal buttonClick()
    onButtonClick: {
        console.log(bdButtonLabel.text + " clicked")
    }

    MouseArea {
        anchors.fill: parent
        onClicked: buttonClick()
    }
}
