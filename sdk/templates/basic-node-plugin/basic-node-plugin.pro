QT += core gui widgets
TEMPLATE = lib
CONFIG += plugin c++17
TARGET = MgbBasicNodePlugin

SDK_ROOT = $$(MGB_UML_SDK)
isEmpty(SDK_ROOT) {
    error("Set MGB_UML_SDK to the root of the MGB-UML Plugin SDK")
}

INCLUDEPATH += $$SDK_ROOT/include/mgb-uml
INCLUDEPATH += $$SDK_ROOT/include/mgb-uml/src
INCLUDEPATH += $$SDK_ROOT/include/mgb-uml/src/data
INCLUDEPATH += $$SDK_ROOT/include/mgb-uml/src/gui

HEADERS += \
    basicnodeplugin.h \
    basicnodeitem.h

SOURCES += \
    basicnodeplugin.cpp \
    basicnodeitem.cpp

RESOURCES += basicnodeplugin.qrc
