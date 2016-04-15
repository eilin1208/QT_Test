#-------------------------------------------------
#
# Project created by QtCreator 2012-12-25T09:58:01
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = FDLib
TEMPLATE = lib

DEFINES += FDLIB_LIBRARY

SOURCES += fdlib.cpp \
    FD_Utils.cpp

HEADERS += fdlib.h\
        FDLib_global.h \
    FD_Utils.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEAAE6764
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = FDLib.dll
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
