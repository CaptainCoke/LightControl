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
    LightBulbWidget.cpp \
    GatewayAccess.cpp \
    LightBulb.cpp \
    RGBLightBulbWidget.cpp \
    RGBLightBulb.cpp \
    CTLightBulb.cpp \
    CTLightBulbWidget.cpp \
    Sensor.cpp \
    DeviceNode.cpp \
    DeviceNodeWidget.cpp \
    SensorWidget.cpp \
    RemoteControl.cpp \
    RemoteControlWidget.cpp

HEADERS += \
        LightControl.h \
    LightBulbWidget.h \
    GatewayAccess.h \
    LightBulb.h \
    RGBLightBulbWidget.h \
    RGBLightBulb.h \
    CTLightBulb.h \
    CTLightBulbWidget.h \
    Sensor.h \
    DeviceNode.h \
    DeviceNodeWidget.h \
    SensorWidget.h \
    RemoteControl.h \
    RemoteControlWidget.h

FORMS += \
        LightControl.ui \
    DeviceNodeWidget.ui
