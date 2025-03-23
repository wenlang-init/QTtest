import QtQuick
import QtQuick.Controls

Item {

    Rectangle {
        id:rect1
        //opacity: 0.5
        width:parent.width
        height:50
        x:0
        //color: "#ff0000ff"
        Row{
            // width: parent.width
            // height: parent.height
            // anchors.left: parent.left
            // anchors.top: parent.top
            anchors.fill: parent
            spacing: 1
            Rectangle {
                width: 70
                height: parent.height
                Text {
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    transformOrigin: Item.Center
                    font.bold: true
                    font.pointSize: 16
                    color: "#FF111111"
                    text: "token:"
                }
            }
            TextField{
                id:textkey
                width: parent.width - parent.spacing*2 - 200
                height: parent.height
                color:"#e35f6e"
                text: "sk-c4a304ec00d2419ea12eb03d60064668"
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
                placeholderText: "在这里输入token"
                placeholderTextColor: "gray"
                onTextChanged: {

                }
            }
            CheckBox{
                id:checkbox
                width: 200
                height: parent.height
                text: qsTr("深度思考(R1)")
                checked: false
            }
        }
    }

    Rectangle {
        id:rect2
        //opacity: 1
        width:rect1.width
        height:parent.height - rect1.height
        anchors.left: rect1.left
        anchors.top: rect1.bottom
        Column{
            anchors.fill: parent
            // anchors.left: parent.left
            // anchors.leftMargin: 10
            // anchors.right:parent.right
            // anchors.rightMargin: 10
            spacing: 1
            /*FliTextA*/ScrTextA {
                id:textout
                //opacity:1
                color: "#8f432ff6"
                width: parent.width
                height: parent.height/4*3
                textArea.textFormat: TextEdit.PlainText
                //textArea.text: qsTr("asfsf\nassg\n2233\nd啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊nd啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊nd啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊nd啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊123")
                //textArea.placeholderText: qsTr("asfsf\nassg\n2233\nd")
                textArea.readOnly: true
            }

            Item{
                width: parent.width
                height: parent.height - textout.height - 5
                Row{
                    anchors.fill: parent
                    spacing: 10
                    ScrTextA {
                        id:textin
                        color: "#8f654321"
                        width: parent.width - 110 - 10*2 - border.width*2
                        height: parent.height
                        textArea.textFormat: TextEdit.PlainText
                        //textArea.text: qsTr("asfsf\nassg\n2233\nd")
                        //textArea.placeholderText: qsTr("asfsf\nassg\n2233\nd")
                    }

                    Column{
                        width: 100
                        height: parent.height
                        Button {
                            id:btn
                            width: 100
                            height: 30
                            text: "发送"
                            onClicked: {
                                //console.log(textin.textArea.text)
                                textout.textArea.text += qsTr("\n->") + textin.textArea.text + qsTr("\n\n")
                                textout.textArea.forceActiveFocus()
                                textout.textArea.cursorPosition = textout.textArea.length // 将光标定位到文本末尾
                                btn.enabled = false;
                                aiMessage.sendData(checkbox.checked,textkey.text,textin.textArea.text)
                            }
                        }
                        Button {
                            id:btn1
                            width: 100
                            height: 30
                            y:textin.y + (textin.height-height)/2
                            text: "查询余额"
                            onClicked: {
                                //console.log(textin.textArea.text)
                                textout.textArea.text += qsTr("\n->") + "查询余额" + qsTr("\n\n")
                                textout.textArea.forceActiveFocus()
                                textout.textArea.cursorPosition = textout.textArea.length // 将光标定位到文本末尾
                                btn1.enabled = false;
                                aiMessage.queryBalance(textkey.text)
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: aiMessage
        function onRecvMessage(message) {
            //console.log(message)
            textout.textArea.color = "black"
            textout.textArea.text += message
            textout.textArea.forceActiveFocus()
            textout.textArea.cursorPosition = textout.textArea.length // 将光标定位到文本末尾
        }

        function onRecvMessageRole(message){
            // textout.textArea.textFormat = TextEdit.RichText
            // textout.textArea.text += "<font color='green'>" + message + "</font>"
            // textout.textArea.textFormat = TextEdit.PlainText
            textout.textArea.color = "#ffff0000"
            textout.textArea.text += message
            textout.textArea.forceActiveFocus()
            textout.textArea.cursorPosition = textout.textArea.length // 将光标定位到文本末尾
        }

        function onStopRecv(){
            btn.enabled = true;
            btn1.enabled = true;
        }
    }
}
