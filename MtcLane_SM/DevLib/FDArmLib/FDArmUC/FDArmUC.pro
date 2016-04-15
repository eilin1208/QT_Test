#-------------------------------------------------
#
# Project created by QtCreator 2012-11-26T13:36:34
#
#-------------------------------------------------

QT       -= gui

TARGET = FDArmUC
TEMPLATE = lib

DEFINES += FDARMUC_LIBRARY

HEADERS += \
    $$PWD/fdinterface.h \
    $$PWD/FDArmBase_global.h \
    $$PWD/fdarmbase.h \
    $$PWD/fduc.h

SOURCES += \
    $$PWD/fdinterface.cpp \
    $$PWD/fdarmbase.cpp \
    $$PWD/fduc.cpp

#SOURCES +=

#HEADERS +=

win32: LIBS += -L$$PWD/../../../DevModule/SerialPort/lib/ -llibQtSerialPort1
unix: LIBS += -L$$PWD/../../../DevModule/SerialPort/lib/ -lQtSerialPort
INCLUDEPATH += $$PWD/../../../DevModule/SerialPort/include
DEPENDPATH += $$PWD/../../../DevModule/SerialPort/include
message(fduc)

#include($$PWD/../../MiniLog/MiniLog.pri)

