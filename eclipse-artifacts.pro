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

OTHER_FILES += \
    resources/artifacts.xml \
    resources/feature.xml \
    resources/com.onpositive.gae.appwrench_1.5.0.jar \
    3rd-party/QuaZIP/quazip/* \
    3rd-party/QuaZIP/test/*

include(3rd-party/QuaZIP/quazip/quazip.pri)
