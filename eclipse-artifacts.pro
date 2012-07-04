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

TEMPLATE = app

LIBS += -lz

SOURCES += main.cpp

RESOURCES += resources/artifacts.xml

OTHER_FILES += \
    resources/feature.xml

include(QuaZIP/quazip/quazip.pri)
