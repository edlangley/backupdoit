// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 100
    height: 50
    radius: 4
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#559ed4"
        }

        GradientStop {
            position: 1
            color: "#2771ad"
        }
    }

    property string label

    Text {
        id: bdButtonLabel
        color: "#ffffff"
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
