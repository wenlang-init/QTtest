QT       += core gui
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 控制台打印
win32:CONFIG += console
MOC_DIR=objc
OBJECTS_DIR=obj
UI_DIR=uic

unix{
    LIBS += -Wl,-rpath=./
}

CONFIG(debug,debug|release){
    DESTDIR=$$DESTDIRBASE/debug
    LIBS += -L$$DESTDIRBASE/debug/lib -lprintFunction
} else {
    # release 也记录行号函数名等信息，会造成额外开支，默认关闭
    DEFINES += QT_MESSAGELOGCONTEXT
    DESTDIR=$$DESTDIRBASE/release
    android:contains(QT_ARCH, arm64-v8a){
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction_arm64-v8a
    } else {
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction
    }
}
INCLUDEPATH += $$PWD/../library/printFunction

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
