#-------------------------------------------------
#
# Project created by QtCreator 2012-12-26T15:39:40
#
#-------------------------------------------------

QT       -= gui

TARGET = CardFileMgr
TEMPLATE = lib

DEFINES += CARDFILEMGR_LIBRARY

SOURCES += cardfilemgr.cpp

HEADERS += cardfilemgr.h\
        CardFileMgr_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE4C7B4F6
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = CardFileMgr.dll
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

