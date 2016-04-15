#-------------------------------------------------
#
# Project created by QtCreator 2012-09-13T23:17:58
#
#-------------------------------------------------

QT       += core gui network

TARGET = IoCardTest
TEMPLATE = app


SOURCES += main.cpp\
        iocardtest.cpp

HEADERS  += iocardtest.h

FORMS    += iocardtest.ui

include($$PWD/../../DevModule/IoBoardArm/IoBoardArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)