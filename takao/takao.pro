#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       -= gui

TARGET = takao
TEMPLATE = lib

DEFINES += TAKAO_LIBRARY

SOURCES += takao.cpp

HEADERS += takao.hpp\
        takao_global.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
