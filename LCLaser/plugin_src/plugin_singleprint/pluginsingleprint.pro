#-------------------------------------------------
#
# Project created by QtCreator 2015-09-22T16:26:54
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = singleprint
TEMPLATE = lib

TRANSLATIONS = singleprint.ts

SOURCES += \
    src/singleprint.cpp \
    src/singleprintwidget.cpp

HEADERS +=\
    ../../src/rpinterface.h \
    ../../src/builder/controlinterface.h \
    ../../src/dataprocess/datainterface.h \
    ../base_plugin/rpexport.h \
    ../base_plugin/rpplugin.h \
    src/singleprint.h \
    src/singleprintwidget.h


INCLUDEPATH += ../base_plugin \
               ../../src \
               ../../src/builder \
               ../../src/dataprocess


FORMS += \
    Forms/singleprintform.ui

DISTFILES +=
