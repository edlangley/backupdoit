
import QtQuick 1.1

Item {
    id: bdtransitionscreen
    width: 800
    height: 600

    BdLoginScreen {
        id: bdloginscreen
        width: parent.width
        height: parent.height
    }

    BdProgressScreen {
        id: bdprogscreen
        width: parent.width
        height: parent.height
        anchors.left: bdloginscreen.right
    }

    BdMainScreen {
        id: bdmainScreen
        width: parent.width
        height: parent.height
        anchors.left: bdprogscreen.right
    }

    states: [
        State {
            name: "login"
            PropertyChanges {
                target: bdtransitionscreen
                x: 0
            }
        },
        State {
            name: "progress"
            PropertyChanges {
                target: bdtransitionscreen
                x: -(bdloginscreen.width)
            }
        },
        State {
            name: "mainscreen"
            PropertyChanges {
                target: bdtransitionscreen
                x: -(bdloginscreen.width * 2)
            }
        }
    ]
    /*
    Here the transitions are defined. A simple NumberAnimation is used to animate the "y" property
    of the mainScreen.
    */
    transitions: [
        Transition {
            from: "login"
            to: "progress"
            NumberAnimation {
                properties: "x"
                duration: 1000
                easing.type: Easing.InOutCubic
            }

        },
        Transition {
            from: "progress"
            to: "mainscreen"
            NumberAnimation {
                properties: "x"
                duration: 1000
                easing.type: Easing.InOutCubic
            }
        }
    ]

    signal loginStarted(string username, string password)
    onLoginStarted: {
        bdtransitionscreen.state = "progress";
        bdLogic.ConnectAndDownload(username, password);
    }

    Connections {
        target: bdLogic
        onDownloadStatusUpdated: {
            switch(status) {
            case 2:
                {
                    bdprogscreen.statusMessage = message;
                    bdmainScreen.loadNewData();
                    bdtransitionscreen.state = "mainscreen";
                }
                break;
            default:
                {
                    bdprogscreen.statusMessage = message;
                }
                break;
            }

            // TODO: if status < 0, stop progress animation
        }
    }

    Component.onCompleted: {
        bdloginscreen.loginStarted.connect(bdtransitionscreen.loginStarted);
        bdtransitionscreen.state = "login";
    }
}
