import QtQuick 1.1

Rectangle {
    width: 200
    height: 200
    color: "#D0D0D0"

    Gradient {
        id: actionGrad
        GradientStop {
            position: 0
            color: "#FFFFFF"
        }
        GradientStop {
            position: 1
            color: "#E0E0E0"
        }
    }

    Component {
        id: orderByBoxDelegate
        Rectangle {
            height: 25
            anchors.left: parent.left
            anchors.right: parent.right
            gradient: actionGrad

            Row {
                spacing: 10
                BdActionPrioRect { priorityval: priority }
                Text { text: name }
                Text { text: '#' + project }
                Text { text: '@' + context }
            }
        }
    }

    Component {
        id: simpleTestDelegate
        Row {
            spacing: 10
            Text { text: '@' + context }
            Text { text: name }
            Text { text: '#' + project }
        }
    }

    ListView {
        id: listview
        anchors.fill: parent
        model: BdSampleActionListModel { }
        delegate: orderByBoxDelegate
    }

    signal newOrderingSelected(int orderingType)
    onNewOrderingSelected: {
        switch(orderingType) {
        case 0: listview.delegate = orderByBoxDelegate;
            break;
        case 1: listview.delegate = simpleTestDelegate;
            break;
        }


    }

}
