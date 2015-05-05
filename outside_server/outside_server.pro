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
    http/request_mapper.cpp \
    interior/uma.cpp \
    game_stage.cpp \

HEADERS  += outside_server.h \
    http/answer_form.h \
    http/help_page.h \
    http/request_mapper.h \
    interior/uma.h \
    game_stage.h \
    global.h \

FORMS    += outside_server.ui

QMAKE_CXXFLAGS += -std=c++14

include(../lib/QtWebApp/httpserver/httpserver.pri)
