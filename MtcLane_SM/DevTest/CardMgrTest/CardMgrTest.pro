#-------------------------------------------------
#
# Project created by QtCreator 2013-01-05T11:15:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CardMgrTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
include($$PWD/../../CommonModule/common.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../DevModule/CardMgrArm/CardMgrArm.pri)
