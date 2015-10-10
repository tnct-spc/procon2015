#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T23:00:51
#
#-------------------------------------------------

QT       -= gui \
         += concurrent

TARGET = tengu
TEMPLATE = lib

DEFINES += TENGU_LIBRARY

SOURCES += algorithm/simple_algorithm.cpp \
    algorithm/poor_algo.cpp \
    algorithm/yrange.cpp \
    algorithm/sticky_algo.cpp \
    algorithm/read_ahead.cpp \
    algorithm/new_beam.cpp \
    algorithm/stikcy_beam.cpp \
    algorithm/yrange_based_yayoi.cpp \
    algorithm/yrange_next.cpp


HEADERS += tengu.hpp\
        tengu_global.h \
    algorithm/simple_algorithm.hpp \
    algorithm/poor_algo.hpp \
    algorithm/yrange.hpp \
    algorithm/sticky_algo.hpp \
    algorithm/read_ahead.hpp \
    algorithm/new_beam.hpp \
    algorithm/stikcy_beam.hpp \
    algorithm/yrange_based_yayoi.h \
    algorithm/yrange_next.h

LIBS += -lQt5Concurrent

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../takao/ -ltakao
QMAKE_CXXFLAGS += -std=c++14 -mavx
INCLUDEPATH += $$PWD/../takao
DEPENDPATH += $$PWD/../takao
