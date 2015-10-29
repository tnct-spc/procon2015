#-------------------------------------------------
#
# Project created by QtCreator 2015-05-14T16:00:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slave
TEMPLATE = app


SOURCES += main.cpp\
        slave.cpp

HEADERS  += slave.h

FORMS    += slave.ui

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

unix {
    target.path = /usr/local/bin
    INSTALLS += target
}
