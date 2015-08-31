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

SOURCES += main.cpp

HEADERS +=

QMAKE_CXXFLAGS += -std=c++14

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../takao/release/ -ltakao
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../takao/debug/ -ltakao
else:unix: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
