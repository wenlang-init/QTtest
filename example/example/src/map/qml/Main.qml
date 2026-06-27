// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtCore
import QtQuick
import QtLocation
import QtPositioning

Item {
    id: root
    anchors.fill: parent

    Plugin {
        id: mapPlugin
        name: "esri" // "mapboxgl", "esri", ...
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(59.91, 10.75) // Oslo
        zoomLevel: 14
    }

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
