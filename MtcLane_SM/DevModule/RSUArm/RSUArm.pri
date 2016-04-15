INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

SOURCES += \
    $$PWD/rsutransframe.cpp \
    $$PWD/rsuctrl.cpp \
    $$PWD/rsuarmcomm.cpp

HEADERS += \
    $$PWD/rsudatatype.h \
    $$PWD/rsutransframe.h \
    $$PWD/rsuctrl.h \
    $$PWD/rsuarmcomm.h
unix {

    linux-g++ {
        message(Linux)
        LIBS += -L$$PWD/../SerialPort/lib -lQtSerialPort
    }

    linux-EMRC-R3-g++ {
        message(ARM_Linux)
        LIBS += -L$$PWD/../SerialPort/lib -lQtSerialPort

    }
}

win32 {
    message(Windows)
    LIBS += -L$$PWD/../SerialPort/lib -lQtSerialPortd1
}
INCLUDEPATH += $$PWD/../SerialPort/include
DEPENDPATH += $$PWD/../SerialPort/include
