#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T13:34:57
#
#-------------------------------------------------

QT += core gui

OUTPUT_DIR_NAME = output
export(OUTPUT_DIR_NAME)

DESTDIR = $$PWD/../$$OUTPUT_DIR_NAME/bin
OBJECTS_DIR = $$PWD/../$$OUTPUT_DIR_NAME/build/QREncryptDll
MOC_DIR = $$PWD/../$$OUTPUT_DIR_NAME/build/QREncryptDll
UI_DIR = $$PWD/../$$OUTPUT_DIR_NAME/build/QREncryptDll

TARGET = QREncryptDll
TEMPLATE = lib

CONFIG += c++11

DEFINES += QRENCRYPTDLL_LIBRARY

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

SOURCES += QREncryptDll.cpp

HEADERS += QREncryptDll.h\
        QREncryptDll_global.h

# =====================================================================================================================
# log4qt 
# ---------------------------------------------------------------------------------------------------------------------
INCLUDEPATH += $$PWD/../log4qt
LOG4QT_DIR = $$PWD/../log4qt

HEADERS += $$LOG4QT_DIR/ilogmsg.h
SOURCES += $$LOG4QT_DIR/ilogmsg.cpp
include($$LOG4QT_DIR/log4qt/log4qt.pri)

# =====================================================================================================================
# QRCode
# ---------------------------------------------------------------------------------------------------------------------
INCLUDEPATH += $$PWD/../QRCode
# 二维码解码包
HEADERS += $$PWD/../QRCode/tcQrencode.h
SOURCES += $$PWD/../QRCode/tcQrencode.cpp
# 第三方源码包 - QRencode
QRENCODE_DIR = $$PWD/../QRCode/qrencode
include($$QRENCODE_DIR/qrencode.pri)

# 二维码解码包
#HEADERS += $$PWD/../QRCode/tcZxing.h
#SOURCES += $$PWD/../QRCode/tcZxing.cpp
# 第三方源码包 - ZXing
#ZXING_DIR = $$PWD/../QRCode/zxing
#include($$ZXING_DIR/zxing.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    QREncryptDll.qrc
