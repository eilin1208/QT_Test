#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T11:52:28
#
#-------------------------------------------------

QT       += core gui network

TARGET = FDTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

include($$PWD/../../DevModule/FareDisplayArm/FareDisplayArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)