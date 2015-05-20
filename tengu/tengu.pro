#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:00:51
#
#-------------------------------------------------

QT       -= gui

TARGET = tengu
TEMPLATE = lib

DEFINES += TENGU_LIBRARY

SOURCES += algorithm/simple_algorithm.cpp \
    algorithm/poor_algo.cpp

HEADERS += tengu.hpp\
        tengu_global.h \
    algorithm/simple_algorithm.hpp \
    algorithm/poor_algo.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao
QMAKE_CXXFLAGS += -std=c++14
INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
