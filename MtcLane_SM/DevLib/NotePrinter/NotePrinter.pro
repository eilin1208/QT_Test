#-------------------------------------------------
#
# Project created by QtCreator 2012-08-22T13:49:48
#
#-------------------------------------------------

QT       -= gui

TARGET = NotePrinter
TEMPLATE = lib

DEFINES += NOTEPRINTER_LIBRARY

SOURCES += noteprinter.cpp \
    qparportnative.cpp \
    qparport.cpp

HEADERS += noteprinter.h\
        NotePrinter_global.h \
    qparportnative.h \
    qparport.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0183473
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = NotePrinter.dll
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

unix:!macx:!symbian: LIBS += -L$$PWD/lib/ -lQtSerialPort

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
