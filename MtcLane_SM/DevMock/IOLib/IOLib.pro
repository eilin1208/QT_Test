#-------------------------------------------------
#
# Project created by QtCreator 2012-12-10T14:33:26
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = IOLib
TEMPLATE = lib

DEFINES += IOLIB_LIBRARY

SOURCES += iolib.cpp

HEADERS += iolib.h\
        IOLib_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2F85BC0
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = IOLib.dll
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
