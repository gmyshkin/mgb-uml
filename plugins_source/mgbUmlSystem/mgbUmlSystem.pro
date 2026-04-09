TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

TARGET = MgbUmlSystem
DESTDIR = ../../build/plugins

INCLUDEPATH += ../../src/gui \
               ../../src/data \
               ../../src

HEADERS += mgbUmlSystemPlugin.h \
           mgbUmlSystemItem.h

SOURCES += mgbUmlSystemPlugin.cpp \
           mgbUmlSystemItem.cpp

macx {
    QMAKE_LFLAGS += -undefined dynamic_lookup
}
win32 {
    LIBS += -L../../build/release -lmgb-uml
}