#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:40:23
#
#-------------------------------------------------

QT       += core network


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

unix:!macx: LIBS += -L$$OUT_PWD/../muen_zuka/ -lmuen_zuka

INCLUDEPATH += $$PWD/../muen_zuka
DEPENDPATH += $$PWD/../muen_zuka

unix:!macx: LIBS += -L$$OUT_PWD/../tengu/ -ltengu

INCLUDEPATH += $$PWD/../tengu
DEPENDPATH += $$PWD/../tengu
