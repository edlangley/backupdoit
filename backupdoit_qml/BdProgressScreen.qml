import QtQuick 1.1

Rectangle {
    id: rectangle1
    width: 600
    height: 400
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#1423ee"
        }

        GradientStop {
            position: 1
            color: "#8f97ff"
        }
    }

    property string statusMessage

    Text {
        id: text1
        x: 270
        y: 193
        height: 40
        text: statusMessage
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        font.pointSize: 25
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
    }

    Text {
        id: text2
        x: 316
        y: 178
        text: qsTr("Loading....")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 30
        font.pixelSize: 12
    }
}
