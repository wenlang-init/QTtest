import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import "./"

Item {
    VideoPlayer {
        anchors.fill: parent
        id:videoplayer
        onClosePlayer: {
            qmlWidget.closeshow()
        }
    }
}
