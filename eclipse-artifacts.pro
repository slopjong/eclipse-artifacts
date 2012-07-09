#-------------------------------------------------
#
# Project created by QtCreator 2012-06-22T07:21:39
#
#-------------------------------------------------


QT       += core xmlpatterns xml network

QT       -= gui

TARGET = eclipse-artifacts
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=gnu++11

OBJECTS_DIR = build/objects
MOC_DIR = build/moc

TEMPLATE = app

LIBS += -lz

HEADERS += \
    src/downloadmanager.h \
    src/consoleapplication.h

SOURCES += \
    src/main.cpp \
    src/downloadmanager.cpp \
    src/consoleapplication.cpp

OTHER_FILES += \
    resources/artifacts.xml \
    resources/feature.xml \
    resources/com.onpositive.gae.appwrench_1.5.0.jar \
    resources/PKGBUILD \
    3rd-party/QuaZIP/quazip/* \
    3rd-party/QuaZIP/test/* \
    README.md

QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter

include(3rd-party/QuaZIP/quazip/quazip.pri)

RESOURCES += \
    resources.qrc
