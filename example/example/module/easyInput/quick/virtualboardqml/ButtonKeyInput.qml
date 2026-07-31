import QtQuick

Item {
    id:buttonkeyinputroot

    property alias rectkey: rectid
    property alias textkey: textid
    property alias mousekey: mouseid

    Rectangle {
        id:rectid
        anchors.fill: parent
        color: "#ffffff"
        radius: 5
        border.width: 0
        Text{
            id:textid
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#000000"
            text: qsTr("")
            MouseArea{
                id:mouseid
                anchors.fill: parent
//                onClicked: {
//                    //console.log(textkey.text)
//                    virtualkeyinputroot.click_key_resolving(textkey.text)
//                }
                onPressed: {
                    rectid.color = "#cfcfcf"
                }
                onReleased: {
                    rectid.color = "#ffffff"
                }
            }
        }
    }
}
