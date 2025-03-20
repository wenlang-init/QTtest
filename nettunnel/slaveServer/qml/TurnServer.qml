import QtQuick
import QtQuick.Controls

Item {
    //anchors.fill: parent
    property bool isreplay: false
    Rectangle{
        opacity: 0.5
        id:toprect
        width:root.width
        height:50
        x:0
        Row{
            anchors.fill: parent
            Rectangle{
                id:button
                y:0
                x:0
                width:50
                height:parent.height
                color:"#666666"
                border.color: "blue"
                border.width: 2
                Text{
                    color: "#111111"
                    text: "自动回复"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    transformOrigin: Item.Center
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if(isreplay){
                                isreplay = false
                                button.color = "#666666"
                            } else {
                                isreplay = true
                                button.color = "#3f5f7f"
                            }
                        }
                    }
                }
            }
            Rectangle{
                width:parent.width - connectbutton.width - button.width
                height:parent.height
                Row{
                    anchors.fill: parent
                    TextField {
                        id:masteraddr
                        width:parent.width/3
                        height:parent.height
                        color:"#e35f6e"
                        text: "127.0.0.1"
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        font.pointSize: 16
                        //默认鼠标选取文本设置为false
                        selectByMouse: true
                        //选中文本的颜色
                        selectedTextColor: "white"
                        //选中文本背景色
                        selectionColor: "black"
                        //超出宽度时开启允许滚动，默认为true
                        autoScroll: true
                        //无输入时显示的文本
                        placeholderText: "中心服务地址"
                        placeholderTextColor: "gray"
                        onTextChanged: {
                            qmlInTurn.setConfig(masteraddr.text,masterport.text,serverflag.text)
                        }
                    }
                    TextField {
                        id:masterport
                        width:parent.width/3
                        height:parent.height
                        text: "12345"
                        color:"#e35f6e"
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        font.pointSize: 16
                        //默认鼠标选取文本设置为false
                        selectByMouse: true
                        //选中文本的颜色
                        selectedTextColor: "white"
                        //选中文本背景色
                        selectionColor: "black"
                        //超出宽度时开启允许滚动，默认为true
                        autoScroll: true
                        //无输入时显示的文本
                        placeholderText: "中心服务端口"
                        placeholderTextColor: "gray"
                        onTextChanged: {
                            qmlInTurn.setConfig(masteraddr.text,masterport.text,serverflag.text)
                        }
                    }
                    TextField {
                        id:serverflag
                        width:parent.width/3
                        height:parent.height
                        text: "0x12345678"
                        color:"#e35f6e"
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        font.pointSize: 16
                        //默认鼠标选取文本设置为false
                        selectByMouse: true
                        //选中文本的颜色
                        selectedTextColor: "white"
                        //选中文本背景色
                        selectionColor: "black"
                        //超出宽度时开启允许滚动，默认为true
                        autoScroll: true
                        //无输入时显示的文本
                        placeholderText: "本地(服务端)标志"
                        placeholderTextColor: "gray"
                        onTextChanged: {
                            qmlInTurn.setConfig(masteraddr.text,masterport.text,serverflag.text)
                        }
                    }
                }
            }
            Button {
                id:connectbutton
                width:80
                height:parent.height
                text:"打开"
                font.bold: true
                // background: Rectangle{
                //     //anchors.fill: parent
                //     color:"#e35f6e"
                // }
                onClicked: {
                    if(qmlInTurn.connectServer()){
                        connectbutton.text = "关闭"
                    } else {
                        connectbutton.text = "打开"
                    }
                }
            }
        }
    }
    ScrollView {
        id:scrollview
        anchors.top:toprect.bottom
        x:0
        width:root.width
        height:root.height - toprect.height
        clip: true
        opacity: 0.5
        //ScrollBar.horizontal: ScrollBar{ visible: false }
        background: Rectangle{
            border.color: textarea.activeFocus? "blue": "black"
            border.width: textarea.activeFocus? 2: 1
        }

        TextArea{
            id:textarea
            //anchors.fill: parent
            //opacity: 0.5

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
            renderType: Text.NativeRendering
            //文本换行，默认NoWrap
            wrapMode: TextEdit.Wrap
        }
    }

    Connections {
        target: qmlInTurn
        function onRecvTextChanged(value){
            //console.log(value)
            if(textarea.text.length > 1024*1024*128){
                textarea.text = ""
            }

            textarea.text += value
            if(isreplay){
                qmlInTurn.sendData(value)
            }
        }
    }

    Component.onCompleted: {
        qmlInTurn.setConfig(masteraddr.text,masterport.text,serverflag.text)
    }
}
