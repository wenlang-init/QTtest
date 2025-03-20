import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
/* *
    bool filterAble : 是否动态筛选，当为false时用户无法输入且不弹出筛选框
    int indicatorPadding : 下拉箭头距离右边缘
    int popupHeight : 下拉框高度
    color popupColor : 下拉框颜色
    color textColor  : 显示字体颜色
 * */
ComboBox {
    id:comboBox
    width: 90
    height: 36
    leftPadding: 5
    rightPadding: 20
    property bool filterAble : false
    property int indicatorPadding:5
    property int popupHeight: 260
    property color popupColor: "#e63f3f3f"
    property color textColor : "#38d3dc"

    Material.accent: "#38d3dc"
    Material.foreground: "#38d3dc"
    Material.background: "#6638d3dc"
    font.bold: true
    font.pointSize: 9
    font.family: "微软雅黑"
    popup.font.pointSize: 9
    popup.leftPadding: 0
    popup.height: popupHeight
    popup.font.bold: true
    popup.font.family: "微软雅黑"
    popup.background: Rectangle{
        color: popupColor
    }
    contentItem: TextInput {
        id:contentText
        clip: true
        text: comboBox.displayText
        font: comboBox.font
        color: textColor
        verticalAlignment: Text.AlignVCenter
        selectByMouse: true
        readOnly: !filterAble
        onTextEdited:{
            dataFilter(text)
        }
    }
    indicator: Canvas {
        id: canvas
        x: comboBox.width - width - indicatorPadding
        y: comboBox.topPadding + (comboBox.availableHeight - height) / 2
        width: 8
        height: 6
        contextType: "2d"

        Connections {
            target: comboBox
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = textColor;
            context.fill();
        }
    }
    onCurrentTextChanged: {
        contentText.text = currentText
    }


    Popup {
        id: popupFilter
        width: parent.width
        height: popupHeight
        y:parent.height
        background: Rectangle{
            color: popupColor
        }

        contentItem: ListView {
            id: filterListView

            anchors.fill: parent
            clip: true
            highlight: Rectangle {color: "#6638d3dc"; radius: 5 }
            delegate: Item{
                width: popupFilter.width
                height: 30
                clip: true
                Row{
                    anchors.fill: parent
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right:parent.right
                    anchors.rightMargin: 10
                    spacing: 10
                    Text {
                        height:30
                        text: modelData
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 13
                        verticalAlignment: Text.AlignVCenter
                        color: "ghostwhite"
                        font.bold: true
                    }
                }



                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        filterListView.currentIndex = index
                    }

                    onClicked: {
                        var index = comboBox.find(modelData)
                        currentIndex = index
                        popupFilter.close()
                    }
                }
            }
        }
    }

    function dataFilter(filter){
        popupFilter.open()
        var filterList = []
        // for(var i in comboBox.model){
        //     var modelData = comboBox.model[i]
        //     if(String(modelData).indexOf(filter) != -1){
        //         filterList.push(modelData.text)
        //     }
        // }
        for(var i=0;i<comboBox.count;i++){
            var ret = comboBox.textAt(i).indexOf(filter)
            if(ret !== -1){
                filterList.push(comboBox.textAt(i))
            }
        }

        //console.log(filter,filterList)
        filterListView.model = filterList
    }
}
