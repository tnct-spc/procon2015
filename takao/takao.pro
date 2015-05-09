#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       += core network

TARGET = takao
TEMPLATE = lib

DEFINES += TAKAO_LIBRARY


unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++14

include(./takao.pri)
