#-------------------------------------------------
#
# Project created by QtCreator 2015-04-20T18:08:56
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_takaotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_takaotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_CXXFLAGS += -std=c++14

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../takao/release/ -ltakao
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../takao/debug/ -ltakao
else:unix: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
