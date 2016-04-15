#-------------------------------------------------
#
# Project created by QtCreator 2013-02-04T15:11:15
#
#-------------------------------------------------

QT       += core gui network

TARGET = VideoCardTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

include($$PWD/../../DevModule/VideoCardArm/VideoCardArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)