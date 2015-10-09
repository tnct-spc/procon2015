#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T01:42:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = master
TEMPLATE = app


SOURCES += main.cpp\
        master.cpp

HEADERS  += master.h

FORMS    += master.ui

QMAKE_CXXFLAGS += -std=c++14

unix:!macx: LIBS += -L$$OUT_PWD/../muen_zuka/ -lmuen_zuka

INCLUDEPATH += $$PWD/../muen_zuka
DEPENDPATH += $$PWD/../muen_zuka

unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao

INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao

unix:!macx: LIBS += -L$$OUT_PWD/../tengu/ -ltengu

INCLUDEPATH += $$PWD/../tengu
DEPENDPATH += $$PWD/../tengu

include(../lib/qhttpserver/qhttpserver.pri)

unix {
    target.path = /usr/local/bin
    INSTALLS += target
}
