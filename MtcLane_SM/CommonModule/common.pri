INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

HEADERS += \   
    $$PWD/LaneType.h \
    $$PWD/laneutils_new.h \
    $$PWD/CardFileDef.h \
    #$$PWD/LaneCrypto.h \
    $$PWD/vehplatefunc.h \
    $$PWD/qlanesocket.h \
    $$PWD/qpausablethread.h \
    $$PWD/ilogmsg.h \
    $$PWD/cardfileconverter.h \
    $$PWD/MsgDefine.h \
    $$PWD/basesaver.h

SOURCES += \
    $$PWD/laneutils_new.cpp \
    #$$PWD/LaneCryptoReplace.cpp \
    $$PWD/vehplatefunc.cpp  \
    $$PWD/qlanesocket.cpp \
    $$PWD/qpausablethread.cpp \
    $$PWD/ilogmsg.cpp \
    $$PWD/LaneType.cpp \
    $$PWD/cardfileconverter.cpp \
    $$PWD/MsgDefine.cpp \
    $$PWD/basesaver.cpp

