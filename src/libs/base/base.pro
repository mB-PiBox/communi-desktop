######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = base

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = $$BUILD_ROOT/lib

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/titlebar.h
HEADERS += $$PWD/treedelegate.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treerole.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/titlebar.cpp
SOURCES += $$PWD/treedelegate.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treewidget.cpp

include(../../config.pri)
include(plugins/plugins.pri)
include(../backend/backend.pri)

!exists(shared/shared.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_ROOT}.)
}
include(shared/shared.pri)
