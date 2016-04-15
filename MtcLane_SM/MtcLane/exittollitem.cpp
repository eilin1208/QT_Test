#include "exittollitem.h"
#include "CommonModule/laneutils_new.h"
#include "ParamModule/qbaseparamfile.h"
#include "ParamModule/qparammgr.h"
#include "PacketSendModule/msgsendfunc.h"
#include "CommonModule/MsgDefine.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "shiftsum.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "CommonModule/vehplatefunc.h"
#include "devicefactory.h"
#include  "lanectrl.h"
#include "FareCalc/vehtollinfoclass.h"
#include "CommonModule/LaneType.h"
#include "laneinfo.h"
#include "WtArm/wtdatamgrunit.h"
ExitTollItem::ExitTollItem(QObject *parent) :
    TollItem(parent)
{

    //变量初始化liujian
    ClearSavedInfo();
}
ExitTollItem::~ExitTollItem()
{
}
void ExitTollItem::ClearVehEnInfo()
{
    m_vehEntryInfo.Clear();
    m_vehEntryInfo.bEnVLPC = -1;
    ClearMoneyInfo();
}

void ExitTollItem::ClearAxisInfo()
{
    m_pVehAxisInfo = NULL;
}

void ExitTollItem::ClearFlagStationInfo()
{
    FFlagStationInfo.clear();
}

void ExitTollItem::ClearInvPrintInfo()
{
    m_InvPrintInfo.clear();
}

void ExitTollItem::SetMoneyByQryResultFare()
{
    TCalculateMoneyInfo calMoneyInfo;
    //实际里程
    memset(&calMoneyInfo,0,sizeof(calMoneyInfo));
    m_nRealDistance = m_FareQryResult.m_dwRealMeters;
    //    //计费里程
    m_nTollDistance = m_FareQryResult.m_dwTollMeters;
    if(m_iNoCard > 0 || m_BadCardType == BadCard_Factitious)
    {
        calMoneyInfo.nCardCost = m_nCardCost;
    }
    calMoneyInfo.nOverWeightMoney = m_FareQryResult.m_dwMoneyOverweightDue;
    //免收通行费
    calMoneyInfo.nFreeMoney = m_FareQryResult.m_dwDiscountedFree;
    //折扣优惠金额
    calMoneyInfo.nRebateMoney = m_FareQryResult.m_dwMoneyDue - m_FareQryResult.m_dwDiscountedDue;
    calMoneyInfo.nTollMoney = (int)(m_FareQryResult.m_dwMoneyDue + m_FareQryResult.m_dwDiscountedFree) + calMoneyInfo.nRebateMoney;
    //应收通行费，折扣后
    calMoneyInfo.nDisCountTollMoney = m_FareQryResult.m_dwMoneyDue;
    if(IsTruck(VehClass2Byte(m_VehInfo.VehClass)))
    {
        calMoneyInfo.nTruckBasicMoney = calMoneyInfo.nTollMoney - calMoneyInfo.nOverWeightMoney;
    }
    SetCalculateMoneyInfo(calMoneyInfo);
    //应收通行费
    m_TollMoneyInfo.nCashMoney = m_FareQryResult.m_dwDiscountedDue;
    m_TollMoneyInfo.nCashCardMoney = m_CalculateMoneyInfo.nCardCost;
    m_TollMoneyInfo.nFreeMoney = m_FareQryResult.m_dwDiscountedFree;
    //拆分信息
    m_SplitInfo.wSplitNum = m_FareQryResult.GetSplitInfo(m_SplitInfo.SplitInfo,sizeof(m_SplitInfo.SplitInfo));
    m_SplitInfo.wCellNum = m_FareQryResult.GetTollCellInfo(m_SplitInfo.CellInfo,sizeof(m_SplitInfo.CellInfo));
    m_SplitInfo.wStationNum = m_FareQryResult.GetStationInfo(m_SplitInfo.PathInfo,sizeof(m_SplitInfo.PathInfo));
    m_FreeArea.bFreeAreaCnt = m_FareQryResult.GetFreeInfo(m_FreeArea.FreeArea,300);
    if(m_FreeArea.bFreeAreaCnt > 0)
    {
        m_FreeArea.bFreeType = frNeighborStaPlus;
    }

}

void ExitTollItem::RegetSplitInfo()
{
    m_SplitInfo.wSplitNum = m_FareQryResult.GetSplitInfo(m_SplitInfo.SplitInfo,sizeof(m_SplitInfo.SplitInfo));
    m_SplitInfo.wCellNum  = m_FareQryResult.GetTollCellInfo(m_SplitInfo.CellInfo,sizeof(m_SplitInfo.CellInfo));
    m_SplitInfo.wStationNum = m_FareQryResult.GetStationInfo(m_SplitInfo.PathInfo,sizeof(m_SplitInfo.PathInfo));
}

//发送通行费信息报文
void ExitTollItem::SendMoneyInfo(const TTollMoneyInfo& TollMoney,const TCalculateMoneyInfo& CalculateMoney)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 26;
    MsgType = 1;
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime, msg,true);
    msg.TollInfo.PayType = 0;
    msg.TollInfo.TollMoney = CalculateMoney.nTollMoney;
    msg.TollInfo.CardCost = CalculateMoney.nCardCost;
    msg.TollInfo.OverWeightMoney = CalculateMoney.nOverWeightMoney;
    msg.TollInfo.FreeMoney = CalculateMoney.nFreeMoney;
    msg.TollInfo.RebateMoney = CalculateMoney.nRebateMoney;
    msg.TollInfo.CashMoney =  TollMoney.nCashMoney;
    msg.TollInfo.CashCardMoney = TollMoney.nCashCardMoney;
    FillVehEnInfo(msg);
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendMoneyInfo失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendMoneyInfo完成"));
}

void ExitTollItem::FillPayCardInfo(TMsgToSend &msg)
{
    TollItem::FillPayCardInfo(msg);
    char tmpPayCardID[16];
    if(m_bPayType == PYT_Cash)
    {
        msg.PayCardInfo.PayCardBalBefore = 0;
        msg.PayCardInfo.PayCardBalAfter = 0;
        return;
    }
    if(!m_bUseMonthCard)
    {
        msg.PayCardInfo.PayCardTranSN = m_proCardConsumeInfo.wCardSeq;
        msg.PayCardInfo.PSAMTradeSN = m_proCardConsumeInfo.dwTermSeq;
        msg.PayCardInfo.TAC = m_proCardConsumeInfo.dwTac;
        Raw2HexStr(msg.PayCardInfo.PSamTerm,m_proCardConsumeInfo.psamTermNo,6);
        msg.PayCardInfo.PayCardBalBefore = m_proCardConsumeInfo.dwBalanceBefore/100.00;
        msg.PayCardInfo.PayCardBalAfter = m_proCardConsumeInfo.dwBalanceAfter/100.00;
        msg.PayCardInfo.TransType = m_proCardConsumeInfo.bTransType;
        msg.PayCardInfo.KeyVer = m_proCardConsumeInfo.bKeyVer;
    }
    else if(m_bUseMonthCard)
    {
        msg.PayCardInfo.PayCardType = CT_BJCard;
        IntToArChar(msg.PayCardInfo.PayCardNet,4,m_MonthCardInfo.M1BasicInfo.wNetworkId);
        IntToArChar(tmpPayCardID,9,m_MonthCardInfo.M1BasicInfo.dwCardNum);
        memcpy(msg.PayCardInfo.PayCardID,msg.PayCardInfo.PayCardNet,4);
        memcpy(&msg.PayCardInfo.PayCardID[4],tmpPayCardID,9);
        msg.PayCardInfo.PayCardSN = m_MonthCardInfo.M1BasicInfo.dwCardId;
        msg.PayCardInfo.PayCardTranSN = 0;
        memcpy(msg.PayCardInfo.PayCardVLP,m_MonthCardInfo.BaoJiaoInfo.szVehPlate,sizeof(msg.PayCardInfo.PayCardVLP));
        msg.PayCardInfo.PayCardVLPC = m_MonthCardInfo.BaoJiaoInfo.bVehPlateColor;
        msg.PayCardInfo.PayCardVehClass = m_MonthCardInfo.BaoJiaoInfo.bVehClass;
        msg.PayCardInfo.PSAMTradeSN = 0;
        msg.PayCardInfo.TAC = 0;
        msg.PayCardInfo.PayCardBalBefore = 0;
        msg.PayCardInfo.PayCardBalAfter = 0;
        msg.PayCardInfo.TransType = 0;
        msg.PayCardInfo.KeyVer = 0xFF;
    }
}

void ExitTollItem::FillDealInfo(TMsgToSend &msg)
{
    TollItem::FillDealInfo(msg);
    char* tmpStr;
    if(Entry_ByCard == m_vehEntryInfo.nEntryType || Entry_ByQry == m_vehEntryInfo.nEntryType)
    {
        tmpStr = new char[sizeof(m_vehEntryInfo.szEnVLP)];
        //tmpStr = m_vehEntryInfo.szEnVLP;
        memcpy(tmpStr,m_vehEntryInfo.szEnVLP,sizeof(m_vehEntryInfo.szEnVLP));
        if(strlen(tmpStr) > 0)
        {
            if(!CompareVehPlate(m_VehInfo.szVehPlate,tmpStr) || (m_VehInfo.nVehPlateColor != -1&&m_vehEntryInfo.bEnVLPC != -1&&m_VehInfo.nVehPlateColor!=m_vehEntryInfo.bEnVLPC))
            {
                msg.DealInfo.SpInfo[DS_TSPLATEERR] = '1';
            }
        }
        //modify by xiangkai
        delete [] tmpStr;
        if(IsCar(m_vehEntryInfo.bEnVC && IsCar(VehClass2Byte(m_VehInfo.VehClass))))
        {
            if(m_vehEntryInfo.bEnVC>VehClass2Byte(m_VehInfo.VehClass))
            {
                msg.DealInfo.SpInfo[DS_TSBDB2S] = '1';
            }
            if(m_vehEntryInfo.bEnVC<VehClass2Byte(m_VehInfo.VehClass))
            {
                msg.DealInfo.SpInfo[DS_TSBDS2B] = '1';
            }
        }
        if(IsTruck(m_vehEntryInfo.bEnVC) && IsTruck(VehClass2Byte(m_VehInfo.VehClass)) && (!getLaneInfo()->getUseWeight()))
        {
            msg.DealInfo.SpInfo[DS_TOLLBYVC] = '1';
            if(m_vehEntryInfo.bEnVC>VehClass2Byte(m_VehInfo.VehClass))
            {
                msg.DealInfo.SpInfo[DS_TSBDB2S] = '1';
            }
            if(m_vehEntryInfo.bEnVC<VehClass2Byte(m_VehInfo.VehClass))
            {
                msg.DealInfo.SpInfo[DS_TSBDS2B] = '1';
            }
        }
        if(IsCar(m_vehEntryInfo.bEnVC) && IsTruck(VehClass2Byte(m_VehInfo.VehClass)))
        {
            msg.DealInfo.SpInfo[DS_TSVTK2H] = '1';
        }
        if(IsTruck(m_vehEntryInfo.bEnVC && IsCar(VehClass2Byte(m_VehInfo.VehClass))))
        {
            msg.DealInfo.SpInfo[DS_TSVTH2K] = '1';
        }
    }
    //U型免费
    if(m_bIsUXing)
    {
        msg.DealInfo.SpInfo[DS_UXPAY] = '1';
        msg.DealInfo.SpInfo[DS_UXFREE] = '0';
        if(m_bFreeUFee)
        {
            msg.DealInfo.SpInfo[DS_UXPAY] = '0';
            msg.DealInfo.SpInfo[DS_UXFREE] = '1';
        }
    }
    //车类不符收改免
    if(m_vehEntryInfo.bEnVT <=1 && IsFullFreeVehTollMode(m_VehInfo.VehType))
    {
        msg.DealInfo.SpInfo[DS_TSVMP2F] = '1';
        msg.DealInfo.SpInfo[DS_TSVMF2P] = '0';
    }
    //车类不符免该收
    if(VehType2Byte(m_VehInfo.VehType) == 1 && IsFullFreeVehTollMode(Byte2VehType(m_vehEntryInfo.bEnVT)))
    {
        msg.DealInfo.SpInfo[DS_TSVMP2F] = '0';
        msg.DealInfo.SpInfo[DS_TSVMF2P] = '1';
    }
    //非正常超时
    if(m_bOverTime)
    {
        msg.DealInfo.SpInfo[DS_SEOVTIME] = '1';
    }
    //出口坏卡
    if(Entry_ByCard != m_vehEntryInfo.nEntryType && Entry_None != m_vehEntryInfo.nEntryType)
    {
        if(FPhysicalCardType == TYPE_M1_S50_MAD || FPhysicalCardType == TYPE_PRO)
        {
            msg.DealInfo.SpInfo[DS_SEBADCARD] = '1';
        }
    }
    //出口无卡
    if(m_iNoCard > 0)
    {
        msg.DealInfo.SpInfo[DS_NoCard] = '1';
    }
    //换卡车
    if(m_bChangedCard)
    {
        msg.DealInfo.SpInfo[DS_SECCARD] = '1';
    }
    //拖车
    if(getLaneCtrl()->IsTract())
    {
        msg.DealInfo.SpInfo[DS_Tract] = '1';
    }
}

void ExitTollItem::FillVehEnInfo(TMsgToSend &msg)
{
    //char* tmpStr;
    QOrgCodeParamFile* OrgTable = NULL;
    SOrgCodeValue OrgCodeInfo;
    OrgTable = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);

    memset(&msg.VehEnInfo,0,sizeof(msg.VehEnInfo));
    msg.VehEnInfo.EnVLPC = -1;
    OrgCodeInfo.Clear();
    OrgTable->queryOrgCodeValue(OrgCodeInfo,m_vehEntryInfo.dwEnStationID);
    memcpy(msg.VehEnInfo.EnProvID,OrgCodeInfo.sBL_ProvID.toLocal8Bit().data(),sizeof(msg.VehEnInfo.EnProvID));
    memcpy(msg.VehEnInfo.EnNetWorkID,OrgCodeInfo.sBL_NetID.toLocal8Bit().data(),sizeof(msg.VehEnInfo.EnNetWorkID));
    memcpy(msg.VehEnInfo.EnRoadID,OrgCodeInfo.sBL_Road.toLocal8Bit().data(),sizeof(msg.VehEnInfo.EnRoadID));
    memcpy(msg.VehEnInfo.EnSubCenter,OrgCodeInfo.sBL_SubCenter.toLocal8Bit().data(),sizeof(msg.VehEnInfo.EnSubCenter));
    sprintf(msg.VehEnInfo.QueryEnStation,"%06d",m_vehEntryInfo.dwQryStationID);
    sprintf(msg.VehEnInfo.EnStation,"%06d",m_vehEntryInfo.dwEnStationID);
    sprintf(msg.VehEnInfo.EnLane,"%03d",m_vehEntryInfo.nLaneID);
    DateTimeToChar14(m_vehEntryInfo.EnTime,msg.VehEnInfo.EnTime);
    msg.VehEnInfo.EnWasteSN = m_vehEntryInfo.bEnWasteSN;
    msg.VehEnInfo.EnLaneType = m_vehEntryInfo.bEnLaneType;
    IntToArChar(msg.VehEnInfo.EnOper,sizeof(msg.VehEnInfo.EnOper),m_vehEntryInfo.dwEnOper);
    msg.VehEnInfo.EnVC = m_vehEntryInfo.bEnVC;
    msg.VehEnInfo.EnVT = m_vehEntryInfo.bEnVT;
    msg.VehEnInfo.EnVLPC = m_vehEntryInfo.bEnVLPC;
    memcpy(msg.VehEnInfo.EnVLP,m_vehEntryInfo.szEnVLP,sizeof(msg.VehEnInfo.EnVLP));
    memcpy(msg.VehEnInfo.EnPSAMTerm,m_vehEntryInfo.szEnPSamTerm,sizeof(msg.VehEnInfo.EnPSAMTerm));
}

void ExitTollItem::FillWeightInfo(TMsgToSend &msg)
{
    TollItem::FillWeightInfo(msg);
    getWtDataMgr()->GetFirstVehAxisInfo(&m_pVehAxisInfo);
    if(m_pVehAxisInfo != NULL)
    {
        msg.WeightInfo.AxleNum = m_pVehAxisInfo->GetAxisCount();
        msg.WeightInfo.TotalWeight = m_pVehAxisInfo->GetWeight();
        msg.WeightInfo.LimitWeight = m_pVehAxisInfo->GetLimitWeight();
        msg.WeightInfo.OverWerghtRate = m_pVehAxisInfo->GetOverloadRate();
        memcpy(msg.WeightInfo.PreAxisInfo,m_pVehAxisInfo->getFPreAxisInfo().toLocal8Bit().data(),
               qMin(sizeof(msg.WeightInfo.PreAxisInfo),(quint32)m_pVehAxisInfo->getFPreAxisInfo().count()));
        //m_VehAxisInfo.GetRawAxisInfoStr(msg.WeightInfo.PreAxisInfo, sizeof(msg.WeightInfo.PreAxisInfo));
        msg.WeightInfo.PreAxleNum = m_pVehAxisInfo->getFPreAxisCount();
        msg.WeightInfo.PreTotalWetght = m_pVehAxisInfo->getFPreWeight();
        msg.WeightInfo.PreOverWeightRate = m_pVehAxisInfo->getFPreOverloadRate();
    }
}

void ExitTollItem::FillTollBaseInfo(TMsgToSend &msg)
{
    QBaseParamFile* pParamFile = NULL;
    pParamFile = getParamMgr()->GetParamFile(cfAllRate);
    msg.TollBaseInfo.OverTime = GetOverTime();
    msg.TollBaseInfo.TollDistance = m_FareQryResult.m_dwTollMeters;
    //    //实际里程（米）
    msg.TollBaseInfo.RealDistance = m_FareQryResult.m_dwRealMeters;
    msg.TollBaseInfo.FlagStationCnt = FFlagStationInfo.bFlagStationCnt;
    memcpy(msg.TollBaseInfo.FlagStationInfo,FFlagStationInfo.FlagStations,sizeof(msg.TollBaseInfo.FlagStationInfo));
    msg.TollBaseInfo.TollFlag = m_bTollType;
    SParamFileHead head;
    pParamFile->GetCfgFileHead(head,false);
    DateTimeToChar14(head.lVersion,msg.TollBaseInfo.FalcVer);
    msg.TollBaseInfo.FreeType = m_FreeArea.bFreeType;
    msg.TollBaseInfo.FreeAreaCnt = m_FreeArea.bFreeAreaCnt;
    memcpy(msg.TollBaseInfo.FreeArea,m_FreeArea.FreeArea,sizeof(msg.TollBaseInfo.FreeArea));
    msg.VehInfo.VSpeed = getMsgSendFun()->CalcVCSpeed(m_FareQryResult.m_dwRealMeters,m_FareQryCondition.m_nDriveTimeInSeconds);
}

void ExitTollItem::FillTollInfo(TMsgToSend &msg)
{
    switch(m_bPayType)
    {
    case PYT_Unknow:
        msg.TollInfo.PayType = 0xFF;
        break;
    case PYT_Cash:
        msg.TollInfo.PayType = 0;
        break;
    case PYT_UniPayCard:
        msg.TollInfo.PayType = 0;
        break;
    case PYT_OntCartoon:
        msg.TollInfo.PayType = 0;
        break;
    case PYT_StoreCard:
        msg.TollInfo.PayType = 1;
        break;
    case PYT_TallyCard:
        msg.TollInfo.PayType = 1;
        break;
    case PYT_MonthCard:
        msg.TollInfo.PayType = 1;
        break;
    case PYT_ServantCard:
        msg.TollInfo.PayType = 1;
        break;

    case PYT_ProvOutStore:
        msg.TollInfo.PayType = 1;
        break;
    case PYT_ProvOutTally:
        msg.TollInfo.PayType = 1;
        break;
    case PYT_Free:
        msg.TollInfo.PayType = 99;
        break;
    default:
        break;
    }
    msg.TollInfo.TruckBasicMoney = m_CalculateMoneyInfo.nTruckBasicMoney;
    msg.TollInfo.TollMoney = m_CalculateMoneyInfo.nTollMoney;
    msg.TollInfo.CardCost = m_CalculateMoneyInfo.nCardCost;
    msg.TollInfo.OverWeightMoney = m_CalculateMoneyInfo.nOverWeightMoney;
    if(m_bFreeUFee)
    {
        msg.TollInfo.FreeMoney = msg.TollInfo.TollMoney;
    }
    else
    {
        msg.TollInfo.FreeMoney = m_CalculateMoneyInfo.nFreeMoney;
    }
    msg.TollInfo.RebateMoney = m_CalculateMoneyInfo.nRebateMoney;
    msg.TollInfo.CashMoney = m_TollMoneyInfo.nCashMoney;
    msg.TollInfo.CashCardMoney = m_TollMoneyInfo.nCashCardMoney;
    msg.TollInfo.PaySN = m_LastMoneyInfo.nPaySn;
    if(msg.TollInfo.PaySN < 1)
    {
        msg.TollInfo.PaySN = 1;
    }
    msg.TollInfo.LastMoney = m_LastMoneyInfo.nLastMoney;
    msg.TollInfo.LastCardMoney = m_LastMoneyInfo.nLastCardMoney;
}

void ExitTollItem::FillSplitInfo(TMsgToSend &msg)
{
    msg.SplitInfo.SpliteNum = m_SplitInfo.wSplitNum;
    memcpy(msg.SplitInfo.SplitInfo,m_SplitInfo.SplitInfo,sizeof(msg.SplitInfo));
    msg.SplitInfo.CellNum = m_SplitInfo.wCellNum;
    memcpy(msg.SplitInfo.CellInfo,m_SplitInfo.CellInfo,sizeof(msg.SplitInfo.CellInfo));
    msg.SplitInfo.StationNum = m_SplitInfo.wStationNum;
    memcpy(msg.SplitInfo.PathInfo,m_SplitInfo.PathInfo,sizeof(msg.SplitInfo.PathInfo));
}

void ExitTollItem::FillInvModefyInfo(TMsgToSend &msg)
{
    quint64 tmpint;
    //票据修改前起号
    tmpint = getDataFileMgr()->GetInvInfo().PreInvBeginNo;
    QString tmpStr = QString::number(tmpint);
    memcpy(msg.InvModefyInfo.PreInvNo,tmpStr.toLocal8Bit().data(),sizeof(msg.InvModefyInfo.PreInvNo));
    //票据修改前张数
    //msg.InvModefyInfo.PreInvCnt = getDataFileMgr()->GetInvInfo().PreInvCount;
    msg.InvModefyInfo.PreInvCnt = 0;
    //票据修改后起号
    tmpint = getDataFileMgr()->GetInvInfo()
            .InvBeginNo;
    tmpStr = QString::number(tmpint);
    memcpy(msg.InvModefyInfo.AftInvNo,tmpStr.toLocal8Bit().data(),qMin(sizeof(tmpStr.toLocal8Bit().data()),sizeof(msg.InvModefyInfo.AftInvNo)));
    //票据修改后张数
    //msg.InvModefyInfo.AftInvCnt = getDataFileMgr()->GetInvInfo().InvCount;
    msg.InvModefyInfo.AftInvCnt = 0;
}

void ExitTollItem::FillInvPrintInfo(TMsgToSend &msg)
{
    quint64 tmpint = 0;
    tmpint = m_InvPrintInfo.InvNum;
    QString tmpStr = QString::number(tmpint);
    memset(msg.InvPrintInfo.Inv,0,sizeof(msg.InvPrintInfo.Inv));
    memcpy(msg.InvPrintInfo.Inv,tmpStr.toLocal8Bit().data(),qMin(strlen(tmpStr.toLocal8Bit().data()),sizeof(msg.InvPrintInfo.Inv)));
    msg.InvPrintInfo.InvCnt = m_InvPrintInfo.InvCnt;
}

void ExitTollItem::FillSingleMsg(int nPos, TMsgToSend &Msg, bool bPeekSN)
{
    TollItem::FillSingleMsg(nPos, Msg, bPeekSN);
    switch(nPos)
    {
    case 10:
        FillVehEnInfo(Msg);
        break;
    case 11:
        FillWeightInfo(Msg);
        break;
    case 12:
        FillTollBaseInfo(Msg);
        break;
    case 13:
        FillTollInfo(Msg);
        break;
    case 14:
        FillSplitInfo(Msg);
        break;
    case 15:
        FillInvModefyInfo(Msg);
        break;
    case 16:
        FillInvPrintInfo(Msg);
        break;
    default:
        break;
    }
}

void ExitTollItem::FillYwWaste(QDateTime lpTime)
{
    int msgID,msgType;
    msgID = 2;
    msgType = LST_TYPE_NORMAL;
    ClearSendMsg(FMsgToSend);
    FillMsgToSend(msgID,msgType,FMsgToSend);
    FMsgToSend.VehInfo.VCnt = 1;
    FillHedgeMsg(FMsgToSend);
}

void ExitTollItem::SetVehEntryInfo(const CVehEntryInfo& AVehEntryInfo)
{
    QDateTime curTime;
    quint32 dwEnTime,dwExitTime;
    QOrgCodeParamFile* OrgTable = NULL;
    SOrgCodeValue OrgCodeInfo;
    OrgTable = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    OrgTable->queryOrgCodeValue(OrgCodeInfo,AVehEntryInfo.dwEnStationID,ORG_TYPE_STATION);
    m_vehEntryInfo.Clear();
    m_vehEntryInfo.bEnVLPC = -1;
    m_vehEntryInfo.nEntryType = AVehEntryInfo.nEntryType;
    m_vehEntryInfo.dwQryStationID = AVehEntryInfo.dwQryStationID;
    m_vehEntryInfo.dwEnStationID = AVehEntryInfo.dwEnStationID;
    memcpy(m_vehEntryInfo.szEnStationName,OrgCodeInfo.sOrgSName.toLocal8Bit().data(),sizeof(m_vehEntryInfo.szEnStationName));
    m_vehEntryInfo.nLaneID = AVehEntryInfo.nLaneID;
    m_vehEntryInfo.EnTime = AVehEntryInfo.EnTime;
    m_vehEntryInfo.bEnWasteSN = AVehEntryInfo.bEnWasteSN;
    m_vehEntryInfo.bEnLaneType = AVehEntryInfo.bEnLaneType;
    m_vehEntryInfo.dwEnOper = AVehEntryInfo.dwEnOper;
    m_vehEntryInfo.bEnVC = AVehEntryInfo.bEnVC;
    m_vehEntryInfo.bEnVT = AVehEntryInfo.bEnVT;
    m_vehEntryInfo.bEnVLPC = AVehEntryInfo.bEnVLPC;
    memcpy(m_vehEntryInfo.szEnVLP,AVehEntryInfo.szEnVLP,sizeof(m_vehEntryInfo.szEnVLP));
    memcpy(m_vehEntryInfo.szEnPSamTerm,AVehEntryInfo.szEnPSamTerm,sizeof(m_vehEntryInfo.szEnPSamTerm));
    m_vehEntryInfo.nFlagCount = AVehEntryInfo.nFlagCount;
    memcpy(m_vehEntryInfo.FlagInfo,AVehEntryInfo.FlagInfo,sizeof(m_vehEntryInfo.FlagInfo));
    m_vehEntryInfo.nPassTime = AVehEntryInfo.nPassTime;
    if(m_vehEntryInfo.nEntryType == Entry_ByCard || m_vehEntryInfo.nEntryType == Entry_ByQry)
    {
        curTime = QDateTime::currentDateTime();
        dwEnTime = SystemTime2CardTime(m_vehEntryInfo.EnTime);
        dwExitTime = SystemTime2CardTime(curTime);
        if(dwExitTime > dwEnTime)
        {
            m_vehEntryInfo.nPassTime = dwExitTime - dwEnTime;
        }
    }
}

void ExitTollItem::SetCalculateMoneyInfo(const TCalculateMoneyInfo& ACalculateMoney)
{
    m_CalculateMoneyInfo.nTruckBasicMoney = ACalculateMoney.nTruckBasicMoney;
    m_CalculateMoneyInfo.nTollMoney = ACalculateMoney.nTollMoney;
    m_CalculateMoneyInfo.nCardCost = ACalculateMoney.nCardCost;
    m_CalculateMoneyInfo.nOverWeightMoney = ACalculateMoney.nOverWeightMoney;
    m_CalculateMoneyInfo.nFreeMoney = ACalculateMoney.nFreeMoney;
    m_CalculateMoneyInfo.nRebateMoney = ACalculateMoney.nRebateMoney;
}

void ExitTollItem::SetTollMoneyInfo(const TTollMoneyInfo& ATollMoney)
{
    m_TollMoneyInfo.nCashCardMoney = ATollMoney.nCashCardMoney;
    m_TollMoneyInfo.nCashMoney = ATollMoney.nCashMoney;
}

void ExitTollItem::SetLastMoneyInfo(const TLastMoneyInfo& ALastMoney)
{
    m_LastMoneyInfo.nPaySn = ALastMoney.nPaySn;
    m_LastMoneyInfo.nLastMoney = ALastMoney.nLastMoney;
    m_LastMoneyInfo.nLastCardMoney = ALastMoney.nLastCardMoney;
}

void ExitTollItem::SetMoneyInfo(TTollMoneyInfo &TollMoney, TLastMoneyInfo &LastMoney, int nDisCount)
{
    bool bClearFree;
    if(nDisCount > 0 )
    {
        m_FareQryResult.Discount(nDisCount);
        SetMoneyByQryResultFare();
    }
    if(LastMoney.nPaySn > 0 || LastMoney.nLastMoney < m_CalculateMoneyInfo.nDisCountTollMoney)
    {
        bClearFree = (LastMoney.nPaySn > 1);
        m_FareQryResult.ModifyDiscountedMoney(LastMoney.nLastMoney,bClearFree);
    }
    m_TollMoneyInfo.nCashMoney = TollMoney.nCashMoney;
    m_TollMoneyInfo.nCashCardMoney = TollMoney.nCashCardMoney;
    m_LastMoneyInfo.nPaySn = LastMoney.nPaySn;
    m_LastMoneyInfo.nLastMoney = LastMoney.nLastMoney;
    m_LastMoneyInfo.nLastCardMoney = LastMoney.nLastCardMoney;
    RegetSplitInfo();
}

void ExitTollItem::SetInvPrintInfo(const TInvPrintInfo& AInvPrintInfo)
{
    m_InvPrintInfo.InvNum = AInvPrintInfo.InvNum;
    m_InvPrintInfo.InvCnt = AInvPrintInfo.InvCnt;
}

bool ExitTollItem::SetVehAxisInfo()
{
    //return getDeviceFactory()->getWtDev()->GetFirstVehAxisInfo(m_VehAxisInfo);
    return getWtDataMgr()->GetFirstVehAxisInfo(&m_pVehAxisInfo);
}

void ExitTollItem::setChangedCard(bool bChange)
{
    m_bChangedCard = bChange;
}

void ExitTollItem::UseMonthCard(const TMonthCardBasicInfo& MonthCardInfo)
{
    m_bUseMonthCard = true;
    m_MonthCardInfo = MonthCardInfo;
}

void ExitTollItem::SetDeleteInvoiceType(TDelInvoiceType nType)
{
    QString tmpStr;
    tmpStr = QString::fromLocal8Bit(m_HdgMsgToSend.InvPrintInfo.Inv);
    bool ok;
    if(tmpStr.toLongLong(&ok,10) > 0)
    {
        switch(nType)
        {
        case DIT_REPRINT:
            m_HdgMsgToSend.DealInfo.SpInfo[DS_TSIVRPT] = '1';
            break;
        case DIT_CANCEL:
            m_HdgMsgToSend.DealInfo.SpInfo[DS_TSIVERR] = '1';
        }
    }
}

bool ExitTollItem::CanReprint()
{
    if(FLastMsgToSend.TollInfo.CashMoney > 0)
    {
        return true;
    }else
        return false;
}

void ExitTollItem::ClearMonthCard()
{
    m_bUseMonthCard = false;
}

void ExitTollItem::ClearMoneyInfo()
{
    m_FareQryCondition.clear();
    m_FareQryResult.clear();
    m_proCardConsumeInfo.clear();
    m_nTollDistance = 0;
    m_nRealDistance = 0;
    m_nCardCost = 0;
    m_FreeArea.clear();
    m_CalculateMoneyInfo.clear();
    m_TollMoneyInfo.clear();
    m_LastMoneyInfo.clear();
    m_SplitInfo.clear();
    m_MonthCardInfo.clear();
    m_bChangedCard = false;
    m_bFreeUFee = false;
    m_iNoCard = 0;
    m_bUseMonthCard = false;
}

void ExitTollItem::ClearSavedInfo()
{
    TollItem::ClearSavedInfo();
    ClearVehEnInfo();
    ClearAxisInfo();
    ClearFlagStationInfo();
    ClearInvPrintInfo();
    ClearCardInfo();
    ClearSaveTime();
    m_bTollType = 0;
    m_bFreeUFee = false;
    m_bChangedCard = false;
    m_iNoCard = 0;
    m_bChangeEntryFlag = false;
    m_bIsUXing =false;
    m_bExceptCard = false;
    m_bOverTime = false;
    m_BadCardType = 0;
    m_bPayType = PYT_Unknow;
    m_proCardConsumeInfo.clear();
}

bool ExitTollItem::CalcFare(QString &sError)
{
    FareQueryConditionClass FareQryCondition;
    QSysParaDicParamFile* sysParam = NULL;
    QSysParamInfo sysInfo;

    //modify by wxk
    //读取费率配置，根据配置使用不同的计算方法
    QSettings setting("./Lane.ini", QSettings::IniFormat);
    int feeType = setting.value("FeeSetting/Type", 0).toInt();
    ::SetFtoLFlag(feeType);
    sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    sysInfo = sysParam->GetSysParamInfo();
    bool ret  = false;
    if(!SetFareQryCondition(FareQryCondition,sError))
    {
        return ret;
    }
    m_FareQryResult.clear();
    if(m_bOverTime || m_iNoCard == 11)
    {
        if(!m_FareCalc.QueryNoCardFare(&m_FareQryResult,FareQryCondition.m_vehTollInfo,FareQryCondition.m_nExSta,m_vehEntryInfo.dwEnStationID))
        {
            if(m_iNoCard == 11)
            {
                sError = tr("无卡最大里程计算通行费错误");
            }else
            {
                sError = tr("可疑超时车通行费计算错误");
            }
            m_bChangedCard = false;
            m_iNoCard = 0;
            return ret;
        }
    }
    else
    {
        if(m_bChangedCard)
        {
            if(!m_FareCalc.QueryChangeCareFare(&m_FareQryResult,FareQryCondition.m_vehTollInfo,FareQryCondition.m_nExSta,m_vehEntryInfo.dwEnStationID))
            {
                m_bChangedCard = false;
                sError = tr("换卡车通行费计算错误");
                return ret;
            }
        }else
            if(FareQryCondition.m_nEnSta == FareQryCondition.m_nExSta)
            {
                m_bIsUXing = true;
                if(!m_FareCalc.QueryUTypeFare(&m_FareQryResult,FareQryCondition))
                {
                    sError = tr("U型车通行费计算错误");
                    return ret;
                }
            }else
            {
                m_bIsUXing = false;
                if(!m_FareCalc.queryFare(&m_FareQryResult,FareQryCondition))
                {
                    sError = tr("通行费计算错误");
                    return ret;
                }
            }
    }
    if(m_iNoCard > 0)
    {
        m_nCardType = -1;
        m_nCardCost = sysInfo.m_nCardCost;
    }
    if(m_BadCardType == BadCard_Factitious)
    {
        m_nCardCost = sysInfo.m_nCardCost;
    }
    m_FareQryCondition = FareQryCondition;
    SetMoneyByQryResultFare();
    return true;
}

int ExitTollItem::GetOverTime()
{
    int ret = 0;
    const int minOver = 3600;
    if(m_vehEntryInfo.nEntryType != Entry_None && m_vehEntryInfo.nEntryType != Entry_ByManual)
    {
        if(m_vehEntryInfo.nPassTime - (int)m_FareQryResult.m_dwMaxSeconds > minOver)
        {
            ret = m_vehEntryInfo.nPassTime - (int)m_FareQryResult.m_dwMaxSeconds - minOver;
        }
    }
    return ret;
    return 3600;
}

void ExitTollItem::SendYewuWasteMsg(int nInductCnt)
{
    TMsgShiftSum MsgShiftSum;
    int MsgID,MsgType;
    FMsgToSend.VehInfo.VCnt = nInductCnt;
    FMsgToSend.VehInfo.InductCnt = nInductCnt;
    MsgID = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgID,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgID));
    MsgType = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgType,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgType));
    if(m_nViolateHeldBack == LST_TYPE_VIOLATE_LOGOUT || m_nViolateHeldBack == LST_TYPE_VIOLATE)
    {
        FMsgToSend.DealInfo.SpInfo[DS_SECG1] = '1';
    }
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,FMsgToSend,true,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("lane",tr("保存收费业务报文失败"));
    }
    memcpy(&FLastMsgToSend,&FMsgToSend,sizeof(FLastMsgToSend));
    LogMsg("lane",tr("SendYewuWasteMsg保存收费业务报文成功"));

}

void ExitTollItem::ReprintInvoiceMsg()
{
    TMsgShiftSum MsgShiftSum;
    m_dOccTime = QDateTime::currentDateTime();
    SendHedgeMsg();
    FMsgToSend.LaneInfo.WasteSN = getLaneInfo()->GetTransID();
    DateTimeToChar14(QDateTime::currentDateTime(),FMsgToSend.LaneInfo.OccurTime);
    FMsgToSend.InvPrintInfo.InvCnt = 1;
    IntToArChar(FMsgToSend.InvPrintInfo.Inv,sizeof(FMsgToSend.InvPrintInfo.Inv),getDataFileMgr()->GetInvInfo().PreInvBeginNo);
    FillHedgeMsg(FMsgToSend);
    getShiftSum()->Sum(FMsgToSend);
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    SendYewuWasteMsg(1);
}

void ExitTollItem::SendModWeightInfo(TModWeightType AModWeight)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 22;
    MsgType = AModWeight;
    SetVehAxisInfo();
    FillMsgToSend(MsgID,MsgType,msg,true);
    msg.DealInfo.SpInfo[DS_WEIGHTC] = '1';
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("保存计重信息修改报文失败"));
    }
    LogMsg("lane",tr("SendYewuWasteMsg保存计重信息修改报文成功"));
}

void ExitTollItem::SendInvDeal(TInvOperType AInvOper)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 23;
    MsgType = AInvOper;
    FillMsgToSend(MsgID,MsgType,msg,true);
    msg.DealInfo.SpInfo[DS_INVOCIEC] = '1';
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("保存票据操作报文失败"));
    }
    LogMsg("lane",tr("SendYewuWasteMsg保存票据操作报文成功"));
}

CPayType ExitTollItem::getPayType()
{
    return m_bPayType;
}

void ExitTollItem::setPayType(CPayType type)
{
    m_bPayType = type;
}

void ExitTollItem::setFreeType(FREE_REGION_TYPE type)
{
    m_FreeArea.bFreeType = type;
}

void ExitTollItem::SetProCardConsumeInfo(const TProCardConsumeInfo& AProCardConsumeInfo)
{
    m_proCardConsumeInfo.ConsumeTime = AProCardConsumeInfo.ConsumeTime;
    m_proCardConsumeInfo.dwMoney = AProCardConsumeInfo.dwTermSeq;
    memcpy(m_proCardConsumeInfo.psamTermNo,AProCardConsumeInfo.psamTermNo,sizeof(m_proCardConsumeInfo.psamTermNo));
    m_proCardConsumeInfo.dwTermSeq = AProCardConsumeInfo.dwTermSeq;
    m_proCardConsumeInfo.bTransType = AProCardConsumeInfo.bTransType;
    memcpy(m_proCardConsumeInfo.Spare1,AProCardConsumeInfo.Spare1,sizeof(m_proCardConsumeInfo.Spare1));
    m_proCardConsumeInfo.dwTac = AProCardConsumeInfo.dwTac;
    m_proCardConsumeInfo.wCardSeq = AProCardConsumeInfo.wCardSeq;
    m_proCardConsumeInfo.dwBalanceBefore = AProCardConsumeInfo.dwBalanceBefore;
    m_proCardConsumeInfo.dwBalanceAfter = AProCardConsumeInfo.dwBalanceAfter;
    m_proCardConsumeInfo.bKeyVer = AProCardConsumeInfo.bKeyVer;
    memcpy(m_proCardConsumeInfo.Spare2,AProCardConsumeInfo.Spare2,sizeof(m_proCardConsumeInfo.Spare2));
}

bool ExitTollItem::CalcTollFare(QString &strErr)
{
    return CalcFare(strErr);
}

bool ExitTollItem::SetFareQryCondition(FareQueryConditionClass& QryCondition,QString& sError)
{
    VehTollInfoClass TollVehInfo;
    int nExitStationID;
    quint32 dwFlagID;
    bool ret = false;
    QryCondition.clear();
    TollVehInfo.clear();
    TollVehInfo.m_bVehClass = (int)(m_VehInfo.VehClass);
    TollVehInfo.m_bChargeByVehClass = (!getLaneInfo()->getUseWeight());
    if(IsYTCard(m_ProCardBasicInfo))
    {
        //qDebug()<<"!!!!!!!!!!!bUserType:VehType"<<m_ProCardBasicInfo.bUserType<<m_VehInfo.VehType;
        if(m_ProCardBasicInfo.bUserType == CARD_UserType_FullFree || IsFullFreeVehTollMode(m_VehInfo.VehType))
        {
            TollVehInfo.m_bIsFullFreeVeh = true;
        }
    }
    else
    {
        if(m_bUseMonthCard)
        {
            if(m_MonthCardInfo.BaoJiaoInfo.bUserType == CARD_UserType_FullFree)
            {
                TollVehInfo.m_bIsFullFreeVeh = true;
            }
        }
        else
        {
            TollVehInfo.m_bIsFullFreeVeh = IsFullFreeVehTollMode(m_VehInfo.VehType);
        }
    }
    //只有包交、免交设置免费区间WXK==
    if(getCardType() >= CARD_TYPE_ROAD_FREE_STORE && getCardType() <= CARD_TYPE_ROAD_BAOJIAO_TALLY)
    {
        TollVehInfo.m_FreeRegion.SetFreeRegion(m_ProCardBasicInfo.bFreeRegionType,m_ProCardBasicInfo.FreeRegion,sizeof(m_ProCardBasicInfo.FreeRegion));
    }else
    {
        if(m_bUseMonthCard)
        {
            if(m_MonthCardInfo.BaoJiaoInfo.bUserType == CARD_UserType_RegionFree)
            {
                TollVehInfo.m_FreeRegion.SetFreeRegion(m_MonthCardInfo.BaoJiaoInfo.bFreeRegionType,m_MonthCardInfo.BaoJiaoInfo.FreeRegion,sizeof(m_MonthCardInfo.BaoJiaoInfo.FreeRegion));
            }
        }
    }
    if(!TollVehInfo.m_bChargeByVehClass && IsTruck(TollVehInfo.m_bVehClass) && !SetVehAxisInfo())
    {
        sError = tr("货车没有记重信息");
        return ret;
    }
    getWtDataMgr()->GetFirstVehAxisInfo(&m_pVehAxisInfo);
    if(m_pVehAxisInfo != NULL)
    {
        TollVehInfo.m_dwTotalWeight = m_pVehAxisInfo->GetWeight();
        TollVehInfo.m_nAxisNum = m_pVehAxisInfo->GetAxisCount();
    }
    QryCondition.m_vehTollInfo = TollVehInfo;
    QryCondition.m_nEnSta = m_vehEntryInfo.dwEnStationID;
    nExitStationID = getLaneInfo()->getOrgInfo().sOrgCode.toInt();
    QryCondition.m_nExSta = nExitStationID;
    if(QryCondition.m_nEnSta == QryCondition.m_nExSta)
    {
        QryCondition.m_nDriveTimeInSeconds = m_vehEntryInfo.nPassTime;
    }
    for(int i = 0;i<=m_vehEntryInfo.nFlagCount-1;i++)
    {
        dwFlagID = 94*10000 + m_vehEntryInfo.FlagInfo[i];
        QryCondition.m_FlagStaList.append(dwFlagID);
    }
    if(getLaneInfo()->getUseWeight())
    {
        m_bTollType = 2;
    }else
    {
        m_bTollType = 1;
    }
    return true;
}
