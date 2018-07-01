QT += core network websockets

TARGET = playground
TEMPLATE = app
CONFIG += c++1z

INCLUDEPATH += ../liblightcontrol
PRE_TARGETDEPS += ../liblightcontrol/liblightcontrol.a
LIBS += ../liblightcontrol/liblightcontrol.a

SOURCES += \
    main.cpp

