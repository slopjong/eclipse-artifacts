#-------------------------------------------------
#
# Project created by QtCreator 2012-06-22T07:21:39
#
#-------------------------------------------------


QT       += gui xmlpatterns xml network

TARGET = eclipse-artifacts
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=gnu++11

OBJECTS_DIR = build/objects
MOC_DIR = build/moc
UI_HEADERS_DIR = src
UI_SOURCES_DIR = src

TEMPLATE = app

LIBS += -lz

HEADERS += \
    src/downloadmanager.h \
    src/application.h \
    src/mainwindow.h

SOURCES += \
    src/main.cpp \
    src/downloadmanager.cpp \
    src/application.cpp \
    src/mainwindow.cpp

OTHER_FILES += \
    resources/* \
    3rd-party/QuaZIP/quazip/* \
    3rd-party/QuaZIP/test/* \
    README.md

QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-reorder

include(3rd-party/QuaZIP/quazip/quazip.pri)

RESOURCES += \
    resources.qrc

FORMS += \
    src/mainwindow.ui
