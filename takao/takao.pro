#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       += core network

TARGET = takao
TEMPLATE = lib

DEFINES += TAKAO_LIBRARY
LIBS += -lboost_system -lboost_thread

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++14 -mavx

include(./takao.pri)
