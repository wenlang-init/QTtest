import QtQuick
import QtQuick.Controls

Rectangle {
    //clip: true
    //opacity: 1
    border.width: 1
    border.color: "#ff000000"
    //property bool isreplay: false
    property alias textArea:textarea

    ScrollView {
        clip: true
        //anchors.fill: parent
        width: parent.width - parent.border.width*2
        height: parent.height - parent.border.width*2
        anchors.centerIn: parent
        TextArea {
            id:textarea
            //anchors.fill: parent
            //opacity: 0.5
            //readOnly: true
            //enabled: false
            //horizontalAlignment: Text.AlignHCenter
            //verticalAlignment: Text.AlignVCenter
            transformOrigin: Item.Center

            padding: 5
            color: "black"
            //无输入时的提示文本
            //placeholderText: "Gong Jian Bo"
            //placeholderTextColor: "gray"
            //text: "<p>Gong Jian Bo</p>"
            //支持富文本
            textFormat: TextEdit.RichText
            //光标样式是可以自定义的
            //cursorVisible: focus
            //鼠标选取文本默认为false
            selectByMouse: true
            //键盘选取文本默认为true
            selectByKeyboard: true
            //选中文本的颜色
            selectedTextColor: "white"
            //选中文本背景色
            selectionColor: "blue"
            //超出宽度时开启允许滚动，默认为true
            //字体的属性也比较多，可以自行看文档
            font{
                family: "SimSun"
                pixelSize: 16
            }
            //截取超出部分
            //clip: true
            //默认Text.QtRendering看起来比较模糊
            //renderType: Text.NativeRendering
            //文本换行，默认NoWrap
            wrapMode: TextEdit.Wrap
        }
    }
}
