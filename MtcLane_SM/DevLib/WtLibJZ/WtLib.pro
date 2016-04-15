#-------------------------------------------------
#
# Project created by QtCreator 2012-09-20T00:53:25
#
#-------------------------------------------------

QT       -= gui

TARGET = WtLib
TEMPLATE = lib

DEFINES += WTLIB_LIBRARY

SOURCES += wtlib.cpp \
    wtthread.cpp

HEADERS += wtlib.h\
        WtLib_global.h \
    wtthread.h \
    datadef.h

DEF_FILE += \
    $$PWD/WtLib.def

win32: DEFINES += WIN32

win32: LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPort1
unix: LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -lQtSerialPort

INCLUDEPATH += $$PWD/../../DevModule/SerialPort/include
DEPENDPATH += $$PWD/../../DevModule/SerialPort/include
