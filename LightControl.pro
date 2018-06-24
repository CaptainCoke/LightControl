TEMPLATE = subdirs
CONFIG += orderes
SUBDIRS += \
    liblightcontrol \
    LightControlGui \
    light_control

LightControlGui.depends = liblightcontrol
light_control.depends = liblightcontrol
