#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new QCardReader("CardReader");
    tmpDev = m_mapDevice.begin().value();
    reader = (QCardReader*)m_mapDevice.value(0, NULL);
    tmpcardid = "0";
    this->setWindowTitle(tr("CardTest"));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pb_init_clicked()
{

    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->t_pstext->setText("设备初始化失败！");

    }
    else
        ui->t_pstext->setText("设备初始化成功！");

}

void Widget::on_pb_close_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->t_pstext->setText("设备关闭失败！");

    }
    else
        ui->t_pstext->setText("设备关闭成功！");
}

void Widget::on_pb_readcard_clicked()
{
    TProCardBasicInfo ProCardBasicInfo;
    quint32 dwBalance = 0;
    TCardTollInfo CardTollInfo;
    TTollCardBasicInfo TollCardInfo;
    TProCardConsumeInfo pConsumeInfo;
    CardTollInfo.clear();
    TollCardInfo.clear();
    bool ok = reader->OpenCard();
    if(ok)
    {
        bool bTollCard;
        bTollCard = reader->ReadTollCard(TollCardInfo,CardTollInfo);
        if(bTollCard == true)
        {	//通行卡
            tmpcardid = QString::number(TollCardInfo.M1BasicInfo.dwCardNum);
            QString str=tr("卡序列号：%1;类型：%2；卡印刷号：%3\n").arg(reader->GetCardSerialID()).arg(reader->GetCardType()).arg(tmpcardid);
            ui->t_pstext->setText(str);
            getCardTollInfo(CardTollInfo);
            ui->t_pstext->append(m_str);
        }
        else
        {	//pro卡，云通卡
            reader->ReadProCard(ProCardBasicInfo,dwBalance,CardTollInfo);
            tmpcardid = QString::fromLocal8Bit(ProCardBasicInfo.szCardNo,16);
            QString str=tr("卡序列号：%1;类型：%2；卡印刷号：%3\n").arg(reader->GetCardSerialID()).arg(reader->GetCardType()).arg(tmpcardid);
            ui->t_pstext->setText(str);
            getCardTollInfo(CardTollInfo);
            ui->t_pstext->append(m_str);
        }
    }
    else
    {
        ui->t_pstext->setText("读卡失败");
    }
    m_str.clear();
}

void Widget::on_writecard_clicked()
{
    TProCardBasicInfo ProCardBasicInfo;
    quint32 dwBalance = 0;
    TCardTollInfo CardTollInfo;
    TTollCardBasicInfo TollCardInfo;
    CardTollInfo.clear();
    TollCardInfo.clear();
    bool readok;
    QString str;
    bool ok = reader->OpenCard();
    if(ok)
    {
        bool bTollCard;
        bTollCard = reader->ReadTollCard(TollCardInfo,CardTollInfo);
        if(bTollCard == true)
        {
            tmpcardid = QString::number(TollCardInfo.M1BasicInfo.dwCardNum);
            //str=tr("卡序列号：%1;类型：%2；卡印刷号：%c 3").arg(reader->GetCardSerialID()).arg(reader->GetCardType()).arg(tmpcardid);
            CardTollInfo.dwStaId = (quint32)ui->t_entrystation->text().toInt();
            CardTollInfo.bLaneId = (quint8)ui->t_laneid->text().toInt();
            CardTollInfo.dwOpId = (quint32)ui->t_oper->text().toInt();
            CardTollInfo.bShiftId = (quint8)ui->t_shift->text().toInt();
            CardTollInfo.bVehClass = (quint8)ui->t_vctype->text().toInt();
            QByteArray tmp = ui->t_vlp->text().toLocal8Bit();
            memcpy(CardTollInfo.szVehPlate,tmp,16);
            CardTollInfo.bVehPlateColor = (quint8)ui->t_vlpc->text().toInt();
            CardTollInfo.bVehTollType = (quint8)ui->t_vccode->text().toInt();
            CardTollInfo.dwCardBoxId = (quint32)ui->t_cardboxid->text().toInt();
            CardTollInfo.PassTime = QDateTime::currentDateTime();
            readok =  reader->WriteEntryInfo(tmpcardid,CardTollInfo);

        }
        else
        {
            reader->ReadProCard(ProCardBasicInfo,dwBalance,CardTollInfo);
            tmpcardid = QString::fromLocal8Bit(ProCardBasicInfo.szCardNo,16);
            //str=tr("卡序列号：%1;类型：%2；卡印刷号：%3").arg(reader->GetCardSerialID()).arg(reader->GetCardType()).arg(tmpcardid);
            CardTollInfo.dwStaId = (quint32)ui->t_entrystation->text().toInt();
            CardTollInfo.bLaneId = (quint8)ui->t_laneid->text().toInt();
            CardTollInfo.dwOpId = (quint32)ui->t_oper->text().toInt();
            CardTollInfo.bShiftId = (quint8)ui->t_shift->text().toInt();
            CardTollInfo.bVehClass = ProCardBasicInfo.bVehClass;//(quint8)ui->t_vctype->text().toInt();
            //QByteArray tmp = ui->t_vlp->text().toLocal8Bit();
            memcpy(CardTollInfo.szVehPlate,ProCardBasicInfo.szVehPlate,16);
            CardTollInfo.bVehPlateColor = ProCardBasicInfo.bVehPlateColor;//(quint8)ui->t_vlpc->text().toInt();
            CardTollInfo.bVehTollType = ProCardBasicInfo.bVehTollType;//(quint8)ui->t_vccode->text().toInt();
            CardTollInfo.dwCardBoxId = (quint32)ui->t_cardboxid->text().toInt();
            CardTollInfo.PassTime = QDateTime::currentDateTime();
            readok = reader->WriteEntryInfo(tmpcardid,CardTollInfo);
        }
        if(readok == true)
        {
            ui->t_pstext->setText("写卡成功");
        }
        else
        {
            ui->t_pstext->setText("写卡失败");
        }
        reader->CloseCard();
    }
    else
    {
        ui->t_pstext->setText("写卡前，读卡失败.");
    }
}

void Widget::getCardTollInfo(TCardTollInfo CardTollInfo)
{
    m_str.append(tr("入口路网号:%1;入口收费站编码：%2;入口车道号：%3\n").arg(CardTollInfo.wNetworkId).arg(CardTollInfo.dwStaId).arg(CardTollInfo.bLaneId));
    m_str.append(tr("通行状态：%1;维护发卡标识:%2;\n").arg(CardTollInfo.bPassStatus).arg(CardTollInfo.bMaintenance));
    char passtime[14];
    FormatTimeToChar14(CardTollInfo.PassTime,passtime);
    char psamtermid[6];
    memcpy(psamtermid,CardTollInfo.baPsamTermId,6);
    m_str.append(tr("入口时间：%1；入口流水记录号：%2;入口操作员号：%3;入口班次：%4;\n入口车型：%5;入口车类：%6;入口PSAM卡号：%7\n").arg(passtime)
            .arg(CardTollInfo.bRecIndex).arg(CardTollInfo.dwOpId).arg(CardTollInfo.bShiftId).arg(CardTollInfo.bVehTollType)
            .arg(CardTollInfo.bVehClass).arg(psamtermid));
    QString platecolor;
    switch(CardTollInfo.bVehPlateColor)
    {
    case 0: platecolor = tr("蓝色");
    case 1: platecolor = tr("黄色");
    case 2: platecolor = tr("黑色");
    case 3: platecolor = tr("白色");
    default:platecolor = tr("无");
    }
    m_str.append(tr("卡片使用次数：%1;卡盒逻辑号：%2;\n车牌号：%3;车牌颜色：%4;\n").arg(CardTollInfo.dwWriteCount)
            .arg(CardTollInfo.dwCardBoxId).arg(CardTollInfo.szVehPlate).arg(platecolor));
    QString bCrc;
    if(CardTollInfo.bCRC == true)
    {
        bCrc = tr("是");
    }
    else
    {
        bCrc = tr("否");
    }
    char FlagstaInfo[10];
    memcpy(FlagstaInfo,CardTollInfo.baFlagStaInfo,10);
    m_str.append(tr("标志站个数：%1；标志站信息：%2；已校验：%3").arg(CardTollInfo.bFlagStaCount)
            .arg(FlagstaInfo).arg(bCrc));
}
