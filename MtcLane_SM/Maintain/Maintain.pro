#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T16:33:32
#
#-------------------------------------------------

QT       += core gui network sql

TARGET = Maintain
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    maintaincfg.cpp \
    formfile.cpp \
    formmain.cpp \
    formlog.cpp \
    formcfgfile.cpp \
    autoupdate.cpp \
    formlogin.cpp \
    formfiledialog.cpp\
    iocardtest.cpp \
    formdevtest.cpp \
    testfd.cpp \
    testprinter.cpp \
    testwt.cpp \
    testvpr.cpp \
    testvideocard.cpp \
    testcardreader.cpp \
    qtelnetperso.cpp \
    mytelnetwidget.cpp \
    formtelnet.cpp \
    formsoftupdate.cpp \
    maintaindatebase.cpp \
    formlogftp.cpp

HEADERS  += widget.h \
    maintaincfg.h \
    formfile.h \
    formmain.h \
    formlog.h \
	formcfgfile.h \
    globalvalue.h \
    globaltypedef.h \
    autoupdate.h \
    formlogin.h \
    formfiledialog.h\
    iocardtest.h \
    formdevtest.h \
    testfd.h \
    testprinter.h \
    testwt.h \
    testvpr.h \
    testvideocard.h \
    testcardreader.h \
    qtelnetperso.h \
    mytelnetwidget.h \
    formtelnet.h \
    formsoftupdate.h \
    maintaindatebase.h \
    formlogftp.h

FORMS    += widget.ui \
    formfile.ui \
    formmain.ui \
    formlog.ui \
    formcfgfile.ui \
    formlogin.ui \
    formfiledialog.ui\
    iocardtest.ui \
    formdevtest.ui \
    testfd.ui \
    testprinter.ui \
    testwt.ui \
    testvpr.ui \
    testvideocard.ui \
    testcardreader.ui \
    formtelnet.ui \
    formsoftupdate.ui \
    formlogftp.ui

include($$PWD/../FtpModule/Ftp.pri)
include($$PWD/../CommonModule/common.pri)
include($$PWD/../log4qt/log4qt.pri)
include($$PWD/../DevModule/IoBoardArm/IoBoardArm.pri)
include($$PWD/../DevModule/VideoCardArm/VideoCardArm.pri)
include($$PWD/../DevModule/CardReaderArm/CardReaderArm.pri)
include($$PWD/../DevModule/CardMgrArm/CardMgrArm.pri)
include($$PWD/../DevModule/BaseInterface/DevBaseInterface.pri)
include($$PWD/../LogModule/LogModule.pri)
include($$PWD/../DevModule/VDMArm/VDMArm.pri)
include($$PWD/../DevModule/VPRArm/VPRArm.pri)
include($$PWD/../DevModule/FareDisplayArm/FareDisplayArm.pri)
include($$PWD/../DevModule/PrinterArm/PrinterArm.pri)
include($$PWD/../ParamModule/ParamModule.pri)
include($$PWD/../FareCalc/FareCalc.pri)
include($$PWD/../DevModule/WtArm/WtArm.pri)
