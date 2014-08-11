import QtQuick 2.3

import "../js/settings.js" as Settings

Rectangle {
    id: individualInfo;
    width: 100;
    height: 400;

    color: "#cccccc";

    border {
        color: "black";
        width: 2;
    }

    property int modelID: -1;

    signal distsUpdated();

    function updateIndividualInfo() {
        if (individualListView.model) {
            individualListView.model.destroy();
        }

        individualListView.model = Qt.createComponent("IndividualInfoModel.qml")
                                     .createObject(individualListView, {
                                                       "modelID" : modelID
                                                   });

        // why bother when no dists are updated?
        if (individualListView.model.distUpdatedCount) {
            distsUpdated();
        }
    }

    ListView {
        id: individualListView;
        delegate: delegateComponent;
        anchors.fill: parent;

        Component.onCompleted: updateIndividualInfo();
    }

    Component {
        id: delegateComponent;

        Rectangle {
            id: mainRectangle;

            property alias elementHeight: mainRectangle.height;
            property int borderRectMargin: 10

            border {
                color: "black";
                width: 1;
            }

            color: "white";

            width: individualInfo.width;
            height: 60;

            Rectangle {
                id: borderRect;

                anchors {
                    fill: parent;
                    margins: parent.borderRectMargin;
                }

                border {
                    color: "black";
                    width:  3;
                }

                visible: grouped;
            }

            Rectangle {
                id: header;
                color: "white";

                x: 20;
                y: 3;

                visible: grouped;

                width: childrenRect.width;
                height: grouped ? childrenRect.height : 0;

                Text {
                    id: headerText;

                    font {
                        pixelSize: 16;
                        bold: true;
                    }

                    visible: grouped;

                    text: itemName;
                    wrapMode: Text.WordWrap;
                }
            }

            Loader {
                id: subItemLoader;

                anchors {
                    top: header.bottom;
                    left: header.left;
                }

                sourceComponent: subItemComponent;
                onStatusChanged: {
                    if (status == Loader.Ready) {
                        item.model = subItems;
                        item.headerText = itemText;

                        if (item.currentItem) {
                            mainRectangle.height =
                                    subItemLoader.y + (grouped ? 4 : 1) * mainRectangle.borderRectMargin + item.headerItem.height + item.currentItem.height;
                        }
                    }
                }
            }

            // if some element changes its height recalculate as sum of heights of all items currently shown in listview
            onHeightChanged: {
                var newHeight = 0;
                // listview
                var elements = parent.parent.contentItem.children;
                for (var i = 0; i !== elements.length; ++ i) {
                    newHeight += elements[i].height;
                }

                individualInfo.height = newHeight;
            }
        }

    }

    Component {
        id: subItemComponent;

        GridView {
            id: gridView;

            property string headerText: "";

            header:
                Text {
                    id: headerText;
                    font {
                        pixelSize: 14;
                        italic: true;
                    }

                    text: gridView.headerText;
                    wrapMode: Text.WordWrap;
                }

            delegate: textMeasures;
        }
    }

    Component {
        id: textMeasures;
        Column {
            id: textMeasuresColumn;
            property variant model: GridView.view.model;
            Repeater {
                model: parent.model;
                delegate: Row {
                    Text {
                        id: from2To;
                        width: individualInfo.width - 40 - data.width;
                        text: from + " --> " + to;
                        clip: true;
                    }
                    Text {
                        id: data;
                        text: measure;
                    }
                }
            }
        }
    }
}
