TEMPLATE = subdirs

SUBDIRS += \
    $$PWD/library/library.pro \
    $$PWD/slaveClient \
    $$PWD/masterCenter \
    $$PWD/shellbuild \
    $$PWD/slaveServer \
    $$PWD/OtherTools \
    $$PWD/netStatus \
    $$PWD/AIDialog \
    $$PWD/audio

#SUBDIRS += $$PWD/ipattrc

OTHER_FILES += \
    .qmake.conf

CONFIG(release,debug|release){
    # 顺序编译
    CONFIG += ordered
}
