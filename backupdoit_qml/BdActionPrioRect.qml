import QtQuick 1.1

Rectangle {
    id: priorityColourRect
    width: 5
    height: 25
    color: "#FFFFFF"

    property int priorityval: 0

    Component.onCompleted: {
        switch(priorityval) {
        case 0:
            color = "#EEEEEE";
            break;
        case 1:
            color = "#A5C4FA";
            break;
        case 2:
            color = "#4468A9";
            break;
        case 3:
            color = "#CF6000";
            break;
        }
    }
}
