import QtQuick
import QtMultimedia
import QtCore
import QtQuick.Controls
// import QtQuick.Layouts
// import QtQuick.Controls.Fusion
// import QtQuick.Dialogs

Item {
    function getTime(time : int) : string {
        const h = Math.floor(time / 3600000).toString()
        const m = Math.floor(time / 60000).toString()
        const s = Math.floor(time / 1000 - m * 60).toString()
        return `${h.padStart(2,'0')}:${m.padStart(2,'0')}:${s.padStart(2, '0')}`
    }

    signal sigfullScreen()

    id:m_root

    Menu {
        id: menu
        MenuItem {
            text: qsTr("全屏")
            onTriggered: {
                m_root.sigfullScreen()
            }
        }
    }

    TapHandler {
        // /property bool isshowmore: true
        // 使用 | (按位或) 组合多个按钮
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onDoubleTapped: {
        }

        onTapped: (eventPoint, button)=> {
            if (button === Qt.RightButton) {
                menu.popup()
            }
        }
    }

    MediaDevices {
        id: mediaDevices
        onVideoInputsChanged: {
            console.log("video inputs changed: ", mediaDevices.videoInputs)
        }
        onAudioInputsChanged: {
            console.log("audio inputs changed: ", mediaDevices.audioInputs)
        }
        onAudioOutputsChanged: {
            console.log("audio outputs changed: ", mediaDevices.audioOutputs)
        }
    }
    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            cameraDevice: mediaDevices.defaultVideoInput
        }
        imageCapture: ImageCapture {
            id: imageCapture
        }

        recorder: MediaRecorder {
            id: recorder
            // resolution: "640x480"
            // frameRate: 30
        }
        videoOutput: viewfinder
        Component.onCompleted: {
            //list<audioDevice>
            // var listad = mediaDevices.audioInputs
            // for(var i=0;i<listad.length;i++){
            //     cmobobox.model.append({text: listad[i].description})
            //     //cmobobox.model.push(listad[i].description)
            // }
            //cmobobox.currentIndex = combox.find("X")

            // list<cameraDevice>
            var listvd = mediaDevices.videoInputs
            for(var i=0;i<listvd.length;i++){
                var str = listvd[i].description
                    // +listvd[i].position
                    // +listvd[i].correctionAngle
                    // +listvd[i].videoFormats
                    // +listvd[i].id
                    // +(listvd[i].isDefault ? "默认" : "")
                cmobobox.model.append({text:str})
                //cmobobox.model.push(listad[i].description)
                //console.log("cameraDevice: ", str)
            }

            //captureSession.camera.start()
            //captureSession.camera.stop()
            console.log("audioInputs: ", mediaDevices.audioInputs)
            console.log("audioOutputs: ", mediaDevices.audioOutputs)
            console.log("videoInputs: ", mediaDevices.videoInputs)
            console.log("defaultVideoInput: ", mediaDevices.defaultVideoInput)
            console.log("defaultAudioInput: ", mediaDevices.defaultAudioInput)
            console.log("defaultAudioOutput: ", mediaDevices.defaultAudioOutput)

            console.log("camera cameraDevice(当前活动的相机设备): ", captureSession.camera.cameraDevice)
            console.log("camera cameraFormat(当前活动的相机格式): ", captureSession.camera.cameraFormat)
            console.log("camera active(是否处于活动状态): ", captureSession.camera.active)
            console.log("camera exposureCompensation(曝光补偿): ", captureSession.camera.exposureCompensation)
            console.log("camera exposureMode(曝光模式): ", captureSession.camera.exposureMode)
            console.log("camera exposureTime(曝光时间s): ", captureSession.camera.exposureTime)
            console.log("camera flashMode(闪光灯模式): ", captureSession.camera.flashMode)
            console.log("camera flashReady", captureSession.camera.flashReady)
            console.log("camera focusDistance(对焦距离0~1)", captureSession.camera.focusDistance)
            console.log("camera focusMode(对焦模式)", captureSession.camera.focusMode)
            console.log("camera focusPoint(对焦点)", captureSession.camera.focusPoint)
            console.log("camera isoSensitivity(ISO感光度)", captureSession.camera.isoSensitivity)
            console.log("camera manualExposureTime(曝光时间)", captureSession.camera.manualExposureTime)
            console.log("camera manualIsoSensitivity(如何设置ISO感光度，-1表示自动感光)", captureSession.camera.manualIsoSensitivity)
            console.log("camera maximumZoomFactor(最大缩放系数)", captureSession.camera.maximumZoomFactor)
            console.log("camera minimumZoomFactor(最小缩放系数)", captureSession.camera.minimumZoomFactor)
            console.log("camera supportedFeatures(相机支持的特性)", captureSession.camera.supportedFeatures)
            console.log("camera torchMode(闪光灯)", captureSession.camera.torchMode)
            console.log("camera whiteBalanceMode(白平衡)", captureSession.camera.whiteBalanceMode)
            console.log("camera zoomFactor(缩放系数)", captureSession.camera.zoomFactor)
        }
    }

    Item {
        id:photoPreview
        anchors.fill:parent
        visible:true
        Image {
            id: preview
            anchors.fill : parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            source: imageCapture.preview
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {

            }
        }
    }

    VideoOutput {
        id: viewfinder
        anchors.fill: parent
        visible: true
    }

    // ButtonGroup {
    //     buttons: rowbtn.children
    // }

    Row{
        id:rowbtn
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: btn1.height
        width: btn1.width + btn2.width + cmobobox.width
        z:1
        RoundButton{
            id:btn1
            text:qsTr("开始")
            radius: width/2
            background: Rectangle {
                color: "#00ff00"
                radius: btn1.width
            }
            onClicked: {
                captureSession.camera.start()
            }
        }
        RoundButton{
            id:btn2
            text:qsTr("停止")
            radius: width/2
            background: Rectangle {
                color: "#ffff00"
                radius: btn2.width
            }
            onClicked: {
                captureSession.camera.stop()
            }
        }
        WComboBox{
        //ComboBox{
            id:cmobobox
            filterAble:false
            editable: false
            //clip: true
            model: ListModel {
                //ListElement { text: "Banana" }
            }
            onDisplayTextChanged: {
                //console.log(combox.displayText)
            }
            // onModelNodeClicked: (text) => {
            //     console.log("clicked: ", text,cmobobox.displayText)
            // }
        }
    }

    Item {
        id:viewfinderitem
        anchors.fill:parent
        visible:false
        MediaPlayer {
            id: mediaPlayer
            //playbackRate:false
            source: recorder.actualLocation ? recorder.actualLocation : ""
            videoOutput: videooutput
            audioOutput: AudioOutput {
                id: audio
                //muted: false
                //volume: slidervolume.value
            }
            onMetaDataChanged : {

            }

            onPositionChanged:{
                //slidervideo.value = mediaPlayer.position
            }
        }
        VideoOutput {
            id: videooutput
            anchors.fill: parent
        }
    }

}
