#include "shiftsum.h"
#include "PacketSendModule/sqlitedatasaver.h"
#include "datafilemgr.h"
#include "laneinfo.h"
ShiftSum::ShiftSum(QObject *parent) :
    QObject(parent)
{
    memset(&m_smallShiftSum,0,sizeof(m_smallShiftSum));
    memset(&m_bigShiftSum,0,sizeof(m_bigShiftSum));
    memset(&m_cardBoxInfo,0,sizeof(m_cardBoxInfo));
}

void ShiftSum::SetBigShiftEndTime(const QDateTime &endTime)
{
    DateTimeToChar14(endTime, m_bigShiftSum.EndTime);
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::StartNewBigShift(int nShiftNo, const QDate &dateTime, const QDateTime &startTime)
{
    memset(&m_bigShiftSum, 0, sizeof(m_bigShiftSum));
    QString tmpStr = tr("%1").arg(nShiftNo, 2, 10, QLatin1Char('0'));
    memcpy(m_bigShiftSum.ShiftID, tmpStr.toLocal8Bit().data(), 2);
    memcpy(m_bigShiftSum.Lchar, dateTime.toString("yyyyMMdd").toLocal8Bit().data(), 8);
    DateTimeToChar14(startTime, m_bigShiftSum.StartTime);
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::AddBar(bool bStatus)
{
    if(bStatus)
    {
        m_smallShiftSum.BarUpCnt++;
        m_bigShiftSum.BarUpCnt++;
    }
    else
    {
        m_smallShiftSum.BarDownCnt++;
        m_bigShiftSum.BarDownCnt++;
    }
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::SaveCardBox()
{
    memset(&m_cardBoxInfo, 0, sizeof(m_cardBoxInfo));
    IntToArChar(m_cardBoxInfo.CardBoxID, sizeof(m_cardBoxInfo.CardBoxID),
                getDataFileMgr()->getCardBoxInfo().CardBoxID);
    m_cardBoxInfo.CardCount = getDataFileMgr()->getCardBoxInfo().nCardCount;
}

void ShiftSum::AddLoop(int nLoop)
{
    if(nLoop == 1)
    {
        m_smallShiftSum.FrontLoopCnt++;
        m_bigShiftSum.FrontLoopCnt++;
    }
    else if(nLoop == 2)
    {
        m_smallShiftSum.BackLoopCnt++;
        m_bigShiftSum.BackLoopCnt++;
    }
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::AddMotor()
{
//    m_smallShiftSum.VCnt++;
    m_bigShiftSum.VCnt++;
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::AddSum(const TMsgShiftSum &single, TMsgShiftSum &sum)
{
    sum.DealCnt += single.DealCnt;
    sum.FrontLoopCnt += single.FrontLoopCnt;
    sum.BackLoopCnt += single.BackLoopCnt;
    sum.BarDownCnt += single.BarDownCnt;
    sum.BarUpCnt += single.BarUpCnt;
    memcpy(sum.InvEndID+2, single.InvEndID, sizeof(single.InvEndID)-2);
    sum.VCnt += single.VCnt;
    static int i= 1;
    //qDebug() <<"sum.VCnt="<< sum.VCnt<<"single.VCnt"<<single.VCnt<<"打印次数"<<i++;
    sum.CardCnt += single.CardCnt;
    sum.PapCnt += single.PapCnt;
    sum.DamCardCnt += single.DamCardCnt;
    sum.YTCardCnt += single.YTCardCnt;
    sum.OffCardCnt += single.OffCardCnt;
    sum.MonthCardCnt += single.MonthCardCnt;   
    sum.TollMoney += single.TollMoney;
    sum.CardCost += single.CardCost;
    sum.OverWeightMoney += single.OverWeightMoney;
    sum.FreeMoney += single.FreeMoney;
    sum.RebateMoney += single.RebateMoney;
    sum.CashMoney += single.CashMoney;
    sum.CashCardMoney += single.CashCardMoney;
    sum.StoreMoney += single.StoreMoney;
    sum.TallyMoney += single.TallyMoney;
    sum.OfficeMoney += single.OfficeMoney;
    sum.DiscountCarMoney += single.DiscountCarMoney;
    sum.UinonPayMoney += single.UinonPayMoney;
    sum.TollFreeMoney += single.TollFreeMoney;
    sum.TollCashMoney += single.TollCashMoney;
}

void ShiftSum::ClearBigShiftSum()
{
    memset(&m_bigShiftSum, 0, sizeof(m_bigShiftSum));
    getDataSaver()->RemoveBigShiftData();
}

void ShiftSum::StartNewSmallShift(const TMsgToSend &msg)
{
    memset(&m_smallShiftSum, 0, sizeof(m_smallShiftSum));
    QString tmpStr = tr("%1").arg(getLaneInfo()->getShiftInfo().wShift, 2, 10, QLatin1Char('0'));
    memcpy(m_smallShiftSum.ShiftID, tmpStr.toLocal8Bit().data(), 2);
    memcpy(m_smallShiftSum.Lchar, getLaneInfo()->getShiftInfo().iWorkDate.toString("yyyyMMdd").toLocal8Bit().data(), 8);
    memcpy(m_smallShiftSum.StartTime, msg.LaneInfo.LoginTime, sizeof(m_smallShiftSum.StartTime));
    m_smallShiftSum.StartWasteSN = getLaneInfo()->getLogWaste();
//    IntToArChar(m_smallShiftSum.InvBeginID, sizeof(m_smallShiftSum.InvBeginID),
//                getDataFileMgr()->GetInvInfo().InvBeginNo);
    //liujian
    memcpy(m_smallShiftSum.InvBeginID+2,QString::number(getDataFileMgr()->GetInvInfo().InvBeginNo).toLocal8Bit().data(),sizeof(m_smallShiftSum.InvBeginID)-2);
}

void ShiftSum::ClearSmallShiftSum()
{
    memset(&m_smallShiftSum, 0, sizeof(m_smallShiftSum));
    memset(&m_cardBoxInfo, 0, sizeof(m_cardBoxInfo));
    getDataSaver()->RemoveSynData();
}

bool ShiftSum::LoadFromFile()
{
    int nShiftSumLen = 0;
    bool nRet1 = false;
    bool nRet2 = false;
    memset(&m_bigShiftSum, 0, sizeof(m_bigShiftSum));
    nShiftSumLen = sizeof(m_bigShiftSum);
    nRet1 = getDataSaver()->GetBigShiftData(&m_bigShiftSum, nShiftSumLen);
    memset(&m_smallShiftSum, 0, sizeof(m_smallShiftSum));
    nRet2 = getDataSaver()->GetSynData(&m_smallShiftSum, nShiftSumLen);
    return nRet1 && nRet2;
}

void ShiftSum::Sum(const TMsgToSend &msg)
{
    int nICCnt = 0;
    TMsgShiftSum singleShiftSum;
    memset(&singleShiftSum, 0, sizeof(singleShiftSum));
    singleShiftSum.DealCnt += 1;
    singleShiftSum.VCnt += msg.VehInfo.VCnt;
    if((msg.BasicInfo.msgHead.cMsgID[1] == '1' && msg.BasicInfo.msgHead.cMsgType[1] == '5') ||
            (msg.BasicInfo.msgHead.cMsgID[1] == '2' && msg.BasicInfo.msgHead.cMsgType[1] == '5'))
    {
        nICCnt = -1;
    }
    else
    {
        nICCnt = 1;
    }

    switch(msg.ICCInfo.CardType)
    {
    //qDebug()<<"singleShiftSum.CardCnt= "<<singleShiftSum.CardCnt;
    case 0: singleShiftSum.CardCnt += nICCnt; break;
    case 9: singleShiftSum.PapCnt += nICCnt; break;
    case 7: singleShiftSum.MonthCardCnt += nICCnt; break;
    case 3: singleShiftSum.OffCardCnt += nICCnt; break;
    case 22:
    case 23:
    case 28:
    case 29:
        singleShiftSum.YTCardCnt += nICCnt;
        break;
    default:
        break;
    }
    if(msg.DealInfo.SpInfo[DS_SEBADCARD] == '1')
    {
        singleShiftSum.DamCardCnt += nICCnt;
    }


    singleShiftSum.TollMoney += msg.TollInfo.TollMoney;
    singleShiftSum.CardCost += msg.TollInfo.CardCost;
    singleShiftSum.CashCardMoney += msg.TollInfo.CashCardMoney;
    singleShiftSum.OverWeightMoney += msg.TollInfo.OverWeightMoney;
    singleShiftSum.FreeMoney += msg.TollInfo.FreeMoney;
    singleShiftSum.RebateMoney += msg.TollInfo.RebateMoney;
    singleShiftSum.CashMoney += msg.TollInfo.CashMoney;
    if(msg.TollInfo.PayType == 0)
    {
        singleShiftSum.TollCashMoney += msg.TollInfo.LastMoney;
    }
    else if(msg.TollInfo.PayType == 1)
    {
        switch(msg.ICCInfo.CardType)
        {
        case CT_POStoreCard:
        case CT_StoreCard:
            singleShiftSum.StoreMoney += msg.TollInfo.LastMoney;
            break;
        case CT_TallyCard:
        case CT_POTallyCard:
            singleShiftSum.TallyMoney += msg.TollInfo.LastMoney;
            break;
        case CT_ServantCard:
            singleShiftSum.OfficeMoney += msg.TollInfo.CashMoney;
            break;
        case CT_BJCard:
            singleShiftSum.DiscountCarMoney += msg.TollInfo.CashMoney;
            break;
        default:
            break;
        }
    }

    //liujian
    memcpy(singleShiftSum.InvEndID,QString::number(getDataFileMgr()->GetInvInfo().PreInvBeginNo).toLocal8Bit().data(),sizeof(singleShiftSum.InvEndID));
    SaveCardBox();
    AddSum(singleShiftSum, m_smallShiftSum);
    AddSum(singleShiftSum, m_bigShiftSum);
    //更新工班
    getDataSaver()->SaveBigShiftData(&m_bigShiftSum, sizeof(m_bigShiftSum));
}

void ShiftSum::GetBigShiftInfo(TMsgShiftSum &AShiftSum)
{
    AShiftSum = m_bigShiftSum;
}

bool ShiftSum::IsBigShiftSame(const int nShiftNo, const QDate &workDate)
{
    int nBigShiftNo = QString::fromLocal8Bit(m_bigShiftSum.ShiftID,2).toInt();
    QDate tmpDate = QDate::fromString(QString::fromLocal8Bit(m_bigShiftSum.Lchar,8), "yyyyMMdd");
    return (nShiftNo == nBigShiftNo) && (workDate == tmpDate);
}

bool ShiftSum::LessThan(const int nShiftNo, const QDate &workDate)
{
    int nBigShiftNo = QString::fromLocal8Bit(m_bigShiftSum.ShiftID,2).toInt();
    QDate tmpDate = QDate::fromString(QString::fromLocal8Bit(m_bigShiftSum.Lchar,8), "yyyyMMdd");
    if(tmpDate < workDate)
    {
        return true;
    }
    if(tmpDate > workDate)
    {
        return false;
    }
    return nBigShiftNo < nShiftNo;
}

TMsgShiftSum& ShiftSum::GetBigShiftSum()
{
    return m_bigShiftSum;
}

int ShiftSum::GetNormalVCnt()
{
    //qDebug()<<"m_smallShiftSum.VCnt"<<m_smallShiftSum.VCnt;
    return m_smallShiftSum.VCnt;
}

int ShiftSum::GetTotalMoney()
{
    return m_smallShiftSum.CashMoney + m_smallShiftSum.CashCardMoney;
}

void ShiftSum::GetShiftCardBoxInfo(TMsgCardBoxInfo &cardBoxInfo)
{
    memcpy(cardBoxInfo.CardBoxID, m_cardBoxInfo.CardBoxID, sizeof(cardBoxInfo.CardBoxID));
    cardBoxInfo.CardCount = m_cardBoxInfo.CardCount;
}

void ShiftSum::GetSmallShiftInfo(TMsgShiftSum &shiftSum)
{
    shiftSum = m_smallShiftSum;
}

bool ShiftSum::HaveSmallShiftSumInfo()
{
    QString tmpId = QString::fromLocal8Bit(m_smallShiftSum.ShiftID, 2);
    QDate tmpDate = QDate::fromString(QString::fromLocal8Bit(m_smallShiftSum.Lchar, 8), "yyyyMMdd");
    return (tmpId.toInt() != 0) && (tmpDate.isValid());
}

TMsgShiftSum ShiftSum::GetSmallShiftSum()
{
    return m_smallShiftSum;
}

int ShiftSum::GetTotalCardVCNt()
{
    return m_smallShiftSum.CardCnt;
}

bool ShiftSum::HaveBigShiftSumInfo()
{
    QString tmpId = QString::fromLocal8Bit(m_bigShiftSum.ShiftID, 2);
    QDate tmpDate = QDate::fromString(QString::fromLocal8Bit(m_bigShiftSum.Lchar, 8), "yyyyMMdd");
    return (tmpId.toInt() != 0) && (tmpDate.isValid());
}

void ShiftSum::SetSmallShiftEndTime(const char *sEndTime)
{
    memcpy(m_smallShiftSum.EndTime, sEndTime, sizeof(m_smallShiftSum.EndTime));
}
ShiftSum* getShiftSum()
{
    static ShiftSum shift;
    return &shift;
}


