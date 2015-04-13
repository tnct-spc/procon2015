#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T05:18:22
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=c++1z

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../takao/release/ -ltakao
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../takao/debug/ -ltakao
else:unix: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
