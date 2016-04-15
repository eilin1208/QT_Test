#include "dialogshowproinfo.h"
#include "ui_dialogshowproinfo.h"
#include "formmainwindow.h"
#include "devicefactory.h"
#include "lanectrl.h"
#include "laneinfo.h"
DialogShowProInfo::DialogShowProInfo(QWidget *parent, QLayout *layout):
    MtcOpWidget(parent, layout),
    ui(new Ui::DialogShowProInfo)
{
    ui->setupUi(this);
    m_proCardConsumeInfo.clear();
}

DialogShowProInfo::~DialogShowProInfo()
{
    //QDebug() << "release ....... " << objectName();

    delete ui;
}

void DialogShowProInfo::showProInfo()
{
    ExitTollItem* pToll = (ExitTollItem*)getLaneCtrl()->getTollItem();
    ui->lbEnVehType->setText(GetVehClassName((CVehClass)getLaneInfo()->getVehClass()));
    QString sColor = GetVehColorQString(getLaneInfo()->getVehPlateColor());
    ui->lbEnVehPlate->setText(sColor+getLaneInfo()->getVehPlate());
    ui->lbFee->setText(QString::number(pToll->getTollMoney().nCashMoney / 100) + tr("元"));
    ui->lbCardNum->setText(QString::number(pToll->getProCardBasicInfo().dwCardId));
    //TODO:这里要重新设计显示文字
    ui->lbCardType->setText(GetCardTypeName(pToll->getProCardBasicInfo().bType));
    ui->lbVehType->setText(GetVehClassName((CVehClass)pToll->getProCardBasicInfo().bVehClass));
    sColor = GetVehColorQString(pToll->getProCardBasicInfo().bVehPlateColor);
    ui->lbVehPlate->setText(sColor + GB2312toUtf8(pToll->getProCardBasicInfo().szVehPlate));
    ui->lbRest->setText(QString::number(pToll->getBalance() / 100.00, 'f', 2) + tr("元"));
    if(getLaneInfo()->isEntryLane())
    {
        ui->label_7->setText("");
        ui->lbFee->setText("");
    }
    else
    {
        ExitTollItem* pExit = (ExitTollItem*)pToll;
        ui->lbFee->setText(QString::number(pExit->getTollMoney().nCashMoney / 100) + tr("元"));
        ::showWarning(tr("请核对信息后按【确认】扣款"));
    }

}

void DialogShowProInfo::clearProInfo()
{
    ui->lbCardNum->clear();
    ui->lbCardType->clear();
    ui->lbEnVehPlate->clear();
    ui->lbEnVehType->clear();
    ui->lbFee->clear();
    ui->lbRest->clear();
    ui->lbVehPlate->clear();
    ui->lbVehType->clear();
}

void DialogShowProInfo::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyConfirm)
    {
        //        bool bRet = doPay();
        bool bRet = true;
        //只有出口才会走到支付流程
        if(!getLaneInfo()->isEntryLane())
        {
            bRet = doPay();
        }
        if(!bRet)
        {
            return;
        }
        else
        {
            //m_nResult = 1;
            setResult(1);
        }
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyEsc)
    {
        //m_nResult = 0;
        setResult(0);
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

bool DialogShowProInfo::doPay()
{
    QCardReader* reader = (QCardReader*)getDeviceFactory()->getCardReader(DEV_CardReader1);
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    bool bOpen = false;
    for(int i = 0 ; i < 3; i++)
    {
        bOpen = reader->OpenCard();
        if(bOpen)
        {
            break;
        }
        else
        {
            //TODO：睡5毫秒
        }
    }
    if(!bOpen)
    {
        ::showWarning(tr("请将云通卡（%1）放到读卡器上")
                      .arg(getLaneCtrl()->getTollItem()->getProCardBasicInfo().szCardNo),
                      false,
                      FormWarning::WARNING);
        return false;
    }
    TProCardBasicInfo basicInfo;
    quint32 dwBalance = 0;
    TCardTollInfo cardTollInfo;
    cardTollInfo.bVehPlateColor = 0xff;
    basicInfo.bVehPlateColor = 0xff;
    if(!reader->ReadProCard(basicInfo, dwBalance, cardTollInfo))
    {
        ::showWarning(tr("读卡失败！"));
        return false;
    }
    QString sCardId = QString::number(pExit->getProCardBasicInfo().dwCardId);
    if(reader->GetCardId() == sCardId)
    {
        ::showWarning(tr("请刷云通卡（%1）").arg(sCardId));
        return false;
    }
    quint32 chargeMoney = 0;
    quint32 actMoney = getLaneInfo()->getTollMoneyInfo()->nCashCardMoney +
            getLaneInfo()->getTollMoneyInfo()->nCashMoney;
    if(dwBalance < actMoney)
    {
        chargeMoney = (dwBalance / 500) * 500;
        getLaneInfo()->setSubPay(true);
    }
    else
    {
        chargeMoney = actMoney;
    }

    QDateTime writeProTime = QDateTime::currentDateTime();
    if(!reader->ProConsume(sCardId, chargeMoney, writeProTime, m_proCardConsumeInfo))
    {
        ::showWarning(tr("扣款失败！"), false, FormWarning::WARNING);
        getLaneInfo()->setSubPay(false);
        return false;
    }
    pExit->SetOccurTime(writeProTime);

    //支付方式
    quint8 cardType = pExit->getProCardBasicInfo().bType;
    quint8 userType = pExit->getProCardBasicInfo().bUserType;
    if(cardType == CARD_TYPE_STORE_CARD && userType == 0)
    {
        pExit->setPayType(PYT_StoreCard);
    }
    else if(cardType == CARD_TYPE_ALLFREE_CARD || cardType == CARD_TYPE_ROAD_BAOJIAO_STORE  ||
            cardType == CARD_TYPE_ALL_BAOJIAO_CARD || cardType == CARD_TYPE_ROAD_FREE_STORE ||
            cardType == CARD_TYPE_ROAD_FREE_TALLY || cardType == CARD_TYPE_ROAD_BAOJIAO_TALLY)
    {
        pExit->setPayType(PYT_MonthCard);
    }

    pExit->SetProCardConsumeInfo(m_proCardConsumeInfo);
    if(cardType == CARD_TYPE_STORE_CARD)
    {
        getDeviceFactory()->getFD()->ShowBalanceAfterDeduct(chargeMoney, m_proCardConsumeInfo.dwBalanceAfter);
    }
    else
    {
        getDeviceFactory()->getFD()->ShowBalanceAfterDeduct(chargeMoney, -1, true);
    }
    pExit->setBalance(m_proCardConsumeInfo.dwBalanceAfter);
    getLaneInfo()->getLastMoneyInfo()->nPaySn++;
    getLaneInfo()->getLastMoneyInfo()->nLastMoney = chargeMoney;
    getLaneInfo()->getLastMoneyInfo()->nLastCardMoney = 0;
    if(getLaneInfo()->getSubPay())
    {
        ::showWarning(tr("余额不足，请收取现金！"),false, FormWarning::WARNING);
    }
    LogMsg("lane", tr("扣款时间：%1").arg(m_proCardConsumeInfo.ConsumeTime.toString("yyyyMMddhhmmss")));
    LogMsg("lane", tr("扣款金额：%1，TAC(OUT): %2, 用户交易序号: %3")
           .arg(m_proCardConsumeInfo.dwMoney).arg(m_proCardConsumeInfo.dwTac)
           .arg(m_proCardConsumeInfo.wCardSeq));
    return true;
}

void DialogShowProInfo::showEvent(QShowEvent *event)
{
    showProInfo();
}

void DialogShowProInfo::hideEvent(QHideEvent *event)
{
    clearProInfo();
}

DialogShowProInfo* getShowProInfo(QWidget *widget, QLayout *layout)
{
    static DialogShowProInfo* p = NULL;
    if(p == NULL)
    {
        p = new DialogShowProInfo(widget, layout);
    }
    return p;
}
