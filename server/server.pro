QT       += core gui network sql
QT       += multimedia multimediawidgets
#QT += bluetooth
QT       += qml quick
QT       += quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $$PWD/bin/

SOURCES += \
    codeeditor/codeeditor.cpp \
    codeeditor/highlighter.cpp \
    graphics/graphicswidget.cpp \
    graphics/tmitem.cpp \
    graphics/tmsecne.cpp \
    graphics/tmview.cpp \
    listw/listmodel.cpp \
    listw/listw.cpp \
    main.cpp \
    nettunnel/edcrypt.c \
    nettunnel/nettunnel.cpp \
    nettunnel/udpslave.cpp \
    network.cpp \
    qmlwidget.cpp \
    sql/sql_engine.cpp \
    sql/testsql.cpp \
    tcpserver.cpp \
    testwk.cpp \
    udpobj.cpp \
    weather/postcode.cpp \
    weather/publicip.cpp \
    weather/weather.cpp \
    weather/weatherwidget.cpp \
    widget.cpp

HEADERS += \
    codeeditor/codeeditor.h \
    codeeditor/highlighter.h \
    graphics/graphicswidget.h \
    graphics/tmitem.h \
    graphics/tmsecne.h \
    graphics/tmview.h \
    listw/listmodel.h \
    listw/listw.h \
    nettunnel/edcrypt.h \
    nettunnel/nettunnel.h \
    nettunnel/udpslave.h \
    network.h \
    propertyHelper.h \
    qmlwidget.h \
    sql/sql_engine.h \
    sql/testsql.h \
    tcpserver.h \
    testwk.h \
    udpobj.h \
    weather/citycode.h \
    weather/postcode.h \
    weather/publicip.h \
    weather/weather.h \
    weather/weatherwidget.h \
    widget.h

FORMS += \
    weather/weatherwidget.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32{
#    LIBS += -L$$PWD/lib/windows/curl-8.11.0_4-win64-mingw/lib -lcurl
##    LIBS += $$PWD/lib/windows/curl-8.11.0_4-win64-mingw/lib/libcurl.a
#    INCLUDEPATH += $$PWD/lib/windows/curl-8.11.0_4-win64-mingw/include
}

RESOURCES += \
    image.qrc \
    qml.qrc \
    qss.qrc \
    res.qrc

DISTFILES +=

