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


SOURCES += main.cpp \
    processor.cpp \
    net.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../algorithm/release/ -lalgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../algorithm/debug/ -lalgorithm
else:unix: LIBS += -L$$OUT_PWD/../algorithm/ -lalgorithm

INCLUDEPATH += $$PWD/../algorithm
DEPENDPATH += $$PWD/../algorithm

QMAKE_CXXFLAGS += -std=c++14

HEADERS += \
    net.hpp \
    processor.hpp
