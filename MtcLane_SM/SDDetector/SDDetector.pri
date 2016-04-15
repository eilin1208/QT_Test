
unix {

    linux-g++ {
        message(Linux)
        LIBS += -L$$PWD/Linux -lQDeviceWatcher
    }

    linux-EMRC-R3-g++ {
        message(ARM_Linux)
        LIBS += -L$$PWD/Arm -lQDeviceWatcher

    }
}

win32 {
    message(Windows)
    LIBS += -L$$PWD/Windows -lQDeviceWatcher2
}

HEADERS += \
        $$PWD/qdevicewatcher.h


