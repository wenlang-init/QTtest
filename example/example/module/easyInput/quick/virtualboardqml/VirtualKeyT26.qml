import QtQuick

Item {
    id:virtualkeyT26
    anchors.fill: parent

    property int key_model:0 // 键盘模式:0中文，1英文，2英文大写,3数字

    property string input_value:""

    signal out_code_data(string value)
    signal exit_T26()
    signal t26_key_signal(string value)

    Component.onCompleted: {
        virtualsymbolleft.listModelappend("英文")
        virtualsymbolleft.listModelappend("中文")
        virtualsymbolleft.listModelappend("网络")
        virtualsymbolleft.listModelappend("邮箱")

        virtualsymbol1.listModelappend(".","@","~","-")
        virtualsymbol1.listModelappend(",",":","*","?")
        virtualsymbol1.listModelappend("!","_","#","/")
        virtualsymbol1.listModelappend("=","+","^","&")
        virtualsymbol1.listModelappend(";","%","$","￥")
        virtualsymbol1.listModelappend("(",")","\\","<")
        virtualsymbol1.listModelappend(">","|","[","]")
        virtualsymbol1.listModelappend("\"","{","}","¥")

        virtualsymbol2.listModelappend("，","。","？","！")
        virtualsymbol2.listModelappend("~","、","：","＇")
        virtualsymbol2.listModelappend("；","‘","’","——")
        virtualsymbol2.listModelappend("……","（","）","！")
        virtualsymbol2.listModelappend("【","】","《","》")

        virtualsymbol3.listModelappend("@","-","_","/")
        virtualsymbol3.listModelappend("wap.","bbs.",".net",".org")
        virtualsymbol3.listModelappend(".com","www.","http://",".cn")
        virtualsymbol3.listModelappend("news.","blog.","ftp://",".中国")

        virtualsymbol4.listModelappend(".","/","-","_")
        virtualsymbol4.listModelappend(".com",".cn",".net",".org")
        virtualsymbol4.listModelappend("@","163","qq","sina")
        virtualsymbol4.listModelappend("126","msn","sohu","gmail")


        chinese_top_show_status(2)
    }

    // 设置显示界面显示为T26初始界面
    function set_init_show_T26(){
        show_info_list(4)
        show_info_list(1)
    }

    // 中文输入时上面显示状态
    function chinese_top_show_status(value){
        if(value === 1){
            textinput.visible = true
            rect1.visible = true
        } else {
            textinput.visible = false
            rect1.visible = false
        }
    }

    // 清空显示的输入字符
    function clean_top_text(){
        text1.text = ""
        text2.text = ""
        text3.text = ""
        text4.text = ""
        text5.text = ""
    }
    // 设置上面显示查找的汉字结果
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
    // 根据按键输入使用谷歌拼音引擎查询字库
    function click_key_resolving(value){
        clean_top_text()
        if(key_model == 0){
            if(value.length === 1){
                if(value >= 'a' && value <= 'z'){
                    input_value += value
                    chinese_top_show_status(1)
                } else {
                    chinese_top_show_status(2)
                    input_value = ""
                    var tmp = value
                    if(value === qsTr(",")){
                        tmp = "，"
                    } else if(value === qsTr(".")){
                        tmp = "。"
                    }
                    out_value(tmp)
                }
            } else {
                if(value === "delete"){
                    if(input_value.length > 0){
                        input_value = input_value.substring(0,input_value.length-1)
                    } else {
                        t26_key_signal("delete")
                        chinese_top_show_status(2)
                    }
                } else {
                    chinese_top_show_status(2)
                   input_value = ""
                    if(value === qsTr("space")){
                        out_value(" ")
                    }
                }
            }
            textinput.text = input_value
            //console.log(value);console.log(input_value)
            var data = InputQML.set_input_value(input_value)
            set_top_text(data)
        } else {
            chinese_top_show_status(2)
            input_value = ""
            var tmp1 = value
            if(key_model == 2){
                if(value.length === 1 && (value >= 'a' && value <= 'z')){
                    switch(value){
                    case 'a':
                        tmp1 = 'A'
                        break
                    case 'b':
                        tmp1 = 'B'
                        break
                    case 'c':
                        tmp1 = 'C'
                        break
                    case 'd':
                        tmp1 = 'D'
                        break
                    case 'e':
                        tmp1 = 'E'
                        break
                    case 'f':
                        tmp1 = 'F'
                        break
                    case 'g':
                        tmp1 = 'G'
                        break
                    case 'h':
                        tmp1 = 'H'
                        break
                    case 'i':
                        tmp1 = 'I'
                        break
                    case 'j':
                        tmp1 = 'J'
                        break
                    case 'k':
                        tmp1 = 'K'
                        break
                    case 'k':
                        tmp1 = 'L'
                        break
                    case 'm':
                        tmp1 = 'M'
                        break
                    case 'n':
                        tmp1 = 'N'
                        break
                    case 'o':
                        tmp1 = 'O'
                        break
                    case 'p':
                        tmp1 = 'P'
                        break
                    case 'q':
                        tmp1 = 'Q'
                        break
                    case 'r':
                        tmp1 = 'R'
                        break
                    case 's':
                        tmp1 = 'S'
                        break
                    case 't':
                        tmp1 = 'T'
                        break
                    case 'u':
                        tmp1 = 'U'
                        break
                    case 'v':
                        tmp1 = 'V'
                        break
                    case 'w':
                        tmp1 = 'W'
                        break
                    case 'x':
                        tmp1 = 'X'
                        break
                    case 'y':
                        tmp1 = 'Y'
                        break
                    case 'z':
                        tmp1 = 'Z'
                        break
                    default:
                        break
                    }
                }
            }
            if(value === "delete"){
                t26_key_signal("delete")
                return
            }

            if(value === qsTr("space")){
                tmp1 = " "
            }
            out_value(tmp1)
        }
    }
    // 输出选择的汉字
    function output_data(key,value){
        //console.log(key+":"+value)
        var tmpp = value
        if(key === qsTr("enter")){
            tmpp = input_value
        }
        //console.log(tmp)
        virtualkeyT26.out_code_data(tmpp)
        input_value = ""
        textinput.text = ""
        clean_top_text()
    }
    // 直接输出,并清除输入的记录
    function out_value(value){
        virtualkeyT26.out_code_data(value)
        input_value = ""
        textinput.text = ""
        clean_top_text()
    }

    // 解析显示所有汉字选项
    function all_code_resolving(){
        var data = InputQML.get_all_icodedata()
        //console.log(data)
        if(input_value != ""){
            virtualmore.clean_all_data()
            virtualmoreleft.clean_all_data()
            if(rect3.visible == true){
                for(var i=0;i<data.length;i++){
                    virtualmore.listModelappend(data[i])
                }
                // 左侧pingyin
                // for(var j=0;j<input_value.length;j++){
                //     virtualmoreleft.listModelappend(input_value[j])
                // }
                virtualmoreleft.listModelappend(input_value)

                show_info_list(2)
            } else {
                show_info_list(1)
            }
        }
    }

    // 显示项
    function show_info_list(value){
        rect3.visible = false
        rectmore.visible = false
        rectsymbol.visible = false
        rectnumber.visible = false
        rectT9.visible = false

        switch(value){
        case 1:
//            key_model = 0
//            buttonkey32.textkey.text = qsTr("中")
//            input_value = ""
//            textinput.text = ""
//            clean_top_text()
//            image1.source = "qrc:/picture/upper_1.png"

            rect3.visible = true
            break
        case 2:
            rectmore.visible = true
            break
        case 3:
            chinese_top_show_status(2)
            rectsymbol.visible = true
            break
        case 4:
            key_model = 0
            input_value = ""
            textinput.text = ""
            clean_top_text()
            chinese_top_show_status(2)

            rectnumber.visible = true
            break
        case 5:
            rectT9.visible = true
            break
        default:
//            key_model = 0
//            buttonkey32.textkey.text = qsTr("中")
//            input_value = ""
//            textinput.text = ""
//            clean_top_text()
//            image1.source = "qrc:/picture/upper_1.png"

            rect3.visible = true
            break
        }
    }

    // 拼音输入显示
    Text {
        id: textinput
        width: parent.width
        height: 30
        anchors.bottom: rectT26.top
        font.pointSize: 8
        font.family: "黑体"
        //visible: false
        text: qsTr("")
    }
    // T26布局
    Rectangle{
        id:rectT26
        anchors.fill: parent
        //color: "#7f7f7f"
        color: Qt.rgba(0x7f,0x7f,0x7f,0)
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
                    font.pointSize: 8
                    font.family: "黑体"
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(text1.text != ""){
                                //console.log(text1.text)
                                chinese_top_show_status(0)
                                output_data(qsTr("text1"),qsTr(text1.text))
                            }
                        }
                    }
                }
                Text {
                    id: text2
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: text1.font
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(text2.text != ""){
                                //console.log(text2.text)
                                chinese_top_show_status(0)
                                output_data(qsTr("text2"),qsTr(text2.text))
                            }
                        }
                    }
                }
                Text {
                    id: text3
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: text1.font
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(text3.text != ""){
                                //console.log(text3.text)
                                chinese_top_show_status(0)
                                output_data(qsTr("text3"),qsTr(text3.text))
                            }
                        }
                    }
                }
                Text {
                    id: text4
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: text1.font
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(text4.text != ""){
                                //console.log(text4.text)
                                chinese_top_show_status(0)
                                output_data(qsTr("text4"),qsTr(text4.text))
                            }
                        }
                    }
                }
                Text {
                    id: text5
                    width: text1.width
                    height: text1.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: text1.font
                    text: qsTr("")
                    color: "#000000"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(text5.text != ""){
                                //console.log(text5.text)
                                chinese_top_show_status(0)
                                output_data(qsTr("text5"),qsTr(text5.text))
                            }
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
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("imagetext ........")
                            all_code_resolving()
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
                    }
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
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("imagefunction2 ........")
                            if(rectT9.visible == false){
                                key_model = 0
                                input_value = ""
                                textinput.text = ""
                                clean_top_text()
                                show_info_list(5)
                            } else {
                                key_model = 0
                                buttonkey32.textkey.text = qsTr("中")
                                input_value = ""
                                textinput.text = ""
                                clean_top_text()
                                image1.source = "qrc:/picture/upper_1.png"
                                show_info_list(1)
                            }
                        }
                        onPressed: {
                            buttonkeyfunction2.rectkey.color = "#cfcfcf"
                        }
                        onReleased: {
                            buttonkeyfunction2.rectkey.color = "#ffffff"
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
                    }
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
                    }
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
                    }
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
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("imagefunction6 ........")
                            input_value = ""
                            textinput.text = ""
                            clean_top_text()
                            show_info_list(1)
                            chinese_top_show_status(2)
                            exit_T26()
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
        // 按键输入框 50*4 26键
        Rectangle {
            id:rect3
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: 200
            width: parent.width
            color: "#7f7f7f"
            visible: true
            // 第一行
            Row{
                id:row1
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.top: parent.top
                anchors.topMargin: 2
                width: parent.width
                height: parent.height/4 - 4
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey1
                    width: parent.width/10 - 4
                    height: parent.height
                    textkey.text: qsTr("q")
                    mousekey.onClicked: {
                        //console.log(textkey.text)
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey2
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("w")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey3
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("e")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey4
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("r")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey5
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("t")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey6
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("y")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey7
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("u")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey8
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("i")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey9
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("o")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey10
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("p")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第二行
            Row{
                id:row2
                anchors.left: parent.left
                anchors.leftMargin: 2 + buttonkey1.width/2
                anchors.rightMargin: 2 + buttonkey1.width/2
                anchors.top: row1.bottom
                anchors.topMargin: 4
                width: row1.width
                height: row1.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey11
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("a")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey12
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("s")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey13
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("d")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey14
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("f")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey15
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("g")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey16
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("h")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey17
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("j")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey18
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("k")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey19
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("l")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
            }
            // 第3行
            Row{
                id:row3
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.top: row2.bottom
                anchors.topMargin: 4
                width: row1.width
                height: row1.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey101
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    mousekey.enabled: false
                    Image {
                        id: image1
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/upper_1.png"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("image1 ........")
                            if(key_model == 1){
                                key_model = 2
                                image1.source = "qrc:/picture/upper_2.png"
                            } else if(key_model == 2){
                                key_model = 1
                                image1.source = "qrc:/picture/upper_1.png"
                            }
                        }
                        onPressed: {
                            if(key_model == 1 || key_model == 2){
                                buttonkey101.rectkey.color = "#cfcfcf"
                            }
                        }
                        onReleased: {
                            if(key_model == 1 || key_model == 2){
                                buttonkey101.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkey20
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("z")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey21
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("x")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey22
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("c")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey23
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("v")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey24
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("b")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey25
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("n")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey26
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr("m")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey102
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    mousekey.enabled: false
                    Image {
                        id: image2
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/delete_white.png"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("image2 ........")
                            click_key_resolving("delete")
                        }
                        onPressed: {
                            buttonkey102.rectkey.color = "#cfcfcf"
                        }
                        onReleased: {
                            buttonkey102.rectkey.color = "#ffffff"
                        }
                    }
                }
            }
            // 第4行
            Row{
                id:row4
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.top: row3.bottom
                anchors.topMargin: 4
                width: row1.width
                height: row1.height
                spacing: 4
                ButtonKeyInput{
                    id:buttonkey27
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    textkey.text: qsTr("符")
                    mousekey.onClicked: {
                        if(rectsymbol.visible == true){
                            show_info_list(1)
                        } else {
                            show_info_list(3)
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkey28
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    textkey.text: qsTr("123")
                    mousekey.onClicked: {
                        if(rectnumber.visible == true){
                            key_model = 0
                            show_info_list(1)
                        } else {
                            key_model = 3
                            show_info_list(4)
                        }
                    }
                }
                ButtonKeyInput{
                    id:buttonkey29
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr(",")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey30
                    width: buttonkey1.width*2
                    height: buttonkey1.height
                    mousekey.enabled: false
                    Image {
                        id: imagespace
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/space.png"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            click_key_resolving("space")
                        }
                        onPressed: {
                            buttonkey30.rectkey.color = "#cfcfcf"
                        }
                        onReleased: {
                            buttonkey30.rectkey.color = "#ffffff"
                        }
                    }

//                    textkey.text: qsTr("space")
//                    mousekey.onClicked: {
//                        click_key_resolving("space")
//                    }
                }
                ButtonKeyInput{
                    id:buttonkey31
                    width: buttonkey1.width
                    height: buttonkey1.height
                    textkey.text: qsTr(".")
                    mousekey.onClicked: {
                        click_key_resolving(textkey.text)
                    }
                }
                ButtonKeyInput{
                    id:buttonkey32
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    textkey.text: qsTr("中")
                    mousekey.onClicked: {
                        if(key_model != 0){
                            key_model = 0
                            textkey.text = qsTr("中")
                        } else {
                            key_model = 1
                            textkey.text = qsTr("英")
                        }
                        input_value = ""
                        textinput.text = ""
                        clean_top_text()
                        chinese_top_show_status(2)
                        image1.source = "qrc:/picture/upper_1.png"
                    }
                }
                ButtonKeyInput{
                    id:buttonkey103
                    width: buttonkey1.width*1.5
                    height: buttonkey1.height
                    mousekey.enabled: false
                    Image {
                        id: image3
                        width: parent.width/2
                        height: parent.height/2
                        anchors.top: parent.top
                        anchors.topMargin: parent.height/4
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width/4
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/picture/enter.png"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //console.log("image3 ........")
                            output_data(qsTr("enter"),qsTr("imagetext"))
                            chinese_top_show_status(2)
                        }
                        onPressed: {
                            buttonkey103.rectkey.color = "#cfcfcf"
                        }
                        onReleased: {
                            buttonkey103.rectkey.color = "#ffffff"
                        }
                    }
                }
            }
        }
        // 汉字选择下拉扩展
        Rectangle{
            id:rectmore
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: rect3.height
            width: rect3.width
            color: "#7f7f7f"
            visible: false
            Rectangle{
                id:rectmoreleft
                width: parent.width/8
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 2
                radius: 5
                color: "#7f7f7f"
                Rectangle{
                    id:rectmoreleft1
                    width:parent.width
                    height: parent.height/5*4
                    anchors.top: parent.top
                    anchors.topMargin: 2
                    radius: 5
                    VirtualKeyMore{
                        id:virtualmoreleft
                        radius: 5
                        onClick_list_value: function(value){
                            console.log(value)
                        }
                    }
                }
                Rectangle{
                    id:rectmoreleft2
                    width:parent.width
                    height: parent.height/5 - 8
                    anchors.top: rectmoreleft1.bottom
                    anchors.topMargin: 4
                    radius: 5
                    color: "#ffffff"
                    Text {
                        id: textleftre
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: qsTr("返回")
                        font.pointSize: 8;font.family: "黑体"
                        color: "#000000"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                //console.log("return")
                                show_info_list(1)
                            }
                            onPressed: {
                                rectmoreleft2.color = "#cfcfcf"
                            }
                            onReleased: {
                                rectmoreleft2.color = "#ffffff"
                            }
                        }
                    }
                }
            }
            Rectangle{
                height: rectmore.height
                width: parent.width - rectmoreleft.width - 8
                anchors.left: rectmoreleft.right
                anchors.leftMargin: 4
                color: "#7f7f7f"

                VirtualKeyMore{
                    id:virtualmore
                    onClick_list_value: function(value){
                        //console.log(value)
                        show_info_list(1)
                        output_data("vmore",value)
                    }
                }
            }
        }
        // 字符框
        Rectangle{
            id:rectsymbol
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: rect3.height
            width: rect3.width
            color: "#7f7f7f"
            visible: false
            Rectangle{
                id:rectsymbolleft
                width: parent.width/8
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 2
                radius: 5
                color: "#7f7f7f"
                Rectangle{
                    id:rectsynbolleft1
                    width:parent.width
                    height: parent.height/5*4
                    anchors.top: parent.top
                    anchors.topMargin: 2
                    radius: 5
                    VirtualKeyMore{
                        id:virtualsymbolleft
                        radius: 5
                        onClick_list_value: {
                            //console.log(value)
                            rectsymballisthead1.visible = false
                            rectsymballisthead2.visible = false
                            rectsymballisthead3.visible = false
                            rectsymballisthead4.visible = false
                            if(value === qsTr("英文")){
                                rectsymballisthead1.visible = true
                            } else if(value === qsTr("中文")){
                                rectsymballisthead2.visible = true
                            } else if(value === qsTr("网络")){
                                rectsymballisthead3.visible = true
                            } else if(value === qsTr("邮箱")){
                                rectsymballisthead4.visible = true
                            }
                        }
                    }
                }
                Rectangle{
                    id:rectsymbolleftr
                    width:parent.width
                    height: parent.height/5 - 8
                    anchors.top: rectsynbolleft1.bottom
                    anchors.topMargin: 4
                    radius: 5
                    color: "#ffffff"
                    Text {
                        id: textsymbolleftre
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: qsTr("返回");
                        font.pointSize: 8;font.family: "黑体"
                        color: "#000000"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                //console.log("return")
                                show_info_list(1)
                            }
                            onPressed: {
                                rectsymbolleftr.color = "#cfcfcf"
                            }
                            onReleased: {
                                rectsymbolleftr.color = "#ffffff"
                            }
                        }
                    }
                }
            }
            // 英文
            Rectangle{
                id:rectsymballisthead1
                height: rectsymbol.height
                width: parent.width - rectsymbolleft.width - 8
                anchors.left: rectsymbolleft.right
                anchors.leftMargin: 4
                color: "#7f7f7f"
                visible: true
                VirtualKeySymbol{
                    id:virtualsymbol1
                    onClick_list_value: {
                        //console.log(value)
                        out_value(value)
                    }
                }
            }
            // 中文
            Rectangle{
                id:rectsymballisthead2
                height: rectsymballisthead1.height
                width: rectsymballisthead1.width
                anchors.left: rectsymballisthead1.left
                anchors.leftMargin: 4
                color: "#7f7f7f"
                visible: false
                VirtualKeySymbol{
                    id:virtualsymbol2
                    onClick_list_value: {
                        //console.log(value)
                        out_value(value)
                    }
                }
            }
            // 网络
            Rectangle{
                id:rectsymballisthead3
                height: rectsymballisthead1.height
                width: rectsymballisthead1.width
                anchors.left: rectsymballisthead1.left
                anchors.leftMargin: 4
                color: "#7f7f7f"
                visible: false
                VirtualKeySymbol{
                    id:virtualsymbol3
                    onClick_list_value: {
                        //console.log(value)
                        out_value(value)
                    }
                }
            }
            // 邮箱
            Rectangle{
                id:rectsymballisthead4
                height: rectsymballisthead1.height
                width: rectsymballisthead1.width
                anchors.left: rectsymballisthead1.left
                anchors.leftMargin: 4
                color: "#7f7f7f"
                visible: false
                VirtualKeySymbol{
                    id:virtualsymbol4
                    onClick_list_value: {
                        //console.log(value)
                        out_value(value)
                    }
                }
            }
        }
        // 数字输入框
        Rectangle{
            id:rectnumber
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: rect3.height
            width: rect3.width
            color: "#7f7f7f"
            visible: false
            Column{
                anchors.fill: parent
                spacing: 4
                Row{
                    id:rownumber1
                    width: parent.width
                    height: parent.height/4 - 4
                    spacing: 4
                    ButtonKeyInput{
                        id:buttonkeynumber1
                        width: parent.width/4 - 4
                        height: parent.height
                        textkey.text: qsTr("1")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber2
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("2")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber3
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("3")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber4
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        Image {
                            id: imagenumber1
                            width: parent.width/2
                            height: parent.height/2
                            anchors.top: parent.top
                            anchors.topMargin: parent.height/4
                            anchors.left: parent.left
                            anchors.leftMargin: parent.width/4
                            fillMode: Image.PreserveAspectFit // 图像按照比例缩放，不裁剪
                            source: "qrc:/picture/delete_white.png"
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                //console.log("imagenumber1 ........")
                                click_key_resolving("delete")
                            }
                            onPressed: {
                                buttonkeynumber4.rectkey.color = "#cfcfcf"
                            }
                            onReleased: {
                                buttonkeynumber4.rectkey.color = "#ffffff"
                            }
                        }
                    }
                }
                Row{
                    id:rownumber2
                    width: rownumber1.width
                    height: rownumber1.height
                    spacing: 4
                    ButtonKeyInput{
                        id:buttonkeynumber5
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("4")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber6
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("5")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber7
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("6")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber8
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("*")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                }
                Row{
                    id:rownumber3
                    width: rownumber1.width
                    height: rownumber1.height
                    spacing: 4
                    ButtonKeyInput{
                        id:buttonkeynumber9
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("7")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber10
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("8")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber11
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("9")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber12
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("#")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                }
                Row{
                    id:rownumber4
                    width: rownumber1.width
                    height: rownumber1.height
                    spacing: 4
                    ButtonKeyInput{
                        id:buttonkeynumber13
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("返回")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            key_model = 0
                            buttonkey32.textkey.text = qsTr("中")
                            input_value = ""
                            textinput.text = ""
                            clean_top_text()
                            image1.source = "qrc:/picture/upper_1.png"

                            show_info_list(1)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber14
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("0")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber15
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr("空格")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving("space")
                        }
                    }
                    ButtonKeyInput{
                        id:buttonkeynumber16
                        width: buttonkeynumber1.width
                        height: buttonkeynumber1.height
                        textkey.text: qsTr(".")
                        mousekey.onClicked: {
                            //console.log(textkey.text)
                            click_key_resolving(textkey.text)
                        }
                    }
                }
            }
        }
        // T9中文键盘
        Rectangle{
            id:rectT9
            anchors.top: rect2.bottom
            anchors.left: parent.left
            height: rect3.height
            width: rect3.width
            color: "#7f7f7f"
            visible: false
        }
    }
}
