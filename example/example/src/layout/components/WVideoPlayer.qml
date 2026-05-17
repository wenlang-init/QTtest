pragma ComponentBehavior: Bound
import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import QtQuick.Controls.Fusion
import QtQuick.Dialogs
import QtCore

Item {
    function getTime(time : int) : string {
        const h = Math.floor(time / 3600000).toString()
        const m = Math.floor(time / 60000).toString()
        const s = Math.floor(time / 1000 - m * 60).toString()
        return `${h.padStart(2,'0')}:${m.padStart(2,'0')}:${s.padStart(2, '0')}`
    }

    signal sigfullScreen()

    id:m_root
    //width: parent.width
    //height: 600
    //color: "transparent"
    //focus: true
    //Keys.forwardTo:{}

    MediaPlayer {
        id: mediaPlayer
        //playbackRate:false
        source: new URL("file:///D:/杂项/视频/火影忍者改/火影忍者卷36-第十班.mp4")
        videoOutput: videoOutput
        audioOutput: AudioOutput {
            id: audio
            //muted: false
            volume: slidervolume.value
        }
        onMetaDataChanged : {
            metadataList.model.clear()

            if (mediaPlayer.metaData) {
                for (var key of mediaPlayer.metaData.keys()) {
                    if (mediaPlayer.metaData.stringValue(key)) {
                        metadataList.model.append({
                            name: mediaPlayer.metaData.metaDataKeyToString(key),
                            value: mediaPlayer.metaData.stringValue(key)
                        })
                    }
                }
            }
        }

        onPositionChanged:{
            slidervideo.value = mediaPlayer.position
        }
    }
    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        visible: true //mediaPlayer.mediaStatus > 0

        TapHandler {
            property bool isshowmore: true
            // 使用 | (按位或) 组合多个按钮
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onDoubleTapped: {
                //root.fullScreen ?  root.showNormal() : root.showFullScreen()
                //root.fullScreen = !root.fullScreen
                if(isshowmore){
                    slidervolume.visible = false
                    itemvideo.visible = false
                } else {
                    slidervolume.visible = true
                    itemvideo.visible = true
                }
                isshowmore = !isshowmore
            }

            onTapped: (eventPoint, button)=> {
                          // console.log("tapped",eventPoint.device.name,
                          //     "button", button,"@",eventPoint.scenePosition)
                          if (button === Qt.RightButton) {
                              menu.popup()
                          }
                          videoOutput.focus=true
                      }
        }

        Keys.onPressed: (event) => {
            //event.accepted = true
            if (event.key === Qt.Key_Left) {
                if(mediaPlayer.position - 5000 > 0)
                   mediaPlayer.position -= 5000
                else
                   mediaPlayer.position = 0
            }
            if (event.key === Qt.Key_Right) {
                if(mediaPlayer.position + 5000 < mediaPlayer.duration)
                   mediaPlayer.position += 5000
                else
                   mediaPlayer.position = mediaPlayer.duration
            }
            if (event.key === Qt.Key_Up) {
                var step = (slidervolume.to - slidervolume.from)/10
                if(slidervolume.value + step < slidervolume.to)
                    slidervolume.value += step
                else
                    slidervolume.value = slidervolume.to
            }
            if (event.key === Qt.Key_Down) {
                var step = (slidervolume.to - slidervolume.from)/10
                if(slidervolume.value - step > slidervolume.from)
                    slidervolume.value -= step
                else
                    slidervolume.value = slidervolume.from
            }
            if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                m_root.sigfullScreen()
            }
        }
    }

    Menu {
        id: menu
        MenuItem {
            text: qsTr("打开")
            onTriggered: {
                fileDialog.open()
            }
        }
        MenuItem {
            text: qsTr("视频信息")
            onTriggered: {
                infoPopup.open()
            }
        }
        MenuItem {
            text: qsTr("全屏")
            onTriggered: {
                m_root.sigfullScreen()
            }
        }
    }

    Popup {
        id:infoPopup
        anchors.centerIn: Overlay.overlay
        padding: 30
        width: 400
        height:400
        background: Rectangle {
            color: "#0f603f"
            opacity: 0.9
            radius: 15
            border.color: "grey"
        }

        ListView {
            id: metadataList
            anchors.fill: parent
            anchors.margins: 5
            model: ListModel {
                id:metadataListmdoe
                //ListElement { name: "Duration"; value: "xxxxxxx"}
            }
            delegate: RowLayout {
                id: rowlayout
                width: metadataList.width

                required property string name
                required property string value

                Label {
                    text: rowlayout.name + ":"
                    font.pixelSize: 16
                    color:"#cccccc"

                    Layout.preferredWidth: infoPopup.width / 2
                }
                Label {
                    text: rowlayout.value
                    font.pixelSize: 16
                    wrapMode: Text.WrapAnywhere
                    color: "#cccccc"

                    Layout.fillWidth: true
                }
            }
        }
    }

    Item{
        id:itemvideo
        anchors.bottom:slidervolume.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 0
        height: 30
        z:1
        opacity:0.8

        RoundButton{
            id:choosebtn
            anchors.bottom:parent.bottom
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 0
            radius: width/2
            text:qsTr("打开")
            background: Rectangle {
                id:choosebtnbg
                color: "#2f3f8f"
                radius: choosebtn.radius
            }
            FileDialog {
                property list<string> m_nameFilters
                property int m_selectedNameFilter: -1
                id: fileDialog
                currentFolder: StandardPaths.standardLocations(StandardPaths.MoviesLocation)[0]
                nameFilters: m_nameFilters
                selectedNameFilter.index: m_selectedNameFilter
                title: qsTr("请选择一个文件")
                onAccepted: {
                    if(fileDialog.selectedFile){
                        mediaPlayer.stop();
                        mediaPlayer.source = fileDialog.selectedFile

                        playbtnbg.color = "#00ff00"
                        playbtn.text=qsTr("播放")
                    }
                }
            }
            onClicked: {
                // QMediaPlayer::StoppedState
                fileDialog.open()
            }
        }

        RoundButton{
            id:playbtn
            anchors.bottom:parent.bottom
            anchors.top: parent.top
            anchors.right: choosebtn.left
            anchors.margins: 0
            text:qsTr("播放")
            radius: width/2
            background: Rectangle {
                id:playbtnbg
                color: "#00ff00"
                radius: playbtn.radius
            }
            onClicked: {
                // QMediaPlayer::StoppedState
                if(mediaPlayer.playbackState === MediaPlayer.PlayingState){
                    mediaPlayer.pause()
                    playbtnbg.color = "#00ff00"
                    playbtn.text=qsTr("播放")
                } else {
                    mediaPlayer.play()
                    playbtnbg.color = "#ffff00"
                    playbtn.text=qsTr("暂停")
                }
            }
        }

        RowLayout {
            anchors.bottom:parent.bottom
            anchors.left: parent.left
            anchors.right: playbtn.left
            anchors.top: parent.top
            anchors.margins: 0
            //text: getTime(mediaPlayer.position) + " / " + getTime(mediaPlayer.duration)
            Label {
                id: mediaTime
                color: "#01ff00"
                font.bold: true
                Layout.maximumWidth: 100
                text: m_root.getTime(slidervideo.value)
            }
            Slider {
                id:slidervideo
                // anchors.bottom:parent.bottom
                // anchors.left: parent.left
                // anchors.right: playbtn.left
                // anchors.top: parent.top
                // anchors.margins: 0
                Layout.fillWidth: true
                //Layout.preferredWidth: parent.width - mediaTime.width - durationTime.width
                background: Rectangle {
                    id: backgroundRec
                    x: slidervideo.leftPadding
                    y: slidervideo.topPadding + slidervideo.availableHeight / 2 - height / 2
                    implicitWidth: 120
                    implicitHeight: 8
                    width: slidervideo.availableWidth
                    height: implicitHeight
                    radius: 10
                    color: "#01ff00"
                    opacity: 0.2
                    border.color: "#01ff00"
                    border.width: 1
                }
                handle: Rectangle {
                    x: slidervideo.leftPadding + slidervideo.visualPosition * (slidervideo.availableWidth - width)
                    y: slidervideo.topPadding + slidervideo.availableHeight / 2 - height / 2
                    implicitWidth: 8
                    implicitHeight: 8
                    color: "transparent"
                }
                Rectangle {
                    width: slidervideo.visualPosition * slidervideo.availableWidth
                    x: slidervideo.leftPadding
                    y: slidervideo.topPadding + slidervideo.availableHeight / 2 - height / 2
                    height: 8
                    color: "#2ecc71"
                    radius: 10
                }
                from: 0
                to: mediaPlayer.duration
                value: 0 //mediaPlayer.position / mediaPlayer.duration
                onMoved: {
                    if(mediaPlayer.seekable){
                        mediaPlayer.position = slidervideo.value
                    }
                }
                Keys.onPressed: (event) => {
                    event.accepted = true
                }
            }
            Label {
                id: durationTime
                color: "#01ff00"
                font.bold: true
                Layout.maximumWidth: 100
                text: m_root.getTime(slidervideo.to)
            }
        }
    }

    // Slider {
    //     z:1
    //     opacity:0.8
    //     id:slidervolume
    //     anchors.bottom:parent.bottom
    //     anchors.left: parent.left
    //     anchors.right: parent.right
    //     anchors.margins: 0
    //     height: 30
    //     from: 0.
    //     to: 1.
    //     value: 0.5
    // }
    Slider {
        id:slidervolume
        background: Rectangle {
            id: backgroundRecvolume
            x: slidervolume.leftPadding
            y: slidervolume.topPadding + slidervolume.availableHeight / 2 - height / 2
            implicitWidth: 120
            implicitHeight: 8
            width: slidervolume.availableWidth
            height: implicitHeight
            radius: 10
            color: "#01ff00"
            opacity: 0.2
            border.color: "#01ff00"
            border.width: 1
        }
        handle: Rectangle {
            x: slidervolume.leftPadding + slidervolume.visualPosition * (slidervolume.availableWidth - width)
            y: slidervolume.topPadding + slidervolume.availableHeight / 2 - height / 2
            implicitWidth: 8
            implicitHeight: 8
            color: "transparent"
        }
        Rectangle {
            width: slidervolume.visualPosition * slidervolume.availableWidth
            x: slidervolume.leftPadding
            y: slidervolume.topPadding + slidervolume.availableHeight / 2 - slidervolume / 2
            height: 8
            color: "#2ecc71"
            radius: 10
        }
        anchors.bottom:parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 0
        //height: 30
        from: 0.
        to: 1.
        value: 0.7
        Keys.onPressed: (event) => {
            event.accepted = true
        }
    }

}
