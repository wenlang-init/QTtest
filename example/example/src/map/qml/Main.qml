// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtCore
import QtQuick
//import QtLocation
//import QtPositioning
import QtWebView

Item {
    id: root
    anchors.fill: parent

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
            console.log(webView.url);
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

    // Plugin {
    //     id: mapPlugin
    //     name: "esri" // "mapboxgl", "esri", ...
    // }

    // Map {
    //     anchors.fill: parent
    //     plugin: mapPlugin
    //     center: QtPositioning.coordinate(59.91, 10.75) // Oslo
    //     zoomLevel: 14
    // }

    // LocationPermission {
    //     id: permission
    //     accuracy: LocationPermission.Precise
    //     availability: LocationPermission.WhenInUse
    // }

    // PermissionsScreen {
    //     anchors.fill: parent
    //     visible: permission.status !== Qt.PermissionStatus.Granted
    //     requestDenied: permission.status === Qt.PermissionStatus.Denied
    //     onRequestPermission: permission.request()
    // }

    // Component {
    //     id: applicationComponent
    //     PlacesMap {}
    // }

    // Loader {
    //     anchors.fill: parent
    //     active: permission.status === Qt.PermissionStatus.Granted
    //     sourceComponent: applicationComponent
    // }
}
