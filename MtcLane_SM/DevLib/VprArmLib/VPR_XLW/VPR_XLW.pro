#-------------------------------------------------
#
# Project created by QtCreator 2012-09-16T10:53:55
#
#-------------------------------------------------

QT       -= gui

TARGET = VPR_XLW
TEMPLATE = lib

DEFINES += VPR_XLW_LIBRARY

SOURCES += vpr_xlw.cpp

HEADERS += vpr_xlw.h\
        VPR_XLW_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE18A1895
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = VPR_XLW.dll
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

unix:!macx:!symbian: LIBS += -L$$PWD/lib -lHVDLL
DEFINES += LINUX
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
