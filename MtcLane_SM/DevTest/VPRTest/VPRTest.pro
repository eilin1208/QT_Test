#-------------------------------------------------
#
# Project created by QtCreator 2013-02-19T11:02:49
#
#-------------------------------------------------

QT       += core gui network

TARGET = VPRTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

include($$PWD/../../DevModule/VPRArm/VPRArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)