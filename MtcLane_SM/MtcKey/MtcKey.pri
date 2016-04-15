# *******************************************************************************
#
# package:     MtcKey
# file:        MtcKey.pri
# created:     November 2011
# denpends on: 
#
# *******************************************************************************

INCLUDEPATH += $$PWD/..

DEPENDPATH += $$PWD/..

HEADERS += \
    $$PWD/MtcKeyDef.h \
    $$PWD/MtcKeySignalTransition.h \
    $$PWD/MtcLaneState.h \
    $$PWD/MtcOpWidget.h

SOURCES += \
    $$PWD/MtcKeyDef.cpp \
    $$PWD/MtcKeySignalTransition.cpp \
    ../MtcKey/MtcOpWidget.cpp
