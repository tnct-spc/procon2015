#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:00:51
#
#-------------------------------------------------

QT       -= gui

TARGET = tengu
TEMPLATE = lib

DEFINES += TENGU_LIBRARY

SOURCES += simple_algorithm.cpp

HEADERS += tengu.hpp\
        tengu_global.h \
    simple_algorithm.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao
QMAKE_CXXFLAGS += -std=c++14
INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao