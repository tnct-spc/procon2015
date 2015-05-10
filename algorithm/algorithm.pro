#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:49:31
#
#-------------------------------------------------

QT       -= gui

TARGET = algorithm
TEMPLATE = lib

DEFINES += ALGORITHM_LIBRARY

SOURCES += \
    algorithm_type.cpp \
    field_type.cpp \
    placed_stone_type.cpp \
    point_type.cpp \
    utils.cpp \
    problem_type.cpp \
    stone_type.cpp

HEADERS += \
    algorithm_global.hpp \
    algorithm.hpp \
    utils.hpp \
    point_type.hpp \
    stone_type.hpp \
    placed_stone_type.hpp \
    field_type.hpp \
    algorithm_type.hpp \
    problem_type.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++14
