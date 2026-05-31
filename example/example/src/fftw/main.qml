import QtQuick
import QtQuick.Window

Window{
    id: rootWindow
    width: 640
    height: 480
    visible: true // 让窗口可见
    title: "我的第一个QML应用"

    Rectangle {
        id: clickableBox
        width: 150
        height: 150
        color: "steelblue"

        anchors.centerIn: parent

        MouseArea {
            anchors.fill: parent

            onClicked: {
                clickableBox.color = (clickableBox.color == "steelblue" ? "tomato" : "steelblue")
            }
        }
    }
}
