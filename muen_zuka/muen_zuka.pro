#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:01:17
#
#-------------------------------------------------

QT       -= gui

TARGET = muen_zuka
TEMPLATE = lib

DEFINES += MUEN_ZUKA_LIBRARY

SOURCES += muen_zuka.cpp

HEADERS += muen_zuka.hpp\
        muen_zuka_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
