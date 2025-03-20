QT       += core gui
QT       += sql network
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
    QMAKE_POST_LINK = xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTUIDIR,/,\\)
}
message($$QMAKE_POST_LINK)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codeeditor/codeeditor.cpp \
    codeeditor/highlighter.cpp \
    main.cpp \
    publicip/publicip.cpp \
    shouyin/shouyinw.cpp \
    sql/sql_engine.cpp \
    sql/testsql.cpp \
    weather/weather.cpp \
    weatherwidget.cpp \
    widget.cpp

HEADERS += \
    codeeditor/codeeditor.h \
    codeeditor/highlighter.h \
    publicip/publicip.h \
    shouyin/shouyinw.h \
    sql/sql_engine.h \
    sql/testsql.h \
    weather/citycode.h \
    weather/weather.h \
    weatherwidget.h \
    widget.h

FORMS += \
    shouyin/shouyinw.ui \
    weatherwidget.ui \
    widget.ui

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
