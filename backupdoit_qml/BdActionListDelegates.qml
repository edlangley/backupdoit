import QtQuick 1.1

Rectangle {
    height: 25
    anchors.left: parent.left
    anchors.right: parent.right

    Gradient {
        id: actionGrad
        GradientStop {
            position: 0
            color: "#FFFFFF"
        }
        GradientStop {
            position: 1
            color: "#808080"
        }
    }

    // View delegates
    Component {
        id: headerDelegate
        Rectangle {
            color: "#ECF1F6"

            Row {
                x: 10
                anchors.verticalCenter: parent.verticalCenter

                spacing: 10
                Text {
                    text: name
                    color: "#858191"
                    font.bold: true
                }
            }
        }
    }

    Component {
        id: orderedByBoxItemDelegate

        Rectangle {
            gradient: actionGrad

            Row {
                spacing: 10
                BdActionPrioRect { priorityval: priority }
                Text { text: box + ':' }
                Text { text: name }
                Text { text: '#' + project }
                Text { text: '@' + context }
            }
        }
    }
    Component {
        id: orderByBoxDelegate

        Loader {
            anchors.fill: parent
            sourceComponent: {
                if(itemType == 0)
                    return orderedByBoxItemDelegate;
                else
                    return headerDelegate;
            }
        }
    }

    Component {
        id: simpleTestDelegate

        Rectangle {
            gradient: actionGrad

            Row {
                spacing: 10
                Text { text: '@' + context }
                Text { text: name }
                Text { text: '#' + project }
            }
        }
    }

    property int orderingType: actionListview.orderingType

    Loader {
        id: delegateLoader
        anchors.fill: parent

        sourceComponent: {
            switch(orderingType) {
            case 0:
            default:
                return orderByBoxDelegate;
            case 1:
                return simpleTestDelegate;
            case 2:
                return orderByBoxDelegate;
            case 3:
                return orderByBoxDelegate;
            }
        }
    }

    Connections {
        target: actionListview
        onOrderingTypeChanged: {
            orderingType = actionListview.orderingType;
        }
    }
}
