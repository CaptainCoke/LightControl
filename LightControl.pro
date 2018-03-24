#-------------------------------------------------
#
# Project created by QtCreator 2018-03-04T18:05:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LightControl
TEMPLATE = app
CONFIG += c++1z

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        LightControl.cpp \
        GatewayAccess.cpp \
    Widgets/LightBulbWidget.cpp \
    Widgets/RGBLightBulbWidget.cpp \
    Widgets/CTLightBulbWidget.cpp \
    Widgets/DeviceNodeWidget.cpp \
    Widgets/SensorWidget.cpp \
    Widgets/RemoteControlWidget.cpp \
    Nodes/LightBulb.cpp \
    Nodes/RGBLightBulb.cpp \
    Nodes/CTLightBulb.cpp \
    Nodes/Sensor.cpp \
    Nodes/DeviceNode.cpp \
    Nodes/RemoteControl.cpp \
    Nodes/Node.cpp \
    Nodes/LightGroup.cpp

HEADERS += \
        LightControl.h \
        GatewayAccess.h \
    Widgets/LightBulbWidget.h \
    Widgets/RGBLightBulbWidget.h \
    Widgets/CTLightBulbWidget.h \
    Widgets/DeviceNodeWidget.h \
    Widgets/SensorWidget.h \
    Widgets/RemoteControlWidget.h \
    Nodes/LightBulb.h \
    Nodes/RGBLightBulb.h \
    Nodes/CTLightBulb.h \
    Nodes/Sensor.h \
    Nodes/DeviceNode.h \
    Nodes/RemoteControl.h \
    Nodes/Node.h \
    Nodes/LightGroup.h

FORMS += \
        LightControl.ui \
    Widgets/DeviceNodeWidget.ui
