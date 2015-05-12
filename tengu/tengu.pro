#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:00:51
#
#-------------------------------------------------

QT       -= gui

TARGET = tengu
TEMPLATE = lib

DEFINES += TENGU_LIBRARY

SOURCES += tengu.cpp

HEADERS += tengu.hpp\
        tengu_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
