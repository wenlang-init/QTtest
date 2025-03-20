TEMPLATE = subdirs

SUBDIRS += \
    $$PWD/nettunnel/nettunnel.pro \
    $$PWD/printFunction

CONFIG(release,debug|release){
    # 顺序编译
    # CONFIG += ordered
}
