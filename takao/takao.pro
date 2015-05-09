#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       += core network

TARGET = takao
TEMPLATE = lib

DEFINES += TAKAO_LIBRARY

SOURCES +=

HEADERS += takao_global.hpp \
        takao.hpp \
        utils.hpp \
        data_type.hpp\
        point_type.hpp \
        stone_type.hpp \
        placed_stone_type.hpp \
        field_type.hpp \
   		net.hpp \
     	algorithm_type.hpp \
    hoge.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++14
