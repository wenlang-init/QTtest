import QtQuick 2.15
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
//import Skin
//import QtGraphicalEffects
import "."

Rectangle {
    id: root
    color: "black"
    property alias source: mediaPlayer.source
    signal closePlayer
    Popup {
        id: mediaError
        anchors.centerIn: Overlay.overlay
        Text {
            id: mediaErrorText
        }
    }

    MediaPlayer {
        id: mediaPlayer

        function updateMetadata() {
            metadataInfo.clear();
            metadataInfo.read(mediaPlayer.metaData);
            metadataInfo.read(mediaPlayer.audioTracks[mediaPlayer.activeAudioTrack]);
            metadataInfo.read(mediaPlayer.videoTracks[mediaPlayer.activeVideoTrack]);
        }

        videoOutput: videoOutput
        audioOutput: AudioOutput {
            id: audio
            muted: playbackControl.muted
            volume: playbackControl.volume
        }

        onErrorOccurred: { mediaErrorText.text = mediaPlayer.errorString; mediaError.open() }
        onMetaDataChanged: { updateMetadata() }
        onTracksChanged: {
            audioTracksInfo.read(mediaPlayer.audioTracks);
            audioTracksInfo.selectedTrack = mediaPlayer.activeAudioTrack;
            videoTracksInfo.read(mediaPlayer.videoTracks);
            videoTracksInfo.selectedTrack = mediaPlayer.activeVideoTrack;
            subtitleTracksInfo.read(mediaPlayer.subtitleTracks);
            subtitleTracksInfo.selectedTrack = mediaPlayer.activeSubtitleTrack;
            updateMetadata()
        }
        onActiveTracksChanged: { updateMetadata() }
    }

    PlayerMenuBar {
        id: menuBar

        anchors.left: parent.left
        anchors.right: parent.right

        visible: !videoOutput.fullScreen

        mediaPlayer: mediaPlayer
        videoOutput: videoOutput
        metadataInfo: metadataInfo
        audioTracksInfo: audioTracksInfo
        videoTracksInfo: videoTracksInfo
        subtitleTracksInfo: subtitleTracksInfo

        //onClosePlayer: root.close()
        onClosePlayer:{
            mediaPlayer.stop()
            root.closePlayer()
        }
    }


    VideoOutput {
        id: videoOutput

        property bool fullScreen: false

        anchors.top: fullScreen ? parent.top : menuBar.bottom
        anchors.bottom: playbackControl.top
        anchors.left: parent.left
        anchors.right: parent.right

        TapHandler {
            onDoubleTapped: {
                parent.fullScreen ?  showNormal() : showFullScreen()
                parent.fullScreen = !parent.fullScreen
            }
            onTapped: {
                metadataInfo.visible = false
                audioTracksInfo.visible = false
                videoTracksInfo.visible = false
                subtitleTracksInfo.visible = false
            }
        }
    }

    MetadataInfo {
        id: metadataInfo

        anchors.right: parent.right
        anchors.top: videoOutput.fullScreen ? parent.top : menuBar.bottom
        anchors.bottom: playbackControl.opacity ? playbackControl.bottom : parent.bottom

        visible: false
    }

    TracksInfo {
        id: audioTracksInfo

        anchors.right: parent.right
        anchors.top: videoOutput.fullScreen ? parent.top : menuBar.bottom
        anchors.bottom: playbackControl.opacity ? playbackControl.bottom : parent.bottom

        visible: false
        onSelectedTrackChanged:  mediaPlayer.activeAudioTrack = audioTracksInfo.selectedTrack
    }

    TracksInfo {
        id: videoTracksInfo

        anchors.right: parent.right
        anchors.top: videoOutput.fullScreen ? parent.top : menuBar.bottom
        anchors.bottom: playbackControl.opacity ? playbackControl.bottom : parent.bottom

        visible: false
        onSelectedTrackChanged: mediaPlayer.activeVideoTrack = videoTracksInfo.selectedTrack
    }

    TracksInfo {
        id: subtitleTracksInfo

        anchors.right: parent.right
        anchors.top: videoOutput.fullScreen ? parent.top : menuBar.bottom
        anchors.bottom: playbackControl.opacity ? playbackControl.bottom : parent.bottom

        visible: false
        onSelectedTrackChanged: mediaPlayer.activeSubtitleTrack = subtitleTracksInfo.selectedTrack
    }

    PlaybackControl {
        id: playbackControl

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        mediaPlayer: mediaPlayer
    }
}
