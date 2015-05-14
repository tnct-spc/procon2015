#-------------------------------------------------
#
# Project created by QtCreator 2015-05-14T16:00:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slave
TEMPLATE = app


SOURCES += main.cpp\
        slave.cpp

HEADERS  += slave.h

FORMS    += slave.ui

QMAKE_CXXFLAGS += -std=c++14
