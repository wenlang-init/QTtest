import QtQuick

Item {
    Loader{
        id:loader
        source: widget.qmlQString()
        //sourceComponent: rect
    }
}
