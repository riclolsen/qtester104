# Protocol behaviour tests: qmake && make && ./protocol_tests (exit code = failures)
QT += network widgets
CONFIG += console c++17
CONFIG -= app_bundle
TARGET = protocol_tests
TEMPLATE = app
DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060C00
INCLUDEPATH += ../..
SOURCES += protocol_tests.cpp \
    ../../iec104_class.cpp \
    ../../logmsg.cpp \
    ../../qiec104.cpp
HEADERS += ../../qiec104.h
