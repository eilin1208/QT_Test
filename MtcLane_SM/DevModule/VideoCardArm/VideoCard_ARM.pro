#-------------------------------------------------
#
# Project created by QtCreator 2012-03-10T15:44:13
#
#-------------------------------------------------

QT       += core

QT       += gui

TARGET = VideoCard_ARM
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    videocard_arm.cpp

HEADERS += \
    videocard_arm.h \
video_lib.h

LIBS += -L$$PWD -lVideoCap

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

unix:!macx:!symbian: LIBS += -ljpeg


