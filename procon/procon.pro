#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:40:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = procon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../takao/release/ -ltakao
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../takao/debug/ -ltakao
else:unix: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao

QMAKE_CXXFLAGS += -std=c++14
