TEMPLATE = subdirs
CONFIG += orderes
SUBDIRS += \
	liblightcontrol \
	LightControlGui

LightControlGui.depends = liblightcontrol
