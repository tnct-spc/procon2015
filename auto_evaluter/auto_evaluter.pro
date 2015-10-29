#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T13:51:41
#
#-------------------------------------------------

QT       += core concurrent

QT       -= gui

TARGET = auto_evaluter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    algorithm_evaluater.cpp

HEADERS += \
    algorithm_evaluater.hpp
QMAKE_CXXFLAGS += -std=c++14
unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao

unix:!macx: LIBS += -L$$OUT_PWD/../tengu/ -ltengu

INCLUDEPATH += $$PWD/../tengu
DEPENDPATH += $$PWD/../tengu

unix {
    target.path = /usr/local/bin
    INSTALLS += target
}
