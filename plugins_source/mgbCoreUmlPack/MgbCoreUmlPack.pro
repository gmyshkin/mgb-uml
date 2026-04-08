# ======================================================================
# MGB Core UML Plugin Build File
# ======================================================================

# Tell qmake to build a dynamic library (.so / .dll) instead of an app
TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

# The name of the final file (e.g., libMgbCoreUmlPack.so)
TARGET = MgbCoreUmlPack

# THE MAGIC TRICK: Auto-drop the finished plugin into the main app's folder!
DESTDIR = ../../plugins

# Tell the compiler where to find the core app's headers
INCLUDEPATH += ../../src/gui \
               ../../src/data \
               ../../src

# The files that make up this specific plugin
HEADERS += mgbCoreUmlPlugin.h \
           mgbUmlNodeItems.h

SOURCES += mgbCoreUmlPlugin.cpp \
           mgbUmlNodeItems.cpp