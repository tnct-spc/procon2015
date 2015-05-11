#-------------------------------------------------
#
# Project created by QtCreator 2015-05-06T00:53:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = outside_server
TEMPLATE = app


SOURCES += main.cpp\
    outside_server.cpp \
    http/answer_form.cpp \
    http/help_page.cpp \
    interior/uma.cpp \
    game_stage.cpp \
    http/request_mapper.cpp \
    http/output_problem_page.cpp

HEADERS  += outside_server.h \
    http/answer_form.h \
    http/help_page.h \
    interior/uma.h \
    game_stage.h \
    global.h \
    http/request_mapper.h \
    http/output_problem_page.h

FORMS    += outside_server.ui

QMAKE_CXXFLAGS += -std=c++14

include(../lib/qhttpserver/qhttpserver.pri)
