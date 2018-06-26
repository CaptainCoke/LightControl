QT += core network

TARGET = light_control
TEMPLATE = app
CONFIG += c++1z

INCLUDEPATH += ../liblightcontrol
PRE_TARGETDEPS += ../liblightcontrol/liblightcontrol.a
LIBS += ../liblightcontrol/liblightcontrol.a

SOURCES += \
    main.cpp \
    RemoteControlService.cpp \
    NetworkService.cpp \
    PowerButtonHandler.cpp \
    SceneButtonHandler.cpp \
    BrightnessButtonHandler.cpp

HEADERS += \
    RemoteControlService.h \
    NetworkService.h \
    NodeTools.h \
    RemoteControlButtonHandler.h \
    PowerButtonHandler.h \
    SceneButtonHandler.h \
    BrightnessButtonHandler.h
