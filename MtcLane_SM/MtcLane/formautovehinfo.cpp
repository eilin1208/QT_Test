#include "formautovehinfo.h"
#include "ui_formautovehinfo.h"
#include "entrycardinfoprocessor.h"
#include "datafilemgr.h"
#include "formmainwindow.h"
#include "formvpr.h"
#include "lanectrl.h"
#include "laneinfo.h"
#include "entrytollitem.h"
FormAutoVehInfo::FormAutoVehInfo(QWidget *parent, QLayout* layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormAutoVehInfo), m_logWidget(NULL), m_cardMgrPos(-1), m_isFirstPush(true)
{
    ui->setupUi(this);
}

FormAutoVehInfo::~FormAutoVehInfo()
{
    delete ui;
}

void FormAutoVehInfo::handleButtonPush(int pos)
{
    //根据上下机位，打开不同的卡读写器线程,上机位对应卡读写器1，下机位对应卡读写器2
    if(m_isFirstPush)
    {
        m_isFirstPush = false;
    }
    else
    {
        ::showWarning(tr("等待卡机处理卡片中，不响应用户按键取卡"));
        return;
    }

    m_cardMgrPos = pos;
    if(pos == 0)
    {
        getDeviceFactory()->getCardReader(DEV_CardReader1)->StartCardDetection();
    }
    else if(pos == 1)
    {
        getDeviceFactory()->getCardReader(DEV_CardReader2)->StartCardDetection();
    }
    //根据上下机位，显示车型、车类信息
    if(pos == 1)
    {
        ui->lbVehClass->setText(QString::fromUtf8("一型客"));
        getLaneInfo()->setVehClass(VC_Car1);
        getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = VC_Car1;
    }
    else if(pos == 0)
    {
        ui->lbVehClass->setText(QString::fromUtf8("五型货"));
        getLaneInfo()->setVehClass(VC_Truck5);
        getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = VC_Truck5;
    }
    ui->lbVehType->setText(QString::fromUtf8("普通车"));
    getLaneCtrl()->getTollItem()->m_VehInfo.VehType = VT_Normal;

    ::showWarning(tr("用户按%1机位按键取卡，自动设置车型为[%2],车类为[%3]")
            .arg(pos == 0? QString::fromUtf8("上") : QString::fromUtf8("下"))
            .arg(ui->lbVehClass->text()).arg(ui->lbVehType->text()));
}

int FormAutoVehInfo::getCardReaderId(QObject *sender)
{
    for(int i = DEV_CardReader1; i <= DEV_CardReader4; i++)
    {
        if(sender == getDeviceFactory()->getCardReader((DEVNO)i)->getDetectThread())
        {
            return i;
        }
    }
    return -1;
}

void FormAutoVehInfo::showVehProInfo()
{
    EntryTollItem* toll = (EntryTollItem*)getLaneCtrl()->getTollItem();
    ui->lbVehClass->setText(GetVehClassName((CVehClass)toll->getProCardBasicInfo().bVehClass));
    QString sColor = GetVehColorQString(toll->getProCardBasicInfo().bVehPlateColor);

    ui->lbVehPlate->setText(sColor + toll->getProCardBasicInfo().szVehPlate);
    ui->lbVehType->setText(QString::fromUtf8("普通车"));
    //设置到报文内
    toll->m_VehInfo.VehClass = (CVehClass)toll->getProCardBasicInfo().bVehClass;
    toll->m_VehInfo.VehType = VT_Normal;
    toll->m_VehInfo.nVehPlateColor = toll->getProCardBasicInfo().bVehPlateColor;
    strcpy(toll->m_VehInfo.szVehPlate, toll->getProCardBasicInfo().szVehPlate);
    ::showWarning(tr("用户刷云通卡，卡内车型为[%1],车牌为[%2]")
            .arg(ui->lbVehClass->text()).arg(ui->lbVehPlate->text()));
}


void FormAutoVehInfo::handleCard(quint32 cardId, int cardType)
{
    bool rlt = false;
    int readerId = getCardReaderId(sender());
    QCardReader* pReader = getDeviceFactory()->getCardReader((DEVNO)readerId);
    pReader->StopCardDetection();
    //判断卡类型，根据卡类型分别处理
    if(cardType == TYPE_PRO)
    {
        ::showWarning(QString::fromUtf8("正在处理云通卡，卡号为[%1]").arg(cardId));
        //这里需要继续处理写卡交易
        rlt = getEntryCardProcessor()->Processopencardevent_Procard(readerId);
        //如果交易失败，则继续检测卡片，pro卡检测线程不停止
        if(!rlt)
        {
            ::showWarning(QString::fromUtf8("云通卡[%1]处理失败，重新读卡").arg(cardId));
            pReader->StartCardDetection();
        }
        else
        {
            //显示pro信息
            ::showWarning(QString::fromUtf8("云通卡[%1]处理成功，正在放行").arg(cardId));
            showVehProInfo();
            emit handleProCardSuccess();
        }
    }
    else if(TYPE_M1_S50_BLOCK == cardType || TYPE_M1_S50_MAD == cardType ||
            TYPE_M1_S70_BLOCK == cardType || TYPE_M1_S70_MAD == cardType)
    {

        rlt = getEntryCardProcessor()->ProcessOpenCardEvent_TollCard(readerId);
        CardMgrArm* pMgr = getDeviceFactory()->getCardMgr();

        if(rlt)
        {
            ::showWarning(tr("通行卡处理成功，[%1]机位正在出卡...").arg(m_cardMgrPos == 0? tr("上") : tr("下")));
            //出卡
            if(m_cardMgrPos == 0)
            {
                //设置卡号
                getEntryCardProcessor()->setCardBoxId(getLaneInfo()->getBoxIndex(pMgr->getUpChannelId()));
                pMgr->UpCard(pMgr->getUpChannelId());
            }
            else if(m_cardMgrPos == 1)
            {
                getEntryCardProcessor()->setCardBoxId(getLaneInfo()->getBoxIndex(pMgr->getDownChannelId()));
                pMgr->UpCard(pMgr->getDownChannelId());
            }
        }
        else
        {
            ::showWarning(tr("通行卡处理失败，[%1]机位正在进行坏卡处理...").arg(m_cardMgrPos == 0? tr("上") : tr("下")));

            //坏卡处理
            if(m_cardMgrPos == 0)
            {
                pMgr->BadCard(pMgr->getUpChannelId());
            }
            else if(m_cardMgrPos == 1)
            {
                pMgr->BadCard(pMgr->getDownChannelId());
            }
            //取消第一次按键操作
            m_isFirstPush = true;
        }
        //通行卡检测线程无论卡片是否处理成功，都需要停止，等待下次用户按键事件到来再打开
        pReader->StopCardDetection();
    }
}

void FormAutoVehInfo::getVpr(int vprIndex)
{
    VPRArm* pVpr = getDeviceFactory()->getVpr();
    const CVPRResult* pResult = pVpr->GetCurrentResult();
    //如果此时没有车牌，或者显示无车牌
    if(ui->lbVehPlate->text().isEmpty())
    {
        ::showWarning(tr("车牌识别设备识别到车牌[%1]").arg(pResult->sVehPlate));

        //显示自动识别车牌
        ui->lbVehPlate->setText( GetVehColorQString(pResult->nColor) + pResult->sVehPlate);
        getLaneInfo()->setVehPlateColor(pResult->nColor);
        getLaneInfo()->setVehPlate(pResult->sVehPlate);
        //getLaneCtrl()->getTollItem()->m_VehInfo.nAutoVehPlateColor = getLaneInfo()->getVehPlateColor();
        getLaneCtrl()->getTollItem()->m_VehInfo.nVehPlateColor = getLaneInfo()->getVehPlateColor();
    }
}

void FormAutoVehInfo::getDIStatus(quint32 input, quint8 status)
{
    //当接到后线圈无车信号时，再次打开pro卡读写器
    if(input == DI_LoopBack && status == 0)
    {
        getDeviceFactory()->getCardReader(DEV_CardReader3)->StartCardDetection();
        getDeviceFactory()->getCardReader(DEV_CardReader4)->StartCardDetection();

    }
    if(input == DI_LoopBack && status == 1)
    {
        //清除车辆信息
        this->clearData();
    }
}

void FormAutoVehInfo::login(bool isLogin)
{
    if(isLogin)
    {
        //连接处理卡信号槽
        for(int i = DEV_CardReader1; i <= DEV_CardReader4; i++)
        {

            connect(getDeviceFactory()->getCardReader((DEVNO)i)->getDetectThread(),
                    SIGNAL(hasOneCard(quint32,int)),
                    this, SLOT(handleCard(quint32,int)));
        }
        //连接卡机信号槽
        connect(getDeviceFactory()->getCardMgr(), SIGNAL(buttonPush(int)),
                this, SLOT(handleButtonPush(int)));
        //连接车牌识别信号槽
        FormVpr *pVpr = NULL;
        getWidgetByType(emFormVpr, (QWidget**)&pVpr);
        connect(pVpr, SIGNAL(onGetVpr(int)), this, SLOT(getVpr(int)));

        //连接过车信号
        connect(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)),
                this, SLOT(getDIStatus(quint32,quint8)));
    }
    else
    {
        //连接处理卡信号槽
        for(int i = DEV_CardReader1; i <= DEV_CardReader4; i++)
        {

            disconnect(getDeviceFactory()->getCardReader((DEVNO)i)->getDetectThread(),
                    SIGNAL(hasOneCard(quint32,int)),
                    this, SLOT(handleCard(quint32,int)));
        }
        //连接卡机信号槽
        disconnect(getDeviceFactory()->getCardMgr(), SIGNAL(buttonPush(int)),
                this, SLOT(handleButtonPush(int)));
        //连接车牌识别信号槽
        FormVpr *pVpr = NULL;
        getWidgetByType(emFormVpr, (QWidget**)&pVpr);
        disconnect(pVpr, SIGNAL(onGetVpr(int)), this, SLOT(getVpr(int)));
        //连接过车信号
        disconnect(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)),
                this, SLOT(getDIStatus(quint32,quint8)));
    }

    //打开pro卡读写器
    for(int i = DEV_CardReader3; i <= DEV_CardReader4; i++)
    {
        getDeviceFactory()->getCardReader((DEVNO)i)->StartCardDetection();
    }
}

void FormAutoVehInfo::clearData()
{
   ui->lbVehClass->clear();
   ui->lbVehPlate->clear();
   ui->lbVehType->clear();
}

