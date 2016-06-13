#-------------------------------------------------
#
# Project created by QtCreator 2016-06-04T11:04:34
#
#-------------------------------------------------

QT       -= gui

TARGET = QRencode
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qrencode.cpp \
    ../qrencode-3.4.4/bitstream.c \
    ../qrencode-3.4.4/mask.c \
    ../qrencode-3.4.4/mmask.c \
    ../qrencode-3.4.4/mqrspec.c \
    ../qrencode-3.4.4/qrenc.c \
    ../qrencode-3.4.4/qrencode.c \
    ../qrencode-3.4.4/qrinput.c \
    ../qrencode-3.4.4/qrspec.c \
    ../qrencode-3.4.4/rscode.c \
    ../qrencode-3.4.4/split.c

HEADERS += qrencode.h \
    ../qrencode-3.4.4/bitstream.h \
    ../qrencode-3.4.4/mask.h \
    ../qrencode-3.4.4/mmask.h \
    ../qrencode-3.4.4/mqrspec.h \
    ../qrencode-3.4.4/qrencode.h \
    ../qrencode-3.4.4/qrencode_inner.h \
    ../qrencode-3.4.4/qrinput.h \
    ../qrencode-3.4.4/qrspec.h \
    ../qrencode-3.4.4/rscode.h \
    ../qrencode-3.4.4/split.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
