QT += core network websockets

TARGET = light_control
TEMPLATE = app
CONFIG += c++1z

INCLUDEPATH += ../liblightcontrol
PRE_TARGETDEPS += ../liblightcontrol/liblightcontrol.a
LIBS += ../liblightcontrol/liblightcontrol.a

SOURCES += \
    main.cpp \
    RemoteControlService.cpp \
    PowerButtonHandler.cpp \
    SceneButtonHandler.cpp \
    BrightnessButtonHandler.cpp \
    AlarmService.cpp

HEADERS += \
    RemoteControlService.h \
    RemoteControlButtonHandler.h \
    PowerButtonHandler.h \
    SceneButtonHandler.h \
    BrightnessButtonHandler.h \
    AlarmService.h
