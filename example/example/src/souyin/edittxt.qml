import QtQuick
import QtQuick.Controls

Item {
    // anchors.fill: parent
    property alias text: textArea.text

    function setText(txt) {
        textArea.text = txt
    }
    function getText() {
        return textArea.text
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: textArea.width
        contentHeight: textArea.height
        clip: true
        //opacity: 0.2

        TextArea.flickable:
            TextArea {
            id: textArea
            //text: "这是一个测试文本，用于演示滚动功能。\n".repeat(30)
            wrapMode: TextArea .Wrap // 换行
            //horizontalAlignment: Text.AlignHCenter
            //verticalAlignment: Text.AlignVCenter
            //transformOrigin: Item.Center
            selectByMouse: true
            mouseSelectionMode: TextEdit.SelectCharacters
            // 暗色主题样式
            color: "#e0e0e0"
            selectionColor: "#4CAF50"
            selectedTextColor: "#ffffff"
            placeholderTextColor: "#808080"

            background: Rectangle {
                color: "#333333"
                border.color: textArea.activeFocus ? "#4CAF50" : "#555555"
                border.width: textArea.activeFocus ? 2 : 1
                radius: 4
            }
        }
        ScrollBar.vertical: ScrollBar {
            id: vScrollBar
            policy: ScrollBar.AsNeeded // 内容超出时显示
            active: hovered || pressed // 悬停或按下时可见
            background: Rectangle {
                color: vScrollBar.activeFocus ? "#4CAF50" : "#555555"
                border.color: vScrollBar.activeFocus ? "#4CAF50" : "#555555"
                //color:  "#4CAF50"
                //border.color:"#555555"
                border.width: vScrollBar.activeFocus ? 2 : 1
                radius: 4
            }
        }
    }
}
