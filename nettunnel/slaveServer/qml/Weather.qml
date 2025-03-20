import QtQuick
import QtQuick.Controls

Item {
    //anchors.fill: parent
    //color: "#6f9012"
    Row {
        id:row
        width:parent.width
        height: 50
        anchors.left: parent.left
        anchors.top: parent.top

        //ComboBox
        MComboBox
        {
            id:combox
            width: parent.width - button.width
            height: button.height
            anchors.top: button.top
            filterAble:true

            editable: true

            //model: ["123"]
            model: ListModel {
                //ListElement { text: "Banana" }
            }
            onDisplayTextChanged: {
                //console.log(combox.displayText)
            }
        }

        Button {
            id:button
            width: 80
            height: parent.height
            anchors.top: parent.top
            text: "查询"
            font.bold: true
            icon.color: "#271268"
            onClicked: {
                qmlInTurn.queryWeather(combox.displayText)
            }
        }
    }

    SwipeView{
        id:sw1
        anchors.top: row.bottom
        anchors.left: parent.left
        width: parent.width
        height: parent.height - row.height

        Item {
            width: sw1.width
            height: sw1.height
            Flickable {
                id:flickableyesterday
                anchors.fill: parent
                //ScrollBar.vertical: ScrollBar{}
                clip: true
                contentWidth: parent.width > textyesterday.contentWidth ? parent.width : textyesterday.contentWidth
                contentHeight: parent.height > textyesterday.contentHeight ? parent.height : textyesterday.contentHeight

                Text {
                    id:textyesterday
                    color: "#111111"
                    //text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    transformOrigin: Item.Center
                    wrapMode: TextArea .Wrap // 换行
                }
            }
        }
        Item {
            width: sw1.width
            height: sw1.height
            Flickable {
                id:flickablehome
                anchors.fill: parent
                //ScrollBar.vertical: ScrollBar{}
                clip: true
                contentWidth: parent.width > texthome.contentWidth ? parent.width : texthome.contentWidth
                contentHeight: parent.height > texthome.contentHeight ? parent.height : texthome.contentHeight

                Text {
                    id:texthome
                    color: "#111111"
                    //text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    transformOrigin: Item.Center
                    wrapMode: TextArea .Wrap // 换行
                }
            }
        }
        Item {
            width: sw1.width
            height: sw1.height
            Flickable {
                id:flickable
                anchors.fill: parent
                //ScrollBar.vertical: ScrollBar{}
                clip: true
                contentWidth: parent.width > textother.contentWidth ? parent.width : textother.contentWidth
                contentHeight: parent.height > textother.contentHeight ? parent.height : textother.contentHeight

                TextArea {
                    id:textother
                    //readOnly: true
                    enabled: false
                    //clip: true
                    //cursorVisible: true
                    anchors.fill: parent
                    wrapMode: TextArea .Wrap // 换行
                    font.bold: true
                    selectByMouse:true
                    selectByKeyboard: true
                    //text: qsTr("ttttttt")
                    color: "#111111"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    transformOrigin: Item.Center
                }
            }
        }
    }

    Component.onCompleted: {
        var allcitylist = qmlInTurn.getCityList()
        for(var i=0;i<allcitylist.length;i++){
            combox.model.append({text: allcitylist[i]})
            //combox.model.push(allcitylist[i])
        }
        combox.currentIndex = combox.find("成都")
        //console.log(combox.displayText)
    }

    Connections {
        target: qmlInTurn

        //function onWeatherValueChanged(listValue) {
        function onQueryWeatherValue(listValue) {
            //console.log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" + listValue.length)
            var str = ""
            for(var i=0;i<listValue.length;i++) {
                if(i == 0) {
                    textyesterday.text = listValue[i]
                } else if(i == 1) {
                    texthome.text = listValue[i]
                } else {
                    str += listValue[i]
                    if(i != listValue.length-1){
                        str += "------------------------------------------------\n"
                    }
                }
                //console.log(listValue[i])
            }
            textother.text = str
            sw1.setCurrentIndex(1)
        }
    }
}
