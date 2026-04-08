# ======================================================================
# MGB UML Use Case Plugin Build File
# ======================================================================

# Build as a dynamic library plugin
TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

# The output name (generates libMgbUmlUseCase.so)
TARGET = MgbUmlUseCase

# Auto-deploy to the main app's plugin folder
DESTDIR = ../../build/plugins

# Locate the core app headers
INCLUDEPATH += ../../src/gui \
               ../../src/data \
               ../../src

# The plugin declarations and drawing logic
HEADERS += mgbUmlUseCasePlugin.h \
           mgbUmlUseCaseItem.h

# The implementation files
SOURCES += mgbUmlUseCasePlugin.cpp \
           mgbUmlUseCaseItem.cpp