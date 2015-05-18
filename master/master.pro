#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T01:42:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = master
TEMPLATE = app


SOURCES += main.cpp\
        master.cpp

HEADERS  += master.h

FORMS    += master.ui

QMAKE_CXXFLAGS += -std=c++14
