import QtQuick
import QtQuick.Controls

Rectangle {
    //clip: true
    //opacity: 1
    border.width: 1
    border.color: "#ff000000"
    //property bool isreplay: false
    property alias textArea:textarea

    Flickable {
        id:flickable
        clip: true
        //anchors.fill: parent
        width: parent.width - parent.border.width*2
        height: parent.height - parent.border.width*2
        anchors.centerIn: parent
        //contentWidth: parent.width > textarea.contentWidth ? parent.width : textarea.contentWidth
        //contentHeight: parent.height > textarea.contentHeight ? parent.height : textarea.contentHeight

        contentWidth: textArea.width
        contentHeight: textArea.implicitHeight

        // 启用滚动条
        //ScrollBar.vertical: ScrollBar {}
        //ScrollBar.horizontal: ScrollBar {}

        TextArea {
            id:textarea
            width:flickable.width
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
            //wrapMode: TextArea.NoWrap  // 禁用自动换行
            onTextChanged: {
                // 当文本变化时，更新 Flickable 的内容高度
                flickable.contentHeight = textArea.implicitHeight
                //flickable.contentY = flickable.contentHeight - textArea.height
            }
        }
    }
}
