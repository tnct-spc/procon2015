#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T00:25:24
#
#-------------------------------------------------

QT       += testlib core network

QT       -= gui

TARGET = stone_type_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -lboost_system
SOURCES += stone_type_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_CXXFLAGS += -std=c++14

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../algorithm/release/ -lalgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../algorithm/debug/ -lalgorithm
else:unix: LIBS += -L$$OUT_PWD/../../algorithm/ -lalgorithm

INCLUDEPATH += $$PWD/../../algorithm
DEPENDPATH += $$PWD/../../algorithm
