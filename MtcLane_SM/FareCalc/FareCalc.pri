# *******************************************************************************
#
# package:     FareCalc
# file:        FareCalc.pri
# created:     November 2011
# denpends on: ParamModule
#              CommonModule
#
# *******************************************************************************

INCLUDEPATH += $$PWD/..

DEPENDPATH += $$PWD/..

HEADERS += \
    $$PWD/FareCalc.h \
    $$PWD/FareCalcFunc.h \
    $$PWD/freeregionclass.h \
    $$PWD/vehtollinfoclass.h \
    $$PWD/farequeryresultclass.h

SOURCES += \
    $$PWD/FareCalc.cpp \
    $$PWD/FareCalcFunc.cpp \
    $$PWD/freeregionclass.cpp \
    $$PWD/vehtollinfoclass.cpp \
    $$PWD/farequeryresultclass.cpp
