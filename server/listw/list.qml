import QtQuick 2.15

Item {
    ListView{
        id:listview
        spacing: 10
        anchors.fill: parent
        model:listmodel
        //interactive: false
        //clip: true
        delegate: Rectangle {
            color: (listview.currentIndex === index && listview.currentIndex >= 0) ? "#0ff0f0" : "#a0a0a0"
            id: item
            width: listview.width
            height: listview.height/5
            Row {
                anchors.fill: parent
                Text {
                    id: text1
                    width: parent.width/3
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr(t1.toString())
                    MouseArea {
                      anchors.fill: parent
                      onClicked: {
                          console.log(t1)
                      }
                    }
                }
                Text {
                    id: text2
                    width: parent.width/3
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr(t2.toString())
                    MouseArea {
                      anchors.fill: parent
                      onClicked: {
                          console.log(t2)
                      }
                    }
                }
                Text {
                    id: text3
                    width: parent.width/3
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr(t3.toString())
                    MouseArea {
                      anchors.fill: parent
                      onClicked: {
                          console.log(t3)
                      }
                    }
                }
            }
            MouseArea {
              anchors.fill: parent
              propagateComposedEvents: true
              hoverEnabled: true
              onClicked: {
                  mouse.accepted = false
                  listview.currentIndex = index;
                  for(var i = 0; i < listview.contentItem.children.length; ++i){
                      listview.contentItem.children[i].color = "#a0a0a0"
                      // for(var j = 0; j < listview.contentItem.children[i].children.length; ++j){
                      //     console.log(listview.contentItem.children[i].children[j]);
                      // }
                  }
                  item.color = "#0ff0f0"
              }
              onEntered: {
                  item.color = "#1f4f30"
              }
              onExited: {
                  item.color = (listview.currentIndex === index && listview.currentIndex >= 0) ? "#0ff0f0" : "#a0a0a0"
              }
            }
        }
        onCurrentIndexChanged: {
            listview.model.currentIndext = listview.currentIndex
            //console.log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",listview.model.currentIndext)
        }
    }
}
