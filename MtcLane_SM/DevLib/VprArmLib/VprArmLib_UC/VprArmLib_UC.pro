#-------------------------------------------------
#
# Project created by QtCreator 2013-01-29T11:47:37
#
#-------------------------------------------------

QT       -= gui

TARGET = VprArmLib_UC
TEMPLATE = lib

DEFINES += VPRARMLIB_UC_LIBRARY

SOURCES += vprarmlib_uc.cpp

HEADERS += vprarmlib_uc.h\
        VprArmLib_UC_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

