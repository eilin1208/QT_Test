INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

HEADERS += \
    $$PWD/video_lib.h \
    $$PWD/videocardarm.h

SOURCES += \
    $$PWD/videocardarm.cpp\

linux-EMRC-R3-g++ {

unix:LIBS += -ljpeg
}

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

