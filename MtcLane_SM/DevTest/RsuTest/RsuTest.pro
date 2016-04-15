#-------------------------------------------------
#
# Project created by QtCreator 2012-10-8T17:29:00
#
#-------------------------------------------------

QT       += core gui network sql

TARGET = RsuTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \

HEADERS  += widget.h \

FORMS    += widget.ui

include($$PWD/../../CommonModule/common.pri)
include($$PWD/../../log4qt/log4qt.pri)
include($$PWD/../../LogModule/LogModule.pri)
include($$PWD/../../ParamModule/ParamModule.pri)
include($$PWD/../../DevModule/RSUArm/RSUArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
