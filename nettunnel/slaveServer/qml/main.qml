import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
//import QmlInTurn 1.0

Window {
    id:root
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    color:"#4ff199"

    property bool isturnserver: true

    // QmlInTurn{
    //     id:qmlinturn
    //     onRecvTextChanged:{
    //         console.log("cc")
    //     }
    // }

    Image {
        anchors.fill: parent
        id: backgroundimage
        source: "qrc:/images/Background.png"
    }

    Button {
        id:buttonroot
        width: parent.width
        height: 30
        x: 0
        y: 0
        text: "切换"
        font.bold: true
        icon.color: "#271268"
        onClicked: {
            if(isturnserver){
                turnserver.visible = false
                weather.visible = true
                isturnserver = false
            } else {
                turnserver.visible = true
                weather.visible = false
                isturnserver = true
            }
        }
    }

    TurnServer {
        id:turnserver
        anchors.top: buttonroot.bottom
        anchors.left: buttonroot.left
        width: parent.width
        height: parent.height - 30
        opacity: 0.8
        visible: true
    }

    Weather {
        id:weather
        anchors.top: buttonroot.bottom
        anchors.left: buttonroot.left
        width: parent.width
        height: parent.height - 30
        opacity: 0.8
        visible: false
    }
}
