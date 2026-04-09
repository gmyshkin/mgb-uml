# ======================================================================
# MGB UML Class Plugin Build File
# ======================================================================

# Build as a dynamic library plugin
TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

# The output name (generates libMgbUmlClass.so)
TARGET = MgbUmlClass

# Auto-deploy to the main app's plugin folder
DESTDIR = ../../build/plugins

# Locate the core app headers
INCLUDEPATH += ../../src/gui \
               ../../src/data \
               ../../src

# The plugin declarations and drawing logic
HEADERS += mgbUmlClassPlugin.h \
           mgbUmlClassItem.h

# The implementation files
SOURCES += mgbUmlClassPlugin.cpp \
           mgbUmlClassItem.cpp

# --- MACOS FIX: Tell Apple to trust the main app to provide core symbols ---
macx {
    QMAKE_LFLAGS += -undefined dynamic_lookup
}