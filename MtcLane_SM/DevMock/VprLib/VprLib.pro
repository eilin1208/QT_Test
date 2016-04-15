#-------------------------------------------------
#
# Project created by QtCreator 2013-01-07T14:40:53
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = VprLib
TEMPLATE = lib

DEFINES += VPRLIB_LIBRARY

SOURCES += vprlib.cpp

HEADERS += vprlib.h\
        VprLib_global.h

include($$PWD/../TcpClient/TcpClient.pri)
