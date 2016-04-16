#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T20:17:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OPETST
TEMPLATE = app

#LIBS += -lsqlite3

INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/lib

CONFIG += c++11
QMAKE_CXXFLAGS += -pedantic

include($$PWD/OPETST.pri)
#include($$PWD/db/db.pri)
include($$PWD/common/common.pri)
include($$PWD/config_builder/config_builder.pri)

RESOURCES += \
    icons.qrc \
    OPETST.qrc \
    devres.qrc


TRANSLATIONS += translations/OPETST_zh_CN.ts \
                translations/OPETST_en.ts
