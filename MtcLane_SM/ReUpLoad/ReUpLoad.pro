#-------------------------------------------------
#
# Project created by QtCreator 2013-02-28T10:57:04
#
#-------------------------------------------------

QT       += core gui network sql

TARGET = ReUpLoad
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include($$PWD/../LogModule/LogModule.pri)
include($$PWD/../PacketSendModule/PacketSendModule.pri)
include($$PWD/../log4qt/log4qt.pri)
include($$PWD/../CommonModule/common.pri)

