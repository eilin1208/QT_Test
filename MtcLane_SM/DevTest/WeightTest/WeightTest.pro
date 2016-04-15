#-------------------------------------------------
#
# Project created by QtCreator 2013-02-05T16:19:01
#
#-------------------------------------------------

QT       += core gui network

TARGET = WeightTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

include($$PWD/../../DevModule/WtArm/WtArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../CommonModule/common.pri)
include($$PWD/../../FareCalc/FareCalc.pri)
include($$PWD/../../LogModule/LogModule.pri)
include($$PWD/../../log4qt/log4qt.pri)
include($$PWD/../../ParamModule/ParamModule.pri)