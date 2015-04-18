#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       -= gui

TARGET = takao
TEMPLATE = lib

DEFINES += TAKAO_LIBRARY

SOURCES +=

HEADERS += data_type.hpp\
        takao_global.hpp \
    point_type.hpp \
    stone_type.hpp \
    placed_stone_type.hpp \
    field_type.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++14
