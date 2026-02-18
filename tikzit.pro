# CONFIG += debug

QT += core gui widgets network pdf pdfwidgets

# --- VERSIONING LOGIC ---
isEmpty(GIT_VERSION) {
    GIT_VERSION = "Dev-Build"
}
# Wrap in quotes to handle spaces safely
DEFINES += "GIT_VERSION=\\\"$$GIT_VERSION\\\""

test {
    CONFIG += testcase
}

TARGET   = mgb-uml
TEMPLATE = app

isEmpty(PREFIX) {
    PREFIX=/usr/local
}

share.path = $${PREFIX}/share
share.files = share/*

target.path = $${PREFIX}/bin
INSTALLS += target share

# platform-specific options
win32:RC_ICONS += images/tikzit.ico
win32:RC_ICONS += images/tikzdoc.ico
macx:ICON = images/tikzit.icns

linux-g++ {
    QMAKE_CXX = ccache g++
}

QMAKE_INFO_PLIST = Info.plist

# Qt 5.8+ support
contains(QT_VERSION, ^5\\.[5-7].*) {
    macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
}

DEFINES += QT_DEPRECATED_WARNINGS

FLEXSOURCES = src/data/tikzlexer.l
BISONSOURCES = src/data/tikzparser.y

include(flex.pri)
include(bison.pri)

# --- SOURCES (Safe Format) ---
SOURCES += src/tikzit.cpp
SOURCES += src/util.cpp

# GUI
SOURCES += src/gui/mainwindow.cpp
SOURCES += src/gui/pathitem.cpp
SOURCES += src/gui/toolpalette.cpp
SOURCES += src/gui/tikzscene.cpp
SOURCES += src/gui/propertypalette.cpp
SOURCES += src/gui/tikzview.cpp
SOURCES += src/gui/nodeitem.cpp
SOURCES += src/gui/edgeitem.cpp
SOURCES += src/gui/commands.cpp
SOURCES += src/gui/undocommands.cpp
SOURCES += src/gui/mainmenu.cpp
SOURCES += src/gui/stylepalette.cpp
SOURCES += src/gui/styleeditor.cpp
SOURCES += src/gui/previewwindow.cpp
SOURCES += src/gui/latexprocess.cpp
SOURCES += src/gui/exportdialog.cpp
SOURCES += src/gui/delimitedstringitemdelegate.cpp
SOURCES += src/gui/preferencedialog.cpp

# DATA
SOURCES += src/data/path.cpp
SOURCES += src/data/graph.cpp
SOURCES += src/data/node.cpp
SOURCES += src/data/edge.cpp
SOURCES += src/data/graphelementdata.cpp
SOURCES += src/data/graphelementproperty.cpp
SOURCES += src/data/tikzdocument.cpp
SOURCES += src/data/tikzassembler.cpp
SOURCES += src/data/tikzstyles.cpp
SOURCES += src/data/style.cpp
SOURCES += src/data/stylelist.cpp
SOURCES += src/data/pdfdocument.cpp
SOURCES += src/data/delimitedstringvalidator.cpp


# --- HEADERS (Safe Format) ---
HEADERS += src/tikzit.h
HEADERS += src/util.h

# GUI
HEADERS += src/gui/mainwindow.h
HEADERS += src/gui/pathitem.h
HEADERS += src/gui/toolpalette.h
HEADERS += src/gui/tikzscene.h
HEADERS += src/gui/propertypalette.h
HEADERS += src/gui/tikzview.h
HEADERS += src/gui/nodeitem.h
HEADERS += src/gui/edgeitem.h
HEADERS += src/gui/commands.h
HEADERS += src/gui/undocommands.h
HEADERS += src/gui/mainmenu.h
HEADERS += src/gui/stylepalette.h
HEADERS += src/gui/styleeditor.h
HEADERS += src/gui/previewwindow.h
HEADERS += src/gui/latexprocess.h
HEADERS += src/gui/exportdialog.h
HEADERS += src/gui/delimitedstringitemdelegate.h
HEADERS += src/gui/preferencedialog.h

# DATA
HEADERS += src/data/path.h
HEADERS += src/data/graph.h
HEADERS += src/data/node.h
HEADERS += src/data/edge.h
HEADERS += src/data/graphelementdata.h
HEADERS += src/data/graphelementproperty.h
HEADERS += src/data/tikzparserdefs.h
HEADERS += src/data/tikzdocument.h
HEADERS += src/data/tikzassembler.h
HEADERS += src/data/tikzstyles.h
HEADERS += src/data/style.h
HEADERS += src/data/stylelist.h
HEADERS += src/data/pdfdocument.h
HEADERS += src/data/delimitedstringvalidator.h

FORMS += src/gui/mainwindow.ui \
    src/gui/propertypalette.ui \
    src/gui/mainmenu.ui \
    src/gui/stylepalette.ui \
    src/gui/styleeditor.ui \
    src/gui/previewwindow.ui \
    src/gui/exportdialog.ui \
    src/gui/preferencedialog.ui

INCLUDEPATH += src src/gui src/data

DISTFILES +=

RESOURCES += tikzit.qrc

test {
    QT += testlib
    TARGET = UnitTests
    SOURCES -= src/main.cpp
    HEADERS += src/test/testtest.h \
        src/test/testparser.h \
        src/test/testtikzoutput.h
    SOURCES += src/test/testmain.cpp \
        src/test/testtest.cpp \
        src/test/testparser.cpp \
        src/test/testtikzoutput.cpp
} else {
    SOURCES += src/main.cpp
}