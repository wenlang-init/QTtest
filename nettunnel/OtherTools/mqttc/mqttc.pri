SOURCES += \
    $$PWD/MQTT-C-master/src/mqtt.c \
    $$PWD/MQTT-C-master/src/mqtt_pal.c

HEADERS += \
    $$PWD/MQTT-C-master/include/mqtt.h \
    $$PWD/MQTT-C-master/include/mqtt_pal.h

INCLUDEPATH += $$PWD/MQTT-C-master/include

win {
    LIBS += -lws2_32
}
