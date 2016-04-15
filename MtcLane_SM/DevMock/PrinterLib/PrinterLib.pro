#-------------------------------------------------
#
# Project created by QtCreator 2012-12-25T10:29:22
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = PrinterLib
TEMPLATE = lib

DEFINES += PRINTERLIB_LIBRARY

SOURCES += printerlib.cpp

HEADERS += printerlib.h\
        PrinterLib_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE70BE94F
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = PrinterLib.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
include($$PWD/../TcpClient/TcpClient.pri)
