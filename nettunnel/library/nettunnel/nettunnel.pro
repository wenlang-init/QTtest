QT += network

TEMPLATE = lib
TARGET = nettunnel

DEFINES += LIBPUBLIC_LIBRARY
win32-msvc*{
}
# mingw
win32-g++{
}

CONFIG(debug,debug|release){
    DESTDIR=$$DESTDIRBASE/debug/lib
} else {
    CONFIG += staticlib
    DESTDIR=$$DESTDIRBASE/release/lib
}
CONFIG += c++17
#message($$CONFIG)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32{
    # 去除版本设置的影响
    CONFIG += skip_target_version_ext
    CONFIG += unversioned_libname
}
# 控制台打印
#win32:CONFIG += console
MOC_DIR=objc
OBJECTS_DIR=obj
UI_DIR=uic

#INCLUDEPATH += $$PWD/


# Default rules for deployment.
unix {
    target.path = /usr/lib
    INSTALLS += target
}

#linux {
#    CONFIG(debug,debug|release){
#        QMAKE_POST_LINK = cp -a $$DESTDIRBASE/debug/lib/lib* $$DESTDIRBASE/debug
#    }
#}

HEADERS += \
    edcrypt.h \
    libpublic_global.h \
    nettunnel.h \
    udpslave.h

SOURCES += \
    edcrypt.c \
    nettunnel.cpp \
    udpslave.cpp
