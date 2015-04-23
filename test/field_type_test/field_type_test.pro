#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T12:50:24
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = field_type_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += field_type_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_CXXFLAGS += -std=c++14

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../takao/release/ -ltakao
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../takao/debug/ -ltakao
else:unix: LIBS += -L$$OUT_PWD/../../takao/ -ltakao

INCLUDEPATH += $$PWD/../../takao
DEPENDPATH += $$PWD/../../takao
