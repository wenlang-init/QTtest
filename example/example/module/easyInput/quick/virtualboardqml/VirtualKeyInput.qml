import QtQuick

Rectangle {
    id:virtualkeyinputroot
    anchors.left: parent.left
    //anchors.leftMargin: 10
    anchors.bottom: parent.bottom
    width: parent.width // - 20
    height: 300

    //color: "#ffffff"
    color: Qt.rgba(0xff,0xff,0xff,0)

    signal get_code_value(string value)
    signal input_key_signal(string value)

    // 清空显示信息
    function init_display_info(value){
        if(value ===  1){ // 为1时显示数字键盘
            virtualkeyt26.show_info_list(4)
        } else {
            virtualkeyt26.set_init_show_T26()
        }
    }

    // 防止按键穿透
    MouseArea{
        anchors.fill: parent
    }

//    // T9布局
//    VirtualKeyT9{
//        id:virtualkeyt9
//        anchors.fill: parent
//        visible: false
//    }

    // T26布局
    VirtualKeyT26{
        id:virtualkeyt26
        anchors.fill: parent
        visible: true
        onOut_code_data: (value)=>{
            virtualkeyinputroot.get_code_value(value)
        }
        onExit_T26: {
            virtualkeyinputroot.visible = false
        }
        onT26_key_signal: (value)=>{
            virtualkeyinputroot.input_key_signal(value)
        }
    }

}
