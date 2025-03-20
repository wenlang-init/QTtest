QT       += core gui network
#QT       += qml
QT       += quick
#QT       += quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 控制台打印
#win32:CONFIG += console
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

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    other/publicip.cpp \
    other/weather.cpp \
    qmlinturn.cpp

HEADERS += \
    other/citycode.h \
    other/publicip.h \
    other/weather.h \
    propertyHelper.h \
    qmlinturn.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
