// 组件内部禁止通过ID访问外部对象
// pragma ComponentBehavior: Bound
import QtQuick

Rectangle {
    id:virtualkeymoreroot
    anchors.fill: parent
    color: "#c6c6c6"
    //每页6项
    property int itemCountPerPage:4
    property int currentindex:0

    signal click_list_value(string value)

    // 添加列表数据
    function listModelappend(m_value){
        valuelistmodelid.append({"value" :  m_value});
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
            height: virtualkeymoreroot.height/4
            width: virtualkeymoreroot.width - 4
            color:virtualkeymoreid.currentIndex == model.index ? "#cfcfcf":"#ffffff"
            Text {
                id: text
                text: value
                anchors.fill:parent
                //width: parent.width - 2
                //height: parent.height - 4
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 8
                font.family: "黑体"
                MouseArea {
                    anchors.fill: parent
                    onClicked: (mouse)=>{
                        //mouse.accepted = true
                        virtualkeymoreid.currentIndex = model.index;
                        virtualkeymoreroot.click_list_value(text.text)
                    }
                    // onPressed: function(mouse){
                    //     //mouse.accepted = true
                    //     recttext.color = "#cfcfcf"
                    // }
                    // onReleased: function(mouse){
                    //     //mouse.accepted = true
                    //     recttext.color = "#ffffff"
                    // }
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
        //     width: parent.width/3
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
