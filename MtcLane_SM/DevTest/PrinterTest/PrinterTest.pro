#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T09:42:28
#
#-------------------------------------------------

QT       += core gui network

TARGET = PrinterTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

include($$PWD/../../DevModule/PrinterArm/PrinterArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)