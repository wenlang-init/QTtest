// 组件内部禁止通过ID访问外部对象
//pragma ComponentBehavior: Bound
import QtQuick

Rectangle {
    id:virtualkeysymbalroot
    anchors.fill: parent
    color: "#c6c6c6"
    //每页6项
    property int itemCountPerPage:4
    property int currentindex:0

    signal click_list_value(string value)

    // 添加列表数据
    function listModelappend(m_value1,m_value2,m_value3,m_value4){
        valuelistmodelid.append({"value1" :  m_value1,"value2" :  m_value2,"value3" :  m_value3,"value4" :  m_value4});
    }
    // 清空列表数据
    function clean_all_data(){
        valuelistmodelid.clear()
    }

    ListModel {
        id: valuelistmodelid
    }

    Component {
        id: valuerectid
        Rectangle{
            id:recttext
            height: virtualkeysymbalroot.height/4
            width: virtualkeysymbalroot.width - 4
            Row{
                Rectangle{
                    id:rect1
                    width: recttext.width/4 - 2
                    height: recttext.height
                    color: "#ffffff"
                    Text {
                        id: text1
                        text: value1
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 8
                        font.family: "黑体"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: function(mouse){
                                // mouse.accepted = true
                                virtualkeysymbalroot.click_list_value(text1.text)
                            }
                            onPressed: {
                                rect1.color = "#cfcfcf"
                            }
                            onReleased: {
                                rect1.color = "#ffffff"
                            }
                        }
                    }
                }
                Rectangle{
                    id:rect2
                    width: rect1.width
                    height: rect1.height
                    color: "#ffffff"
                    Text {
                        id: text2
                        text: value2
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: text1.font
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                virtualkeysymbalroot.click_list_value(text2.text)
                            }
                            onPressed: {
                                rect2.color = "#cfcfcf"
                            }
                            onReleased: {
                                rect2.color = "#ffffff"
                            }
                        }
                    }
                }
                Rectangle{
                    id:rect3
                    width: rect1.width
                    height: rect1.height
                    color: "#ffffff"
                    Text {
                        id: text3
                        text: value3
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: text1.font
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                virtualkeysymbalroot.click_list_value(text3.text)
                            }
                            onPressed: {
                                rect3.color = "#cfcfcf"
                            }
                            onReleased: {
                                rect3.color = "#ffffff"
                            }
                        }
                    }
                }
                Rectangle{
                    id:rect4
                    width: rect1.width
                    height: rect1.height
                    color: "#ffffff"
                    Text {
                        id: text4
                        text: value4
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: text1.font
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                virtualkeysymbalroot.click_list_value(text4.text)
                            }
                            onPressed: {
                                rect4.color = "#cfcfcf"
                            }
                            onReleased: {
                                rect4.color = "#ffffff"
                            }
                        }
                    }
                }
            }
        }
    }

    ListView {
        id: virtualkeymoreid
        anchors.fill: parent
        model: valuelistmodelid
        delegate: valuerectid
        clip:true
        // MouseArea {
        //     //anchors.fill: parent
        //     height: parent.height
        //     anchors.right: parent.right
        //     width: parent.width/8
        //     onClicked: {
        //         if(mouseY > virtualkeymoreid.height/2){
        //             if(currentindex < virtualkeymoreid.count-itemCountPerPage){
        //                 currentindex++
        //                 virtualkeymoreid.positionViewAtIndex(currentindex,virtualkeymoreid.Beginning)
        //             }
        //         } else if(mouseY < virtualkeymoreid.height/2){
        //             if(currentindex > 0){
        //                 currentindex--
        //                 virtualkeymoreid.positionViewAtIndex(currentindex,virtualkeymoreid.Beginning)
        //             }
        //         }
        //     }
        // }
    }

}
