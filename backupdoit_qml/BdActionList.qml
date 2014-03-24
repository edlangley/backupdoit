import QtQuick 1.1

Rectangle {

    // Empty data model to be populated with data provided from C++
    ListModel {
        id: dynamicModel
    }

    ListView {
        id: actionListview
        property int orderingType: 0

        anchors.fill: parent
        model: BdSampleActionListModel { }
        delegate: BdActionListDelegates {
            id: actionListDelegates
        }
    }

    signal newOrderingSelected(int newOrderingType)
    onNewOrderingSelected: {
        actionListview.orderingType = newOrderingType;
    }
}
