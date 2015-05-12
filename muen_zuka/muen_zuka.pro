#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:01:17
#
#-------------------------------------------------

QT       -= gui

TARGET = muen_zuka
TEMPLATE = lib

DEFINES += MUEN_ZUKA_LIBRARY

SOURCES += net.cpp \
    processor.cpp

HEADERS += muen_zuka.hpp\
        muen_zuka_global.h \
    net.hpp \
    processor.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao
QMAKE_CXXFLAGS += -std=c++14
INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
