#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T18:44:30
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = problem_maker
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -lboost_system -lboost_program_options

SOURCES += main.cpp \
    raw_field.cpp \
    file_export.cpp \
    raw_stone.cpp

HEADERS += \
    raw_field.hpp \
    file_export.hpp \
    raw_stone.hpp \
    problem_maker.hpp

QMAKE_CXXFLAGS += -std=c++14
