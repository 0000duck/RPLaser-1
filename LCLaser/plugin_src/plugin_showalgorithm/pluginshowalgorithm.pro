#-------------------------------------------------
#
# Project created by QtCreator 2015-09-22T16:26:54
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = showAlgorithm
TEMPLATE = lib

TRANSLATIONS = plugin_exportSTL.ts

SOURCES += \
    src/showalgorithm.cpp

   
HEADERS +=\
    ../../src/rpinterface.h \
    ../../src/builder/controlinterface.h \
    ../../src/dataprocess/datainterface.h \
    ../base_plugin/rpexport.h \
    ../base_plugin/rpplugin.h \
    src/showalgorithm.h


INCLUDEPATH += ../base_plugin \
               ../../src \
        ../../src/builder\
        ../../src/dataprocess

