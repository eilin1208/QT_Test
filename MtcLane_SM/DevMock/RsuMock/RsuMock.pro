#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T10:17:27
#
#-------------------------------------------------

QT       += core gui

TARGET = RsuMock
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    portcomm.cpp

HEADERS  += widget.h \
    portcomm.h

FORMS    += widget.ui

LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPort1

INCLUDEPATH += $$PWD/../../DevModule/SerialPort/include
DEPENDPATH += $$PWD/../../DevModule/SerialPort/include
