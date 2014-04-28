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
                Text { text: name }
                Text {
                    text: { (project != '') ? '#' + project : '' }
                }
                Text {
                    text: { (context != '') ? '@' + context : '' }
                }
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
        id: orderedByContextItemDelegate

        Rectangle {
            gradient: actionGrad

            Row {
                spacing: 10
                BdActionPrioRect { priorityval: priority }
                Text { text: name }
                Text {
                    text: { (project != '') ? '#' + project : '' }
                }
                Text { text: '^' + box }
            }
        }
    }
    Component {
        id: orderByContextDelegate

        Loader {
            anchors.fill: parent
            sourceComponent: {
                if(itemType == 0)
                    return orderedByContextItemDelegate;
                else
                    return headerDelegate;
            }
        }
    }

    Component {
        id: orderedByProjectItemDelegate

        Rectangle {
            gradient: actionGrad

            Row {
                spacing: 10
                BdActionPrioRect { priorityval: priority }
                Text { text: name }
                Text {
                    text: { (context != '') ? '@' + context : '' }
                }
                Text { text: '^' + box }
            }
        }
    }
    Component {
        id: orderByProjectDelegate

        Loader {
            anchors.fill: parent
            sourceComponent: {
                if(itemType == 0)
                    return orderedByProjectItemDelegate;
                else
                    return headerDelegate;
            }
        }
    }

    Component {
        id: orderedByPriorityItemDelegate

        Rectangle {
            gradient: actionGrad

            Row {
                spacing: 10
                BdActionPrioRect { priorityval: priority }
                Text { text: name }
                Text {
                    text: { (project != '') ? '#' + project : '' }
                }
                Text {
                    text: { (context != '') ? '@' + context : '' }
                }
                Text { text: '^' + box }
            }
        }
    }
    Component {
        id: orderByPriorityDelegate

        Loader {
            anchors.fill: parent
            sourceComponent: {
                if(itemType == 0)
                    return orderedByPriorityItemDelegate;
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
                Text { text: name }
                Text { text: '#' + project }
                Text { text: '@' + context }
                Text { text: '^' + box }
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
                return orderByContextDelegate;
            case 2:
                return orderByProjectDelegate;
            case 3:
                return orderByPriorityDelegate;
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
