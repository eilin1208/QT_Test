#include "widget.h"
#include "ui_widget.h"
#include "RSUArm/rsuctrl.h"
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "RSUArm/rsuarmcomm.h"
#include "RSUArm/rsudatatype.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //rsuCtrl = new rsuctrl("RSU");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startRsu_clicked()
{
    GetRsuInstance()->StartDev();
    //connect(rsuCtrl,SIGNAL(StatusChanged(int)),this,SLOT(onStatusChangedSlot(int)));
    connect(GetRsuInstance(),SIGNAL(StatusChanged(int)),this,SLOT(onStatusChangedSlot(int)));
    //
    connect(GetRsuInstance(),SIGNAL(signalRecivedFrameData(QByteArray)),this,SLOT(getFrameDataSlot(QByteArray)));
    connect(GetRsuArmCommInstance(),SIGNAL(sendingFrameData(QByteArray)),this,SLOT(getSendingFrameDataSlot(QByteArray)));
    //get frame data
    connect(GetRsuInstance(),SIGNAL(signalWaitInit()),this,SLOT(getB0InfoSlot()));
    connect(GetRsuInstance(),SIGNAL(signalWaitProcessOBUBaseInfo(QVariant,QVariant/*char errCode*/)),this,SLOT(getB2InfoSlot(QVariant,QVariant/*char errCode*/)));
    connect(GetRsuInstance(),SIGNAL(signalWaitProcessOBUinfo(QVariant ,QVariant )),this,SLOT(getB3InfoSlot(QVariant ,QVariant )));
    connect(GetRsuInstance(),SIGNAL(signalWaitProcessCardInfo(QVariant ,QVariant )),this,SLOT(getB4InfoSlot(QVariant ,QVariant )));
    connect(GetRsuInstance(),SIGNAL(signalWaitProcessSuccInfo(QVariant ,QVariant )),this,SLOT(getB5InfoSlot(QVariant ,QVariant )));
    connect(GetRsuInstance(),SIGNAL(signalWaitProcessFreeInfo(QVariant ,QVariant )),this,SLOT(getB6InfoSlot(QVariant ,QVariant )));
}

void Widget::on_stopRsu_clicked()
{
    GetRsuInstance()->StopDev();
}
void Widget::onStatusChangedSlot(int state)
{
    qDebug()<<"statuschangeeSlot";
    QString msg;
    switch(state)
    {
    case 0://DEV_STATUS_OK:
        msg = tr("RSU设备打开成功");
        ui->textBrowser->append(msg);
        break;
    case 1://DEV_STATUS_ParamErr:
        msg = tr("RSU设备打开失败，加载参数错误");
        ui->textBrowser->append(msg);
        break;
    case 2://DEV_STATUS_LibErr:
        msg = tr("RSU设备打开失败，加载类库失败");
        ui->textBrowser->append(msg);
        break;
    case 3://DEV_STATUS_CommErr:
        msg = tr("RSU设备打开失败，加通信失败");
        ui->textBrowser->append(msg);
        break;
    case 4://DEV_STATUS_Offline:
        msg = tr("RSU设备打开失败，脱机");
        ui->textBrowser->append(msg);
        break;
     default :
        break;

    }
}

void Widget::getFrameDataSlot(QByteArray frameData)
{
    QString msg;
    msg = QString(tr("recived the frame data:"));

   ui->sendTextBrowser->append(msg);
   ui->sendTextBrowser->append(frameData.toHex().constData());

   //if()
}

void Widget::getSendingFrameDataSlot(QByteArray frameData)
{
    QString msg;
    msg = QString(tr("send the frame data:"));

    ui->sendTextBrowser->append(msg);
    ui->sendTextBrowser->append(frameData.toHex().constData());
}

void Widget::getB0InfoSlot()
{
    ui->textBrowser->append("GetB0infoslot");

    const QRSUState *rsuState = GetRsuInstance()->GetB0Info();

    QString str;
    str = QString(" rsuState->status:         %1").arg(rsuState->status);
    ui->textBrowser->append(str);

    str = QString(" rsuState->RSUTerminalID1: %1 %2 %3 %4 %5 %6")
            .arg(rsuState->RSUTerminalID1[0])
            .arg(rsuState->RSUTerminalID1[1])
            .arg(rsuState->RSUTerminalID1[2])
            .arg(rsuState->RSUTerminalID1[3])
            .arg(rsuState->RSUTerminalID1[4])
            .arg(rsuState->RSUTerminalID1[5]);
    ui->textBrowser->append(str);

    str = QString(" rsuState->RSUTerminalID2: %1 %2 %3 %4 %5 %6")
            .arg(rsuState->RSUTerminalID2[0])
            .arg(rsuState->RSUTerminalID2[1])
            .arg(rsuState->RSUTerminalID2[2])
            .arg(rsuState->RSUTerminalID2[3])
            .arg(rsuState->RSUTerminalID2[4])
            .arg(rsuState->RSUTerminalID2[5]);
    ui->textBrowser->append(str);

    str = QString(" rsuState->bRsuAlgID:      %1").arg(rsuState->bRsuAlgID);
    ui->textBrowser->append(str);

    str = QString(" rsuState->bRsuManuID:     %1").arg(rsuState->bRsuManuID);
    ui->textBrowser->append(str);

    str = QString(" rsuState->RsuID:          %1").arg(rsuState->RsuID);
    ui->textBrowser->append(str);

    str = QString(" rsuState->RsuVersion:     %1").arg(rsuState->RsuVersion);
    ui->textBrowser->append(str);

}
void Widget::getB2InfoSlot(QVariant frameVar,QVariant errVar)
{
    ui->textBrowser->append("GetB2infoslot");

    QFrame_B2 frameB2;
    frameB2 = frameVar.value<QFrame_B2>();

    RSUERRORCODE errCode;
    errCode = errVar.value<RSUERRORCODE>();

    //continue deal frame
    GetRsuInstance()->ContinueDeal(frameB2.OBUID);

    QString str;
    str = QString("FrameB2 Error Code is:     %1")
            .arg(errCode);
    ui->textBrowser->append(str);

    str = QString(" frameB2->OBUID:            %1")
            .arg(frameB2.OBUID);
    ui->textBrowser->append(str);

    str = QString(" frameB2->ErrorCode:        %1")
            .arg(frameB2.ErrorCode);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IssuerIdentifier: %1 %2 %3 %4 %5 %6 %7 %8")
            .arg((quint8)frameB2.IssuerIdentifier[0])
            .arg((quint8)frameB2.IssuerIdentifier[1])
            .arg((quint8)frameB2.IssuerIdentifier[2])
            .arg((quint8)frameB2.IssuerIdentifier[3])
            .arg((quint8)frameB2.IssuerIdentifier[4])
            .arg((quint8)frameB2.IssuerIdentifier[5])
            .arg((quint8)frameB2.IssuerIdentifier[6])
            .arg((quint8)frameB2.IssuerIdentifier[7]);
    ui->textBrowser->append(str);

    str = QString(" frameB2->ContractSerialNumer:%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16")
            .arg((quint8)frameB2.ContractSerialNumer[0])
            .arg((quint8)frameB2.ContractSerialNumer[1])
            .arg((quint8)frameB2.ContractSerialNumer[2])
            .arg((quint8)frameB2.ContractSerialNumer[3])
            .arg((quint8)frameB2.ContractSerialNumer[4])
            .arg((quint8)frameB2.ContractSerialNumer[5])
            .arg((quint8)frameB2.ContractSerialNumer[6])
            .arg((quint8)frameB2.ContractSerialNumer[7])
            .arg((quint8)frameB2.ContractSerialNumer[8])
            .arg((quint8)frameB2.ContractSerialNumer[9])
            .arg((quint8)frameB2.ContractSerialNumer[10])
            .arg((quint8)frameB2.ContractSerialNumer[11])
            .arg((quint8)frameB2.ContractSerialNumer[12])
            .arg((quint8)frameB2.ContractSerialNumer[13])
            .arg((quint8)frameB2.ContractSerialNumer[14])
            .arg((quint8)frameB2.ContractSerialNumer[15]);
    ui->textBrowser->append(str);

    str = QString(" frameB2->DateofIssue:      %1 %2 %3")
            .arg(frameB2.DateofIssue.date().year())
            .arg(frameB2.DateofIssue.date().month())
            .arg(frameB2.DateofIssue.date().day());
    ui->textBrowser->append(str);

    str = QString(" frameB2->DateofExpire:     %1 %2 %3")
            .arg(frameB2.DateofExpire.date().year())
            .arg(frameB2.DateofExpire.date().month())
            .arg(frameB2.DateofExpire.date().day());
    ui->textBrowser->append(str);

    str = QString(" frameB2->EquitmentStatus:  %1")
            .arg(frameB2.EquitmentStatus);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IsCardInsert:     %1")
            .arg(frameB2.IsCardInsert);
    ui->textBrowser->append(str);

    str = QString(" frameB2->CardType:         %1")
            .arg(frameB2.CardType);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IsCardNormal:     %1")
            .arg(frameB2.IsCardNormal);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IsUnlocked:       %1")
            .arg(frameB2.IsUnlocked);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IsUndisassembled: %1")
            .arg(frameB2.IsUndisassembled);
    ui->textBrowser->append(str);

    str = QString(" frameB2->IsPowerNormal:    %1")
            .arg(frameB2.IsPowerNormal);
    ui->textBrowser->append(str);
}

void Widget::getB3InfoSlot(QVariant frameVar,QVariant errVar)
{
    ui->textBrowser->append("GetB3infoslot");

    QFrame_B3 frameB3;
    frameB3 = frameVar.value<QFrame_B3>();

    RSUERRORCODE errCode;
    errCode = errVar.value<RSUERRORCODE>();

    //continue deal frame
    GetRsuInstance()->ContinueDeal(frameB3.OBUID);

    QString str;
    str = QString("FrameB3 Error Code is:     %1")
            .arg(errCode);
    ui->textBrowser->append(str);
}

void Widget::getB4InfoSlot(QVariant frameVar,QVariant errVar)
{
    ui->textBrowser->append("Getb4infoslot");
    QFrame_B4 frameB4;
    frameB4 = frameVar.value<QFrame_B4>();

    RSUERRORCODE errCode;
    errCode = errVar.value<RSUERRORCODE>();

    //for test
    TCardTollInfo entryInfo;
    entryInfo.clear();
    //write entry infomation  C3

//    GetRsuInstance()->WriteEntryInfo(frameB4.OBUID,entryInfo,false);
   // GetRsuInstance()->WriteEntryInfo(frameB4.OBUID,&entryInfo, false);

    QString str;
    str = QString("FrameB4 Error Code is:     %1")
            .arg(errCode);
    ui->textBrowser->append(str);
}

void Widget::getB5InfoSlot(QVariant frameVar,QVariant errVar )
{
    ui->textBrowser->append("Getb5infoslot");
    QFrame_B5 frameB5;
    frameB5 = frameVar.value<QFrame_B5>();

    RSUERRORCODE errCode;
    errCode = errVar.value<RSUERRORCODE>();

    QString str;
    str = QString("FrameB5 Error Code is:     %1")
            .arg(errCode);
    ui->textBrowser->append(str);
}

void Widget::getB6InfoSlot(QVariant frameVar,QVariant errVar )
{
    ui->textBrowser->append("Getb6infoslot");

    QFrame_B6 frameB6;
    frameB6 = frameVar.value<QFrame_B6>();

    RSUERRORCODE errCode;
    errCode = errVar.value<RSUERRORCODE>();

    QString str;
    str = QString("FrameB6 Error Code is:     %1")
            .arg(errCode);
    ui->textBrowser->append(str);
}

void Widget::on_pushButton_clicked()
{

}
