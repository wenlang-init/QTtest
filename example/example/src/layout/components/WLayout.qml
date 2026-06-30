//pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import QtQuick.Controls.Fusion
import QtQuick.Dialogs
import QtCore
import QtWebView

//Item
Rectangle {
    id: root
    width: 400
    height: 300
    anchors.margins:0
    // anchors.topMargin: 0
    // anchors.leftMargin: 0
    // anchors.rightMargin: 0
    // anchors.bottomMargin: 0
    //color: "transparent"
    Image {
        id:backgroundimage
        anchors.fill: parent
        source: "qrc:/src/layout/resource/pic/01.jpg" // 图片路径，可为相对或绝对路径
        // Image.Stretch // 拉伸填充，可能会失真
        // Image.PreserveAspectFit // 保持比例适应，可能会有空白
        // Image.PreserveAspectCrop // 保持比例并裁剪填充
        // Image.Tile // 平铺填充
        // Image.TileVertically // 垂直平铺填充
        // Image.TileHorizontally // 水平平铺填充
        // Image.Pad // 保持原始大小，超出部分裁剪
        fillMode: Image.PreserveAspectCrop
        smooth: true // 平滑缩放
    }
    color:Qt.rgba(0,0,0,255)

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.topMargin: 0
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        clip: true
        contentWidth: flowContent.implicitWidth
        contentHeight: flowContent.implicitHeight

        Flow {
            id: flowContent
            width: flickable.width
            spacing: 16
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 24
            anchors.rightMargin: 24

            Rectangle {
                width: flowContent.width
                height: 100
                color: "transparent"
                // 占位用，制造顶部空白
            }

            Rectangle{
                // 占位
                id:videocontainerrect
                width: flowContent.width
                height: 600
                color: "transparent"
                WVideoPlayer {
                    id:videocontainer
                    anchors.fill:parent
                    onSigfullScreen: {
                        var isfullscreen = wLayout.changeFullScreen()
                        if(isfullscreen){
                            // 改变父级到主窗口的 contentItem
                            videocontainer.parent = root
                            videocontainer.anchors.fill = root
                            flickable.visible = false
                            backgroundimage.visible = false
                        } else {
                            // 恢复
                            videocontainer.parent = videocontainerrect
                            videocontainer.anchors.fill = videocontainerrect
                            flickable.visible = true
                            backgroundimage.visible = true
                        }
                    }
                }
            }

            // Rectangle {
            //     id:videocontainer
            //     width: flowContent.width
            //     height: 600
            //     color: "transparent"
            //     WVideoPlayer {
            //         id: wvideoPlayer
            //         anchors.fill: parent
            //     }
            // }


            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    text: "--------😎主要组件😎--------"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 30
                    font.bold: true
                    color: "#cccccc"
                }
            }

            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    text: "🍭各种按钮："
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20
                    font.bold: true
                    color: "#000000"
                }
            }

            Rectangle {
                width: flickable.width
                height: 150
                color: "#ff0000"
            }

            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    text: "😋一些表单："
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20
                    font.bold: true
                    color: "#000000"
                }
            }
            Rectangle {
                id:camerarect
                width: flowContent.width
                height: 500
                //color: "#fff000"
                color: "transparent"
                WCamera{
                    id:camera
                    anchors.fill:parent
                    onSigfullScreen: {
                        var isfullscreen = wLayout.changeFullScreen()
                        if(isfullscreen){
                            // 改变父级到主窗口的 contentItem
                            camera.parent = root
                            camera.anchors.fill = root
                            flickable.visible = false
                            //backgroundimage.visible = false
                        } else {
                            // 恢复
                            camera.parent = camerarect
                            camera.anchors.fill = camerarect
                            flickable.visible = true
                            //backgroundimage.visible = true
                        }
                    }
                }
            }

            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    text: "🍀好多卡片："
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20
                    font.bold: true
                    color: "#000000"
                }
            }
            Rectangle {
                width: flickable.width
                height: 650
                color: "#00ff00"
                WebView {
                    id: webView
                    anchors.fill: parent
                    // settings {
                    //     // 允许 WebView 本身加载 file:// URL
                    //     allowFileAccess: true
                    //     // 允许本地加载的文档访问其他本地 URL
                    //     localContentCanAccessFileUrls: true
                    // }
                    // url: "https://www.baidu.com/"
                    url: "file:///D:/mapdownload/高德/index.html"
                    Component.onCompleted: {
                        console.log("xxxxxxxxx");
                        //webView.loadHtml("<h1>Hello, Qt WebView!</h1>", "https://example.com")
                        //webView.loadHtml("", "file:///D:/mapdownload/高德/index1.html")
                    }
                    onLoadingChanged: {
                        console.log("Loading progress:", webView.loadProgress)
                    }
                    onUrlChanged: {
                        console.log("URL changed to:", webView.url)
                    }
                    onTitleChanged: {
                        console.log("Page title:", webView.title)
                    }
                    onCookieAdded: (domain, name)=>{
                        console.log("Cookie added:", domain, name)
                    }
                    onCookieRemoved: (domain, name)=>{
                        console.log("Cookie removed:", domain, name)
                    }
                }
            }

            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    id:cardText
                    text: "🍀------："
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20
                    font.bold: true
                    color: "#000000"
                }
            }
            Rectangle {
                width: flickable.width
                height: 350
                color: "#0000ff"
            }

            Rectangle {
                width: flickable.width
                height: 50
                color: "transparent"
                // 分割占位
                Text {
                    text: "🍀111111："
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20
                    font.bold: true
                    color: "#000000"
                }
            }
            Rectangle {
                width: flickable.width
                height: 150
                color: "#00ff00"
            }
        }
    }
}