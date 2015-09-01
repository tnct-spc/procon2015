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
    problem_maker.cpp

HEADERS += \
    problem_maker.hpp

QMAKE_CXXFLAGS += -std=c++14

LIBS += -L$$OUT_PWD/../takao/ -ltakao
LIBS += -L$$OUT_PWD/../tengu/ -ltengu

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
