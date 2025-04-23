QT       += core gui multimedia
QT       += printsupport

#DEFINES += QCUSTOMPLOT_USE_OPENGL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += resources_big

win32{
    win32-msvc*{
    }
    #mingw
    win32-g++{
        # file too big
        QMAKE_CXXFLAGS += -Wa,-mbig-obj
    }
}
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
    DESTDIR=$$DESTDIRBASE/release
    android:contains(QT_ARCH, arm64-v8a){
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction_arm64-v8a
    } else {
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction
    }
}
INCLUDEPATH += $$PWD/../library/printFunction

SOURCES += \
    Xxw/XxwCustomPlot.cpp \
    Xxw/XxwTracer.cpp \
    audioproc.cpp \
    fft/fft.cpp \
    fft2d/fftsg.c \
    fft2d/fftsg2d.c \
    main.cpp \
    qcustomplot/qcustomplot.cpp \
    wav/audio_wave.cpp \
    widget.cpp

HEADERS += \
    Xxw/XxwCustomPlot.h \
    Xxw/XxwTracer.h \
    audioproc.h \
    fft/fft.h \
    qcustomplot/qcustomplot.h \
    wav/audio_wave.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
