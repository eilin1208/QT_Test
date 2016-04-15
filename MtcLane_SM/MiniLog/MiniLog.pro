#-------------------------------------------------
#
# Project created by QtCreator 2013-03-11T17:49:44
#
#-------------------------------------------------

QT       -= gui

TARGET = MiniLog
TEMPLATE = lib

DEFINES += MINILOG_LIBRARY

SOURCES += minilog.cpp \
    fileoperate.cpp

HEADERS += minilog.h\
        MiniLog_global.h \
    fileoperate.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE37CD11D
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MiniLog.dll
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
