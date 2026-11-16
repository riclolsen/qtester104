# -------------------------------------------------
# Project created by QtCreator 2009-12-28T16:19:15
# -------------------------------------------------
QT += network \
    widgets
TARGET = QTester104
TEMPLATE = app
# target: Qt 6.12 LTS
!versionAtLeast(QT_VERSION, 6.12.0): error("QTester104 requires Qt 6.12 LTS or later, found $$QT_VERSION")
# fail the build on any API deprecated up to the 6.12 target
DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060C00
# same language standard on every compiler (GCC 16 defaults to C++20, MSVC to C++17)
CONFIG += c++17
SOURCES += main.cpp \
    appwindow.cpp \
    mainwindow.cpp \
    iec104_class.cpp \
    logmsg.cpp \
    qiec104.cpp
HEADERS += appwindow.h \
    mainwindow.h \
    iec104_types.h \
    iec104_class.h \
    logmsg.h \
    qiec104.h
FORMS += mainwindow.ui
OTHER_FILES += \
    qtester104.ini
