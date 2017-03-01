#-------------------------------------------------
#
# Project created by QtCreator 2017-02-27T21:36:59
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = MultilayerINI
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Lib.cpp \
    mini.cpp \
    preprocessor.cpp \
    tree.cpp

HEADERS += \
    Lib.h \
    mini.h \
    preprocessor.h

QMAKE_CXXFLAGS += -std=c++11
