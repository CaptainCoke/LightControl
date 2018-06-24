#-------------------------------------------------
#
# Project created by QtCreator 2018-05-06T20:57:49
#
#-------------------------------------------------

QT       += core network

TARGET = lightcontrol
TEMPLATE = lib
CONFIG += staticlib c++1z

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GatewayAccess.cpp \
    LightBulbState.cpp \
    LightColor.cpp \
    LightTemperature.cpp \
    Nodes/LightBulb.cpp \
    Nodes/RGBLightBulb.cpp \
    Nodes/CTLightBulb.cpp \
    Nodes/Sensor.cpp \
    Nodes/DeviceNode.cpp \
    Nodes/RemoteControl.cpp \
    Nodes/Node.cpp \
    Nodes/LightGroup.cpp \
    Nodes/LightGroupScene.cpp

HEADERS += \
    GatewayAccess.h \
    LightBulbState.h \
    LightColor.h \
    LightTemperature.h \
    Nodes/LightBulb.h \
    Nodes/RGBLightBulb.h \
    Nodes/CTLightBulb.h \
    Nodes/Sensor.h \
    Nodes/DeviceNode.h \
    Nodes/RemoteControl.h \
    Nodes/Node.h \
    Nodes/LightGroup.h \
    Nodes/LightGroupScene.h \
    Nodes/NodeFactory.h
