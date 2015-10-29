#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:01:17
#
#-------------------------------------------------

QT       -= gui
QT       += network
TARGET = muen_zuka
TEMPLATE = lib

DEFINES += MUEN_ZUKA_LIBRARY

SOURCES += net.cpp \
    algorithm_manager.cpp

HEADERS += muen_zuka.hpp\
        muen_zuka_global.h \
    net.hpp \
    algorithm_manager.hpp

unix {
    target.path = /usr/local/lib
    target.extra = ldconfig
    INSTALLS += target
}

LIBS += -lboost_system
unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao
QMAKE_CXXFLAGS += -std=c++14
INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao

unix:!macx: LIBS += -L$$OUT_PWD/../tengu/ -ltengu

INCLUDEPATH += $$PWD/../tengu
DEPENDPATH += $$PWD/../tengu
