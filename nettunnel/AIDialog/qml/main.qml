import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    color: "#7f3f0ff1"

    DialogBox {
        opacity: 0.7
        anchors.fill: parent
    }
}
