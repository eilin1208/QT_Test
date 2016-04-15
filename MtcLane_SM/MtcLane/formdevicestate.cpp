#include "formdevicestate.h"
#include "ui_formdevicestate.h"
#include "lanectrl.h"
#include "laneinfo.h"
#include "shiftsum.h"
const QString SCIRCLEHASCAR = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/circle.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");
const QString SCIRCLENOCAR = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/circle0.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");
const QString LEVERUP = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/barup.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");
const QString LEVERDOWN = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/sss.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString LEVERLOCK = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/baruplock.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString LANELIGHTGREEN = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/TranGreen.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString LANELIGHTRED = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/TranRed.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString CANLIGHTRED = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/nopass.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString CANLIGHTGREEN = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/1-2.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString PRINTEROK = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/Printer.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString PRINTERFAIL = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/PrinterFail.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString CARDEROK = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/Reader.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

const QString CARDERFAIL = QObject::tr("%1;%2;%3;%4")
.arg("border-image: url(:/images/image/Readerr.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("border:1px solid")
.arg("border-radius:5px");

FormDeviceState::FormDeviceState(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDeviceState)
{
    ui->setupUi(this);
    //连接IO改变信号，更新界面
    connect(getDeviceFactory()->getIOBoard(), SIGNAL(OutputChanged(int,int)),
            this, SLOT(IoOuputChanged(int,int)));
    //连接设备状态信号，更新界面
    connect(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32, quint8)),
            this, SLOT(IoInputChanged(quint32,quint8)));
    connect(getLaneCtrl(), SIGNAL(AllowCarPassing(int)), this, SLOT(allowCarPass(int)));
    //设置费显工作模式
    //TODO：费显工作模式设置不应该放在这里
    //根据车道类型确定是否显示打印机接口
    if(!getLaneInfo()->isEntryLane())
    {
        ui->lbPrinter->show();
    }
    else
    {
        ui->lbPrinter->hide();
    }
}

FormDeviceState::~FormDeviceState()
{
    delete ui;
}

void FormDeviceState::IoOuputChanged(int nDevice, int nStatus)
{
    switch(nDevice)
    {
    case DO_CanopyLightGreen:
        ui->lbCanLight->setStyleSheet(nStatus == 0? CANLIGHTRED : CANLIGHTGREEN);
        break;
    case DO_PassLightGreen:
        ui->lbLaneLight->setStyleSheet(nStatus == 0? LANELIGHTRED : LANELIGHTGREEN);
        break;
    default:
        break;
    }
}

void FormDeviceState::IoInputChanged(quint32 dev, quint8 nStatus)
{

    switch(dev)
    {
    case DI_LoopFront:
        ui->lbExists->setStyleSheet(nStatus == 1 ? SCIRCLEHASCAR : SCIRCLENOCAR);
        break;
    case DI_LoopBack:
        ui->lbCross->setStyleSheet(nStatus == 1 ? SCIRCLEHASCAR : SCIRCLENOCAR);
        break;
    case DI_BarrierUp:
        if(!getDeviceFactory()->getIOBoard()->IsLockBar())
        {
            ui->lbBar->setStyleSheet(nStatus == 1 ? LEVERUP : LEVERDOWN);
        }
        break;
    case DI_BarrierDown:
        ui->lbBar->setStyleSheet(nStatus == 1 ? LEVERDOWN : LEVERUP);
        break;
    default:
        break;
    }
}

void FormDeviceState::printerStatusChanged(int status)
{
    if(status == DEV_STATUS_OK)
    {
        ui->lbPrinter->setStyleSheet(PRINTEROK);
    }
    else
    {
        ui->lbPrinter->setStyleSheet(PRINTERFAIL);
    }
}

void FormDeviceState::deviceStatusChanged(int status)
{
    if(sender() == getDeviceFactory()->getPrinter())
    {
        if(status == DEV_STATUS_OK)
        {
            ui->lbPrinter->setStyleSheet(PRINTEROK);
        }
        else
        {
            ui->lbPrinter->setStyleSheet(PRINTERFAIL);
        }
    }
    if(sender() == getDeviceFactory()->getCardReader(DEV_CardReader1))
    {
        if(status == DEV_STATUS_OK)
            ui->lbCardReader->setStyleSheet(CARDEROK);
        else
            ui->lbCardReader->setStyleSheet(CARDERFAIL);
    }

    //设置设备状态 by xiangkai
    getLaneCtrl()->getTollItem()->SaveDevStatus(
                getDeviceFactory()->getMapDevice()->key((DevBaseInterface*)sender()),
                status);
}

void FormDeviceState::allowCarPass(int nNum)
{
    ui->lbPassCarsNum->setText(QString::number(nNum));
}

void FormDeviceState::LockBar(bool bLock)
{
    if(bLock)
    {
        ui->lbBar->setStyleSheet(LEVERLOCK);
    }
    else
    {
        ui->lbBar->setStyleSheet(LEVERDOWN);
    }
}

void FormDeviceState::Init()
{
    QMap< DEVNO, DevBaseInterface* >::iterator it = getDeviceFactory()->getMapDevice()->begin();
    DevBaseInterface* tmpDev = NULL;
    for(;it!= getDeviceFactory()->getMapDevice()->end();it++)
    {
        tmpDev = it.value();
        connect(tmpDev,SIGNAL(StatusChanged(int)),this,SLOT(deviceStatusChanged(int)));
        tmpDev->sendSingle(tmpDev->GetStatus());
        //保存设备状态
        getLaneCtrl()->getTollItem()->SaveDevStatus(it.key(),tmpDev->GetStatus());
    }

}

