#-------------------------------------------------
#
# Project created by QtCreator 2012-12-13T11:13:13
#
#-------------------------------------------------

QT       += core gui network

TARGET = CardTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui


include($$PWD/../../DevModule/CardReaderArm/CardReaderArm.pri)
include($$PWD/../../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../../LogModule/LogModule.pri)
include($$PWD/../../CommonModule/common.pri)
include($$PWD/../../log4qt/log4qt.pri)
include($$PWD/../../ParamModule/ParamModule.pri)
