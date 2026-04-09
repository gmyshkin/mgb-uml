TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

TARGET = MgbUmlActor
DESTDIR = ../../build/plugins

INCLUDEPATH += ../../src/gui \
               ../../src/data \
               ../../src

HEADERS += mgbUmlActorPlugin.h \
           mgbUmlActorItem.h

SOURCES += mgbUmlActorPlugin.cpp \
           mgbUmlActorItem.cpp

macx {
    QMAKE_LFLAGS += -undefined dynamic_lookup
}
win32 {
    LIBS += -L../../build/release -lmgb-uml
}