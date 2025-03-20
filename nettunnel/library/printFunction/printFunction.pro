#CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = lib
TARGET = printFunction

CONFIG(debug,debug|release){
    DESTDIR=$$DESTDIRBASE/debug/lib
} else {
    CONFIG += staticlib
    DESTDIR=$$DESTDIRBASE/release/lib
}
win32-msvc*{
    # pthread lib
    LIBS += -L$$PWD/Pre-built.2/lib/x64/ -lpthreadVC2
    #LIBS += -L$$PWD/Pre-built.2/lib/x64/ -lpthreadGC2
    #LIBS += -L$$PWD/Pre-built.2/dll/x64/ -lpthreadVC2
    #LIBS += -L$$PWD/Pre-built.2/dll/x64/ -lpthreadGC2
    #LIBS += $$PWD/Pre-built.2/lib/x64/pthreadVC2.lib
    INCLUDEPATH += $$PWD/Pre-built.2/include

    SRCUIDIR = $$PWD/Pre-built.2/dll/x64/pthreadVC2.dll
    CONFIG(debug,debug|release){
        DESTUIDIR=$$DESTDIRBASE/debug
    } else {
        DESTUIDIR=$$DESTDIRBASE/release
    }
    QMAKE_POST_LINK = xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTUIDIR,/,\\)
} else {
    LIBS += -lpthread
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
    decode/autf8.h \
    decode/base64.h \
    decode/beastSound.h \
    decode/finallyRecovery.h \
    decode/unicode2gbktab.h \
    lognone.h \
    mArray.h \
    mList.h \
    mlog.h \
    printFunction.h

SOURCES += \
    decode/autf8.c \
    decode/base64.c \
    decode/beastSound.c \
    lognone.c \
    mArray.c \
    mList.c \
    mlog.cpp \
    printFunction.c
