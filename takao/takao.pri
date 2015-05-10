INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT       += core network

HEADERS += $$PWD/takao_global.hpp \
        $$PWD/takao.hpp \
        $$PWD/utils.hpp \
        $$PWD/point_type.hpp \
        $$PWD/stone_type.hpp \
        $$PWD/placed_stone_type.hpp \
        $$PWD/field_type.hpp \
        $$PWD/net.hpp \
        $$PWD/algorithm_type.hpp \
        $$PWD/problem_type.hpp \
    $$PWD/processor.hpp

SOURCES += \
    $$PWD/algorithm_type.cpp \
    $$PWD/field_type.cpp \
    $$PWD/net.cpp \
    $$PWD/placed_stone_type.cpp \
    $$PWD/point_type.cpp \
    $$PWD/problem_type.cpp \
    $$PWD/processor.cpp \
    $$PWD/stone_type.cpp \
    $$PWD/utils.cpp
