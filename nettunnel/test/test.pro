QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

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

# gstreamer
win32 {
    XCOPYCMD =
    win32-msvc*{
        GSTBASE = D:/gstreamer/1.0/msvc_x86_64
        INCLUDEPATH += $$GSTBASE/include \
                       $$GSTBASE/include/gstreamer-1.0 \
                       $$GSTBASE/include/gstreamer-1.0/gst \
                       $$GSTBASE/include/glib-2.0 \
                       $$GSTBASE/lib/glib-2.0/include \
                       $$GSTBASE/lib/gstreamer-1.0/include/gst

        LIBS += -L$$GSTBASE/lib -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstapp-1.0

        LIBDIR = D:/gstreamer/run/1.0/msvc_x86_64/bin
        SRCUIDIR = $$LIBDIR/gstreamer-1.0-0.dll
        XCOPYCMD += xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/intl-8.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/glib-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/gobject-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/gmodule-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/ffi-7.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/pcre2-8-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
    } else {
        GSTBASE = D:/gstreamer/1.0/mingw_x86_64
        INCLUDEPATH += $$GSTBASE/include \
                       $$GSTBASE/include/gstreamer-1.0 \
                       $$GSTBASE/include/gstreamer-1.0/gst \
                       $$GSTBASE/include/glib-2.0 \
                       $$GSTBASE/lib/glib-2.0/include \
                       $$GSTBASE/lib/gstreamer-1.0/include/gst

        LIBS += -L$$GSTBASE/lib -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0 -lgstapp-1.0
        # LIBS += $$(pkg-config --libs gstreamer-1.0)
        # QMAKE_LFLAGS += -Wl,--whole-archive -lmingw32 -Wl,--no-whole-archive

        LIBDIR = D:/gstreamer/run/1.0/mingw_x86_64/bin
        SRCUIDIR = $$LIBDIR/libgstreamer-1.0-0.dll
        XCOPYCMD += xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libintl-8.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libglib-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libgobject-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libgmodule-2.0-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libffi-7.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
        SRCUIDIR =$$LIBDIR/libpcre2-8-0.dll
        XCOPYCMD += & xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
    }
}
QMAKE_POST_LINK += $$XCOPYCMD #xcopy /E/Y/H/C/I $$replace(SRCUIDIR,/,\\) $$replace(DESTDIR,/,\\)
message($$QMAKE_POST_LINK)
message($$LIBS)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
