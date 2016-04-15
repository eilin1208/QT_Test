#-------------------------------------------------
#
# Project created by QtCreator 2013-01-30T16:24:51
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = FtpModuleTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

include($$PWD/../../FtpModule/Ftp.pri)
