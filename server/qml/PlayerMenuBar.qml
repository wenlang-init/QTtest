// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtMultimedia

Item {
    id: root

    required property MediaPlayer mediaPlayer
    required property VideoOutput videoOutput
    required property MetadataInfo metadataInfo
    required property TracksInfo audioTracksInfo
    required property TracksInfo videoTracksInfo
    required property TracksInfo subtitleTracksInfo

    height: menuBar.height

    signal closePlayer

    function loadUrl(url) {
        mediaPlayer.stop()
        mediaPlayer.source = url
        mediaPlayer.play()
    }

    function closeOverlays(){
        metadataInfo.visible = false;
        audioTracksInfo.visible = false;
        videoTracksInfo.visible = false;
        subtitleTracksInfo.visible = false;
    }

    function showOverlay(overlay){
        closeOverlays();
        overlay.visible = true;
    }

    Popup {
        id: urlPopup
        anchors.centerIn: Overlay.overlay

        RowLayout {
            id: rowOpenUrl
            Label {
                text: qsTr("URL:");
            }

            TextInput {
                id: urlText
                focus: true
                Layout.minimumWidth: 400
                wrapMode: TextInput.WrapAnywhere
                Keys.onReturnPressed: { loadUrl(text); urlText.text = ""; urlPopup.close() }
            }

            Button {
                text: "加载"
                onClicked: { loadUrl(urlText.text); urlText.text = ""; urlPopup.close() }
            }
        }
        onOpened: { urlPopup.forceActiveFocus() }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        onAccepted: {
            mediaPlayer.stop()
            mediaPlayer.source = fileDialog.currentFile
            mediaPlayer.play()
        }
    }

    MenuBar {
        id: menuBar
        anchors.left: parent.left
        anchors.right: parent.right

        Menu {
            title: qsTr("&文件")
            Action {
                text: qsTr("&打开")
                onTriggered: fileDialog.open()
            }
            Action {
                text: qsTr("&URL");
                onTriggered: urlPopup.open()
            }

            Action {
                text: qsTr("&退出");
                onTriggered:{
                    closePlayer()
                }
            }
        }

        Menu {
            title: qsTr("&查看")
            Action {
                text: qsTr("媒体信息")
                onTriggered: showOverlay(metadataInfo)
            }
        }

        Menu {
            title: qsTr("&选项")
            Action {
                text: qsTr("音频")
                onTriggered: showOverlay(audioTracksInfo)
            }
            Action {
                text: qsTr("视频")
                onTriggered: showOverlay(videoTracksInfo)
            }
            Action {
                text: qsTr("字幕")
                onTriggered: showOverlay(subtitleTracksInfo)
            }
        }
    }
}
