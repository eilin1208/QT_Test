#-------------------------------------------------
#
# Project created by QtCreator 2012-12-01T14:25:10
#
#-------------------------------------------------

QT       -= gui

TARGET = CardMgrLib
TEMPLATE = lib

DEFINES += CARDMGRLIB_LIBRARY

SOURCES += cardmgrlib.cpp \
    cardmgrdecoder.cpp \
    cardmgrhandler.cpp \
    cardmgrencoder.cpp

HEADERS += cardmgrlib.h\
        CardMgrLib_global.h \
    datadef.h \
    cardmgrdecoder.h \
    cardmgrhandler.h \
    cardmgrencoder.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE424BBA6
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = CardMgrLib.dll
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

win32: LIBS += -L$$PWD/../../DevModule/SerialPort/lib/ -llibQtSerialPort1

INCLUDEPATH += $$PWD/../../DevModule/SerialPort/include
DEPENDPATH += $$PWD/../../DevModule/SerialPort/include
