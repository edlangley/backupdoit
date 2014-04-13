// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: rectangle2
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


    Grid {
        id: maingrid
        spacing: 10
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10
        anchors.fill: parent
        rows: 3
        columns: 2

        BdRadioArea {
            id: bdradioarea
            x: 1
            y: 31
            label: "Sort by:"

            BdRadioButton {
                id: bdradiobutton1
                buttonId: 0
                ticked: true
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.top: parent.top
                anchors.topMargin: 20
                label: "Box"
                anchors.left: parent.left
                anchors.leftMargin: 4
            }

            BdRadioButton {
                id: bdradiobutton2
                buttonId: 1
                anchors.right: parent.right
                anchors.rightMargin: 4
                label: "Context"
                anchors.top: parent.top
                anchors.topMargin: 44
                anchors.left: parent.left
                anchors.leftMargin: 4
            }

            BdRadioButton {
                id: bdradiobutton3
                buttonId: 2
                label: "Project"
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.top: parent.top
                anchors.topMargin: 68
            }

            BdRadioButton {
                id: bdradiobutton4
                buttonId: 3
                label: "Priority"
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.top: parent.top
                anchors.topMargin: 92
            }
        }

        BdActionList {
            id: actionlist
            anchors.left: parent.left
            anchors.leftMargin: 130
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            anchors.top: parent.top
            anchors.topMargin: 0

        }

        Text {
            id: savelocationtext
            y: 75
            height: 20
            text: qsTr("Save Location:")
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            font.pixelSize: 12
        }

        Row {
            id: savepathbrowserow
            height: 20
            spacing: 10
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 130
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30

            Rectangle {
                id: savepathbackground
                color: "#ffffff"
                anchors.right: parent.right
                anchors.rightMargin: 110
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0

                TextEdit {
                    id: savepathtext
                    text: qsTr("text edit")
                    anchors.fill: parent
                    font.pixelSize: 12
                }
            }

            BdButton {
                id: choose_file_button
                x: 0
                width: 100
                smooth: true
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                label: "..."

                // Temp handler for testing data download:
                onButtonClick: {
                    savepathtext.text = bdLogic.GetSaveFileName(0);
                }
            }

        }

        Rectangle {
            id: spacerrect
            y: 374
            width: 120
            height: 20
            color: "#00000000"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        Row {
            id: savequitrow
            y: 364
            height: 20
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 130
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            BdButton {
                id: savebutton
                x: 0
                y: 0
                height: 20
                anchors.right: parent.right
                anchors.rightMargin: 110
                label: "Save"

                onButtonClick: {
                    // TODO: retrieve file type choice from another radio button group
                    bdLogic.SaveDataToFile(savepathtext.text, 0);
                }
            }

            BdButton {
                id: quitbutton
                x: 0
                y: 0
                height: 20
                label: "Quit"
                anchors.right: parent.right
                anchors.rightMargin: 0

                // Temp handler for testing data download:
                // TODO: change this to close the application
                onButtonClick: {
                    actionlist.loadNewData();
                }
            }

        }
    }

    Component.onCompleted: {
        bdradioarea.announceSelected.connect(actionlist.newOrderingSelected);
        bdradioarea.announceSelected(0);

        // Temporarily do this here:
        savepathtext.text = bdLogic.ConnectAndDownload("<testuser>", "<testuserpassword>");
    }
}
