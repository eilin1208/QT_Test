#-------------------------------------------------
#
# Project created by QtCreator 2012-08-07T09:20:52
#
#-------------------------------------------------

QT       -= gui

TARGET = NotePrinter_SP320
TEMPLATE = lib

DEFINES += NOTEPRINTER_SP320_LIBRARY

SOURCES += noteprinter.cpp

HEADERS += noteprinter.h\
        NotePrinter_SP320_global.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPort1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPortd1
else:unix:!macx:!symbian: LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPort1

INCLUDEPATH += $$PWD/../../DevModule/SerialPort/include
DEPENDPATH += $$PWD/../../DevModule/SerialPort/include
