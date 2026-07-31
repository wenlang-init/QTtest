import QtQuick

Item {
    id:virtualkeyT9
    anchors.fill: parent

    property int key_model:0 // 键盘模式:0中文，1英文，2英文大写，3数字

    function set_top_text(array_data){
        if(array_data.length > 5){
            console.log("array data length:"+array_data.length)
        } else {
            for(var i=0;i<array_data.length;i++){
                switch(i){
                case 0:
                    text1.text = array_data[i]
                    break
                case 1:
                    text2.text = array_data[i]
                    break
                case 2:
                    text3.text = array_data[i]
                    break
                case 3:
                    text4.text = array_data[i]
                    break
                case 4:
                    text5.text = array_data[i]
                    break
                default:
                    break
                }
            }
        }
    }

    function click_key_resolving(value){
        console.log(value)
        var data = InputQML.set_input_value(value)
        set_top_text(data)
        switch(value){
        case "a":
            break
        default:
            break
        }
    }

    // T9布局
    Rectangle{
        id:rectT26
        anchors.fill: parent
        color: "#7f7f7f"
        // 文字待选框 50
        Rectangle {
            id:rect1
            anchors.top: parent.top
            anchors.left: parent.left
            height: 50
            width: parent.width
            color: "#7f7f7f"
            Row{
                id:rowtext
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.top: parent.top
                anchors.topMargin: 2
                width: parent.width
                height: parent.height - 4
                spacing: 4
                Text {
                    id: text1
                    width: parent.width/6 - 4
                    height: parent.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(text1.text)
                        }
                    }
                }
                Text {
                    id: text2
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(text2.text)
                        }
                    }
                }
                Text {
                    id: text3
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(text3.text)
                        }
                    }
                }
                Text {
                    id: text4
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(text4.text)
                        }
                    }
                }
                Text {
                    id: text5
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(text5.text)
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyimagetext
                    width: text1.width
                    height: text1.height
                    rectkey.color:"#7f7f7f"
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagetext
                        //anchors.fill: parent
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit // 图像按照比例缩放，不裁剪
                        source: "qrc:/picture/moreb.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagetext ........")
                            }
                            onPressed: {
                                buttonkeyimagetext.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyimagetext.rectkey.color = "#7f7f7f"
                            }
                        }
                    }
                }
            }
        }
        // 功能框 50
        Rectangle {
            id:rect2
            anchors.top: rect1.bottom
            anchors.left: parent.left
            height: 50
            width: parent.width
            color: "#efefef"
            Row{
                id:rowfunction
                anchors.left: parent.left
                anchors.leftMargin: 3
                anchors.rightMargin: 3
                anchors.top: parent.top
                anchors.topMargin: 2
                width: parent.width
                height: parent.height - 4
                spacing: 0
                ButtonKeyInput{
                    id:buttonkeyfunction1
                    width: parent.width/6 - 1
                    height: parent.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction1
                        //anchors.fill: parent
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit // 图像按照比例缩放，不裁剪
                        source: "qrc:/picture/set.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction1 ........")
                            }
                            onPressed: {
                                buttonkeyfunction1.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction1.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyfunction2
                    width: buttonkeyfunction1.width
                    height: buttonkeyfunction1.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction2
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/multimedia.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction2 ........")
                            }
                            onPressed: {
                                buttonkeyfunction2.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction2.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyfunction3
                    width: buttonkeyfunction1.width
                    height: buttonkeyfunction1.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction3
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/microphone.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction3 ........")
                            }
                            onPressed: {
                                buttonkeyfunction3.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction3.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyfunction4
                    width: buttonkeyfunction1.width
                    height: buttonkeyfunction1.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction4
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/key_check.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction4 ........")
                            }
                            onPressed: {
                                buttonkeyfunction4.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction4.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyfunction5
                    width: buttonkeyfunction1.width
                    height: buttonkeyfunction1.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction5
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/emoji.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction5 ........")
                            }
                            onPressed: {
                                buttonkeyfunction5.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction5.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkeyfunction6
                    width: buttonkeyfunction1.width
                    height: buttonkeyfunction1.height
                    rectkey.radius: 0
                    mousekey.enabled: false
                    Image {
                        id: imagefunction6
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/hid.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("imagefunction6 ........")
                            }
                            onPressed: {
                                buttonkeyfunction6.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeyfunction6.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
            }
        }
        // 按键输入框 50*4 9键
        Rectangle {
            id:rect3
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: 200
            width: parent.width
            color: "#7f7f7f"
            // 第一列
            Column{
                id:column1
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.top: parent.top
                anchors.topMargin: 2
                width: parent.width/5 - 4
                height: parent.height
                spacing: 0
                ButtonKeyInput{
                    id:buttonkey1
                    width: parent.width
                    height: parent.height/5
                    rectkey.radius: 0
                    textkey.text: qsTr("，")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey2
                    width: buttonkey1.width
                    height: buttonkey1.height
                    rectkey.radius: 0
                    textkey.text: qsTr("。")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey3
                    width: buttonkey1.width
                    height: buttonkey1.height
                    rectkey.radius: 0
                    textkey.text: qsTr("？")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey4
                    width: buttonkey1.width
                    height: buttonkey1.height
                    rectkey.radius: 0
                    textkey.text: qsTr("！")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey5
                    width: buttonkey1.width
                    height: buttonkey1.height
                    rectkey.radius: 0
                    textkey.text: qsTr("...")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第2列
            Column{
                id:column2
                anchors.left: column1.right
                anchors.leftMargin: 2
                anchors.top: column1.top
                anchors.topMargin: 2
                width: column1.width
                height: parent.height - 4
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey6
                    width: parent.width
                    height: parent.height/4 - 4
                    textkey.text: qsTr("1\n分词")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey7
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("4\nGHI")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey8
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("7\nPQRS")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey9
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("123")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第3列
            Column{
                id:column3
                anchors.left: column2.right
                anchors.leftMargin: 2
                anchors.top: column2.top
                anchors.topMargin: 2
                width: column2.width
                height: column2.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey10
                    width: parent.width
                    height: parent.height/4 - 4
                    textkey.text: qsTr("2\nABC")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey11
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("5\nJKL")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey12
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("8\nTUV")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey13
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("space")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第4列
            Column{
                id:column4
                anchors.left: column3.right
                anchors.leftMargin: 2
                anchors.top: column2.top
                anchors.topMargin: 2
                width: column2.width
                height: column2.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey14
                    width: parent.width
                    height: parent.height/4 - 4
                    textkey.text: qsTr("3\nDEF")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey15
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("6\nMNO")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey16
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("9\nWXYZ")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey17
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("中/英")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第5列
            Column{
                id:column5
                anchors.left: column4.right
                anchors.leftMargin: 2
                anchors.top: column2.top
                anchors.topMargin: 2
                width: column2.width
                height: column2.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey18
                    width: parent.width
                    height: parent.height/4 - 4
                    textkey.text: qsTr("clean")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey19
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("ab")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey20
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("123")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey21
                    width: parent.width
                    height: buttonkey6.height
                    textkey.text: qsTr("enter")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
            }
        }
    }
}
