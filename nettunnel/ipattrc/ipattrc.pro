QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 控制台打印
#win32:CONFIG += console
MOC_DIR=objc
OBJECTS_DIR=obj
UI_DIR=uic
CONFIG(debug,debug|release){
    DESTDIR=$$DESTDIRBASE/debug
} else {
    DESTDIR=$$DESTDIRBASE/release
}

# QT_ARCH有以下预定义值：
#   x86_64：64位x86处理器
#   i386：32位x86处理器
#   arm：ARM处理器
#   armv6：ARMv6处理器
#   armv7：ARMv7处理器
#   armv7-a：ARMv7-A处理器
#   aarch64：64位ARM处理器
#   ppc：PowerPC处理器
#   ppc64：64位PowerPC处理器
#   mips：MIPS处理器
#   mipsel：小端MIPS处理器
#   sparc：SPARC处理器

PCAPPLUSPLUSHOME_MINGW=$$PWD/3rdparty/pcapplusplus/pcapplusplus-24.09-windows-mingw64-x86_64-gcc-14.2.0
PCAPPLUSPLUSHOME_MSVC=$$PWD/3rdparty/pcapplusplus/pcapplusplus-24.09-windows-vs2022-x64-release
PCAPPLUSPLUSHOME_UBUNTU=$$PWD/3rdparty/pcapplusplus/pcapplusplus-24.09-ubuntu-24.04-gcc-13.2.0-x86_64
PCAPPLUSPLUSHOME_ANDROID=$$PWD/3rdparty/pcapplusplus/pcapplusplus-24.09-android
#LIBS += -L$${PCAPPLUSPLUSHOME_MINGW}/lib -l-lPcap++ -lPacket++ -lCommon++ -lwpcap -lPacket -lws2_32 -liphlpapi
unix{
    android {
        INCLUDEPATH += $${PCAPPLUSPLUSHOME_ANDROID}/include
        contains(QT_ARCH, arm64-v8a){
            LIBS += -L$${PCAPPLUSPLUSHOME_MINGW}/arm64-v8a/30 -lPcap++ -lPacket++ -lCommon++
            #LIBS += $$DESTDIRBASE/arm64-v8a/30/libPcap++.a
        } else {
            LIBS += -L$$DESTDIRBASE/armeabi-v7a/30 -lPcap++ -lPacket++ -lCommon++
        }
    }
    linux {
        INCLUDEPATH += $${PCAPPLUSPLUSHOME_UBUNTU}/include
        contains(QT_ARCH, x86_64) {
            # QMAKE_HOST.arch x86_64
            LIBS += -L$${PCAPPLUSPLUSHOME_UBUNTU}/lib -lPcap++ -lPacket++ -lCommon++
        }
    }
}
#message($$DEFINES)
#message($$QT_ARCH)
win32{
    win32-msvc*{
        INCLUDEPATH += $${PCAPPLUSPLUSHOME_MSVC}/include
        contains(QT_ARCH, x86_64) {
            LIBS += -L$${PCAPPLUSPLUSHOME_MSVC}/lib -lPcap++ -lPacket++ -lCommon++
        }
    }
    #mingw
    win32-g++{
        INCLUDEPATH += $${PCAPPLUSPLUSHOME_MINGW}/include
        contains(QT_ARCH, x86_64) {
            LIBS += -L$${PCAPPLUSPLUSHOME_MINGW}/lib -lPcap++ -lPacket++ -lCommon++
            #LIBS += -lwpcap
            #LIBS += -lPacket
            #LIBS += -lws2_32
            #LIBS += -liphlpapi
            #LIBS += -static-libgcc -static-libstdc++
        }
    }
    #contains(DEFINES, WIN64):message("MSVC WIN64")
    #contains(DEFINES, MINGW_HAS_SECURE_API=1):message("MINGW_HAS_SECURE_API=1")
}

CONFIG(debug,debug|release){
    LIBS += -L$$DESTDIRBASE/debug/lib -lprintFunction
} else {
    android:contains(QT_ARCH, arm64-v8a){
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction_arm64-v8a
    } else {
        LIBS += -L$$DESTDIRBASE/release/lib -lprintFunction
    }
}
INCLUDEPATH += $$PWD/../library/printFunction

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
