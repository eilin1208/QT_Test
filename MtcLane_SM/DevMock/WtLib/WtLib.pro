#-------------------------------------------------
#
# Project created by QtCreator 2013-01-10T10:54:01
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = WtLib
TEMPLATE = lib

DEFINES += WTLIB_LIBRARY

SOURCES += wtlib.cpp \
    wtthread.cpp

HEADERS += wtlib.h\
        WtLib_global.h \
    wtthread.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE5BAEA24
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = WtLib.dll
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

OTHER_FILES +=
include($$PWD/../TcpClient/TcpClient.pri)
