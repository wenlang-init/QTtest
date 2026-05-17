QT       += core gui
QT       += sql network
#QT       += qml quick
QT       += quickwidgets
win32: QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 控制台打印
win32:CONFIG += console
MOC_DIR=objc
OBJECTS_DIR=obj
UI_DIR=uic
CONFIG(debug,debug|release){
    DESTDIR=$$DESTDIRBASE/debug
    LIBS += -L$$DESTDIRBASE/debug/lib -lnettunnel
    #LIBS += $$DESTDIRBASE/debug/lib/libnettunnel.so
    LIBS += -L$$DESTDIRBASE/debug/lib -lprintFunction
} else {
    DESTDIR=$$DESTDIRBASE/release
    android:contains(QT_ARCH, arm64-v8a){
        LIBS += -L$$DESTDIRBASE/release/lib -lnettunnel_arm64-v8a
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction_arm64-v8a
    } else {
        LIBS += -L$$DESTDIRBASE/release/lib -lnettunnel
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction
    }
    #LIBS += $$DESTDIRBASE/release/lib/libnettunnel.a
    win32-msvc*{}else{LIBS += -lpthread}
}
INCLUDEPATH += $$PWD/../library/nettunnel
INCLUDEPATH += $$PWD/../library/printFunction

unix{
    LIBS += -Wl,-rpath=./
}

#include(qmqtt/src/CMakeLists.txt)
#include(qmqtt/qmqtt.pri)
#include(mqttc/mqttc.pri)

win32 {
    INCLUDEPATH += $$PWD/ffmpeg/windows/include
    LIBS += -L$$PWD/ffmpeg/windows/lib -lavformat -lavcodec -lavutil -lswscale -lswresample -lavfilter -lavdevice

    SRCUIDIR = $$PWD/ffmpeg/windows/bin/*.dll
    CONFIG(debug,debug|release){
        DESTUIDIR = $$DESTDIRBASE/debug
    } else {
        DESTUIDIR = $$DESTDIRBASE/release
    }
    QMAKE_POST_LINK += xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTUIDIR,/,\\)
    #QMAKE_POST_LINK += copy /Y $$replace(libsrcpath,/,\\) $$replace(libdespath,/,\\)
}


win32 {
    INCLUDEPATH += $$PWD/qmqtt/include
    win32-msvc*{
        # qmqtt lib
        CONFIG(debug,debug|release){
            LIBS += -L$$PWD/qmqtt/lib/win/msvc -lQt6Mqttd
            SRCUIDIR = $$PWD/qmqtt/lib/win/msvc/Qt6Mqttd.dll
            DESTUIDIR = $$DESTDIRBASE/debug
        } else {
            LIBS += -L$$PWD/qmqtt/lib/win/msvc -lQt6Mqtt
            SRCUIDIR = $$PWD/qmqtt/lib/win/msvc/Qt6Mqtt.dll
            DESTUIDIR=$$DESTDIRBASE/release
        }
    } else {
        CONFIG(debug,debug|release){
            LIBS += -L$$PWD/qmqtt/lib/win/mingw64 -lQt6Mqttd
            SRCUIDIR = $$PWD/qmqtt/lib/win/mingw64/Qt6Mqttd.dll
            DESTUIDIR = $$DESTDIRBASE/debug
        } else {
            LIBS += -L$$PWD/qmqtt/lib/win/mingw64 -lQt6Mqtt
            SRCUIDIR = $$PWD/qmqtt/lib/win/mingw64/Qt6Mqtt.dll
            DESTUIDIR=$$DESTDIRBASE/release
        }
    }
    QMAKE_POST_LINK += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTUIDIR,/,\\)
}
message($$QMAKE_POST_LINK)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    SOURCES += \
        ffmpegtest.cpp
    HEADERS += \
        ffmpegtest.h
}

SOURCES += \
    codeeditor/codeeditor.cpp \
    codeeditor/codeedittestw.cpp \
    codeeditor/highlighter.cpp \
    delegate/mylistview.cpp \
    delegate/picturedelegate.cpp \
    delegate/picturemodel.cpp \
    delegate/pictureproxymodel.cpp \
    draw/drawobject.cpp \
    draw/drawwidget.cpp \
    graphics/graphicswidget.cpp \
    graphics/tmitem.cpp \
    graphics/tmsecne.cpp \
    graphics/tmview.cpp \
    listw/listmodel.cpp \
    listw/listw.cpp \
    main.cpp \
    publicip/publicip.cpp \
    shouyin/decode/autf8.c \
    shouyin/decode/base64.c \
    shouyin/decode/beastSound.c \
    shouyin/decode/edcrypt.c \
    shouyin/public/funchelper.cpp \
    shouyin/shouyinw.cpp \
    shouyin/usecode.cpp \
    sql/sql_engine.cpp \
    sql/testsql.cpp \
    weather/weather.cpp \
    weatherwidget.cpp \
    widget.cpp

HEADERS += \
    codeeditor/codeeditor.h \
    codeeditor/codeedittestw.h \
    codeeditor/highlighter.h \
    delegate/mylistview.h \
    delegate/picturedelegate.h \
    delegate/picturemodel.h \
    delegate/pictureproxymodel.h \
    draw/drawobject.h \
    draw/drawwidget.h \
    graphics/graphicswidget.h \
    graphics/tmitem.h \
    graphics/tmsecne.h \
    graphics/tmview.h \
    listw/listmodel.h \
    listw/listw.h \
    publicip/publicip.h \
    shouyin/decode/autf8.h \
    shouyin/decode/base64.h \
    shouyin/decode/beastSound.h \
    shouyin/decode/edcrypt.h \
    shouyin/decode/finallyRecovery.h \
    shouyin/decode/unicode2gbktab.h \
    shouyin/public/funchelper.h \
    shouyin/shouyinw.h \
    shouyin/usecode.h \
    sql/sql_engine.h \
    sql/testsql.h \
    weather/citycode.h \
    weather/weather.h \
    weatherwidget.h \
    widget.h

FORMS += \
    codeeditor/codeedittestw.ui \
    shouyin/shouyinw.ui \
    weatherwidget.ui \
    widget.ui

linux {
    SOURCES += \
    epoll/epolltest.cpp

    HEADERS += \
    epoll/epolltest.h
}

win32 {
    SOURCES += \
    wordtopdf/wordtopdf.cpp

    HEADERS += \
    wordtopdf/wordtopdf.h

    FORMS += \
    wordtopdf/wordtopdf.ui
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

OTHER_FILES += \
    $$PWD/sql/test.sql

RESOURCES += \
    resource.qrc \
    $$PWD/shouyin/res.qrc
