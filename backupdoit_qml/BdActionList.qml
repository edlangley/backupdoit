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
        // TODO: Just declare a plain empty ListModel here:
        model: BdSampleActionListModel {
            id: actionListModel
        }
        delegate: BdActionListDelegates {
            id: actionListDelegates
        }
    }

    signal newOrderingSelected(int newOrderingType)
    onNewOrderingSelected: {
        actionListview.orderingType = newOrderingType;

        // TODO: call setordering in bdlogic,
        //       call loadNewData()
    }

    function loadNewData() {
        actionListModel.clear();

        var loadedactionlist = bdLogic.GetDataModel();
        for (var i = 0; i < loadedactionlist.length; i++) {
            var action = loadedactionlist[i];

            actionListModel.append({
                                       "itemType": action["itemType"],
                                       "name": action["name"],
                                       "box": action["box"],
                                       "project": action["project"],
                                       "context": action["context"],
                                       "priority": action["priority"]
                                   });

        }
    }
}
