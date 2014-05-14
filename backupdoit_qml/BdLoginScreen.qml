import QtQuick 1.1

Rectangle {
    id: loginscreen
    width: 600
    height: 400
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#1d74af"
        }

        GradientStop {
            position: 1
            color: "#125288"
        }
    }

    signal loginStarted(string username, string password)
    onLoginStarted: {
    }

    Column {
        id: column1
        x: 80
        y: 0
        width: 300
        height: 100
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        Grid {
            id: grid1
            x: 86
            height: 50
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            rows: 2
            columns: 2

            Text {
                id: usernamelabel
                color: "#ffffff"
                text: qsTr("Username:")
                font.pixelSize: 12
            }

            Rectangle {
                id: usernamebackground
                x: 125
                y: 134
                width: 200
                height: 20
                color: "#ffffff"

                TextInput {
                    id: usernametextedit
                    text: qsTr("")
                    font.pointSize: 12
                    echoMode: TextInput.Normal
                    anchors.fill: parent
                    font.pixelSize: 12
                }
            }

            Text {
                id: passwordlabel
                x: 55
                y: 31
                color: "#ffffff"
                text: qsTr("Password:")
                font.pixelSize: 12
            }

            Rectangle {
                id: passwordbackground
                x: 108
                y: 112
                width: 200
                height: 20
                color: "#ffffff"

                TextInput {
                    id: passwordtextedit
                    text: qsTr("")
                    echoMode: TextInput.Password
                    font.pointSize: 11
                    anchors.fill: parent
                    font.pixelSize: 12
                }
            }
        }

        Row {
            id: row1
            height: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            BdButton {
                id: loginbutton
                x: 41
                y: 219
                width: 100
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                label: "Login"

                onButtonClick: {
                    loginscreen.loginStarted(usernametextedit.text, passwordtextedit.text);
                }
            }

            BdButton {
                id: quitbutton
                y: 50
                width: 100
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                label: "Quit"

                onButtonClick: {
                    Qt.quit();
                }
            }
        }
    }



}
