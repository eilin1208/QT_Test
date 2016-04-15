#include "formentryreverse.h"
#include "ui_formentryreverse.h"
#include "formrecordtable.h"
FormEntryReverse::FormEntryReverse(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormEntryReverse)
{
    ui->setupUi(this);
}

FormEntryReverse::~FormEntryReverse()
{
    delete ui;
}

void FormEntryReverse::checkAndPromptCardNo()
{
    //qDebug()<<"checkAndPromptCardNo";
    bool bCanReverse = false;
    if(!getLaneCtrl()->GetAllowReverse(FCardType,m_FPhysicalCardID,FCardID))
    {
        getEntryReverse()->setMessage(tr("无法进行倒车处理"));
        bCanReverse = false;
    }
    else
    {
        //启动卡线程
        QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
        connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
                this, SLOT(cardDetected(quint32,int)));
        pReader->StartCardDetection() ;

        LogMsg("lane",tr("%1卡号：%2").arg(GetCardTypeName(FCardType)).arg(FCardID));
        bCanReverse = true;

    }
    //进行操作
    if(bCanReverse)
    {
        getEntryReverse()->setMessage(tr("请刷刚刚发出的%1").arg(GetCardTypeName(FCardType)));

    }
    else
    {
        ::showWarning(tr("请按【取消】键返回"),true);
    }

}


void FormEntryReverse::cardDetected(quint32 nCardId, int nCardType)
{
    //qDebug()<<"cardDetected";
    //关闭卡检测线程，防止重复读卡
    QCardReader *Reader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    Reader->StopCardDetection();
    TProCardBasicInfo ProCardBasicInfo;
    quint32 dwBalance = 0;
    TCardTollInfo CardTollInfo;
    TTollCardBasicInfo TollCardInfo;
    CardTollInfo.clear();
    if (CARD_TYPE_PHYSICAL(nCardType) == TYPE_PRO)
    {
        ProCardBasicInfo.clear();
        dwBalance = 0;
        if (! Reader->ReadProCard(ProCardBasicInfo, dwBalance, CardTollInfo))
        {
            getEntryReverse()->setMessage(tr("读卡失败\n请刷刚刚发出的%1").arg(GetCardTypeName(FCardType)));
            Reader->StartCardDetection();
            return;
        }
    }
    else
    {
        TollCardInfo.clear();
        if (!Reader->ReadTollCard(TollCardInfo, CardTollInfo))
        {
            getEntryReverse()->setMessage(tr("读卡失败\n请刷刚刚发出的%1").arg(GetCardTypeName(FCardType)));
            Reader->StartCardDetection();
            return;
        }
    }
    //需要qud掉前面的'0'才能进行比较
    // NOTE: 为防止倒车处理过程中，车辆离开等意外情况造成倒车无法处理，
    //在回写卡前重新进行判断
    if (! getLaneCtrl()->GetAllowReverse(FCardType,m_FPhysicalCardID,FCardID))
    {
        return;
    }
    nCardId = Reader->GetCardId().toUInt();
    LogMsg("lane",tr("本次卡号：%1，上次卡号：%2").arg(nCardId).arg(FCardID.toUInt()));
    if (nCardId != FCardID.toUInt())
    {
        getEntryReverse()->setMessage(tr("卡号不同\n请刷刚刚发出的%1").arg(GetCardTypeName(FCardType)));
        Reader->StartCardDetection();
        return;
    }
    if (!Reader->WriteBack(Reader->GetCardId()))
    {
        getEntryReverse()->setMessage(tr("回写%1失败，请重试！").arg(GetCardTypeName(FCardType)));
        Reader->StartCardDetection();
        return;
    }
    LogMsg("lane",tr("倒车处理，回写卡成功，卡号：%1").arg(nCardId));
    ::showWarning(tr("倒车成功,回写卡成功！"),true);
    //只有刷通行卡 倒车成功后，卡盒中卡数加1
    if (FCardType == CARD_TYPE_PASS_CARD)
    {
        getDataFileMgr()->AddOneCard(1);
    }
    //回写卡成功后，落杆
    getLaneCtrl()->DoReverse();
    setResult(1);
    StopReadCard();
    FormRecordTable* pRecord = NULL;
    getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    pRecord->ReverseSet();
}

void FormEntryReverse::StopReadCard()
{
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StopCardDetection();
    disconnect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
               this, SLOT(cardDetected(quint32,int)));
}

QString FormEntryReverse::getMessage()
{
    return ui->label_2->text();
}

void FormEntryReverse::setMessage(const QString &msg)
{
    ui->label_2->setText(msg);
}

void FormEntryReverse::showEvent(QShowEvent *event)
{
    checkAndPromptCardNo();
}

FormEntryReverse* getEntryReverse(QWidget *widget, QLayout *layout)
{
    static FormEntryReverse* p = NULL;
    if(p == NULL)
    {
        p = new FormEntryReverse(widget, layout);
    }
    return p;
}

void FormEntryReverse::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    //如果是domodalshow则需要处理enter按键和esc按键
    if(m_pEventLoop != 0)
    {
        if(mtcKeyEvent->func() == KeyConfirm)
        {
//            setResult(1);
        }
        else if(mtcKeyEvent->func() == KeyEsc)
        {
            setResult(-1);
        }
        mtcKeyEvent->setKeyType(KC_Func);
    }

}
