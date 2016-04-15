#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T22:21:02
#
#-------------------------------------------------

QT       -= gui

TARGET = FDArm
TEMPLATE = lib

DEFINES += FDARM_LIBRARY

SOURCES += fdarm.cpp

HEADERS += fdarm.h\
        FDArm_global.h


LIBS += -L$$PWD/lib/ -lQtSerialPort

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
