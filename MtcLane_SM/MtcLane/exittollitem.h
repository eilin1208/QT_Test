#ifndef EXITTOLLITEM_H
#define EXITTOLLITEM_H

#include <QObject>

#include "tollitem.h"
#include "WtArm/wtdatadefineunit.h"
#include "FareCalc/farequeryresultclass.h"
#include "FareCalc/FareCalc.h"
enum TDelInvoiceType
{
    DIT_REPRINT,  //重打
    DIT_CANCEL   //废票
};
class ExitTollItem : public TollItem
{
    Q_OBJECT
private:
    //是否为U型车
    bool  m_bIsUXing;
    //计算费率条件
    FareQueryConditionClass m_FareQryCondition;
    //计算费率结果
    FareQueryResultClass m_FareQryResult;
    //
    FareCalc m_FareCalc;
protected:
    //CPU卡消费信息
    TProCardConsumeInfo  m_proCardConsumeInfo;
    //车辆入口信息
    CVehEntryInfo m_vehEntryInfo;
    //*******************计费基础信息*****************************
    //计费里程
    quint32  m_nTollDistance;
    //实际里程
    quint32 m_nRealDistance;
    //免费区域
    TFreeAreaInfo m_FreeArea;
    ////计费方式 1车型计费 2吨公里
    char m_bTollType;
    //卡成本
    int m_nCardCost;
    //支付方式
    CPayType m_bPayType;
    //计算通行费
    TCalculateMoneyInfo m_CalculateMoneyInfo;
    //应实收通行费,分段支付等于实收通行费
    TTollMoneyInfo m_TollMoneyInfo;
    //实收通行费
    TLastMoneyInfo m_LastMoneyInfo;
    //打印票据信息
    TInvPrintInfo m_InvPrintInfo;
    //拆分信息
    TSplitInfo m_SplitInfo;
    //车辆计重信息
    TWtVehicle *m_pVehAxisInfo;
    //标识站信息
    TFlagStationInfo FFlagStationInfo;
    //换卡
    bool m_bChangedCard;
    //U型免费
    bool m_bFreeUFee;
    //是否为无卡 10-无卡 11-无卡最大里程
    int m_iNoCard;
    //是否已经改写入出口标志
    bool m_bChangeEntryFlag;
    //可疑超时  赋值为True
    bool m_bOverTime;
    //异常云通卡  用于云通卡转现金支付使用 当云通卡在黑名单中、车卡不符时 赋值为True
    bool m_bExceptCard;
    bool m_bUseMonthCard;
    //包交卡基本信息
    TMonthCardBasicInfo m_MonthCardInfo;
    //坏卡类型  1-正常损坏 2-人为损坏
    int m_BadCardType;
protected:
    //清除入口信息
    void ClearVehEnInfo();
    void ClearAxisInfo();
    void ClearFlagStationInfo();
    void ClearInvPrintInfo();
    //设置费率查询条件
    bool SetFareQryCondition(FareQueryConditionClass& QryCondition,QString& sError);
    //设置从费率表中计算的费率
    void SetMoneyByQryResultFare();
    //重新获取拆分信息
    void RegetSplitInfo();
protected:
    //填充支付卡信息
    virtual void FillPayCardInfo(TMsgToSend &msg);
    virtual void FillDealInfo(TMsgToSend &msg);
    void FillVehEnInfo(TMsgToSend &msg);
    virtual void FillWeightInfo(TMsgToSend &msg);
    void FillTollBaseInfo(TMsgToSend &msg);
    void FillTollInfo(TMsgToSend &msg);
    void FillSplitInfo(TMsgToSend &msg);
    void FillInvModefyInfo(TMsgToSend &msg);
    void FillInvPrintInfo(TMsgToSend &msg);
    virtual void FillSingleMsg(int nPos, TMsgToSend &Msg, bool bPeekSN);
    virtual void FillYwWaste(QDateTime lpTime);


public:
     //设置入口车辆信息
    void SetVehEntryInfo(const CVehEntryInfo &AVehEntryInfo);
    void SetCalculateMoneyInfo(const TCalculateMoneyInfo& ACalculateMoney);
    void SetTollMoneyInfo(const TTollMoneyInfo& ATollMoney);
    void SetLastMoneyInfo(const TLastMoneyInfo& ALastMoney);
    void SetMoneyInfo(TTollMoneyInfo &TollMoney,TLastMoneyInfo &LastMoney,int nDisCount = 0);
    void SetInvPrintInfo(const TInvPrintInfo& AInvPrintInfo);
    bool SetVehAxisInfo();
    void setChangedCard(bool bChange);
    TProCardConsumeInfo &getProCardConsum()
    {
        return m_proCardConsumeInfo;
    }
    TWtVehicle* GetVehAxisInfo()
    {
        return m_pVehAxisInfo;
    }

    void SetProCardConsumeInfo(const TProCardConsumeInfo& AProCardConsumeInfo);
    void UseMonthCard(const TMonthCardBasicInfo& MonthCardInfo);
    void SetDeleteInvoiceType(TDelInvoiceType nType);
    FareQueryResultClass& getFareQryResult()
    {
        return m_FareQryResult;
    }
    int getCardCost()
    {
        return m_nCardCost;
    }
    void setBadCardType(qint16 nType)
    {
        m_BadCardType = nType;
    }

    TCalculateMoneyInfo& getCalculateMoneyInfo()
    {
        return m_CalculateMoneyInfo;
    }
    CVehEntryInfo& getVehEntryInfo()
    {
        return m_vehEntryInfo;
    }

    bool getChangeEntryFlag()
    {
        return m_bChangeEntryFlag;
    }
    void setChangeEntryFlag(bool bFlag)
    {
        m_bChangeEntryFlag = bFlag;
    }
    bool setiNoCard(int inocard)
    {
        if(m_iNoCard == inocard)
            return true;
        return false;
    }
    void setChangeEntryOvertime(bool bOvertime)
    {
        m_bOverTime = bOvertime;
    }
    void setExceptCard(bool bExceptcard)
    {
        m_bExceptCard = bExceptcard;
    }
    bool getExceptCard()
    {
        return m_bExceptCard;
    }
    TLastMoneyInfo& getLastMoney()
    {
        return m_LastMoneyInfo;
    }
    TTollMoneyInfo& getTollMoney()
    {
        return m_TollMoneyInfo;
    }

public:
    bool CanReprint();
    void ClearMonthCard();
    void ClearMoneyInfo();
    //清空所有信息
    void ClearSavedInfo();
    bool CalcFare(QString &sError);
    bool CalcTollFare(QString &strErr);
    int GetOverTime();
    void setOverTime(bool bOverTime)
    {
        m_bOverTime = bOverTime;
    }

    void setNoCard(int nNoCard)
    {
        m_iNoCard = nNoCard;
    }
    void setUFree(bool bFree)
    {
        m_bFreeUFee = bFree;
    }
    bool IsUXing()
    {
        return m_bIsUXing;
    }
    void setUXing(bool bUXing)
    {
        m_bIsUXing = bUXing;
    }

public:
     //*** 报文发送相关函数 ***
    void SendYewuWasteMsg(int nInductCnt);
    void ReprintInvoiceMsg();
    void SendModWeightInfo(TModWeightType AModWeight);
    void SendInvDeal(TInvOperType AInvOper);
    void SendMoneyInfo(const TTollMoneyInfo& TollMoney,const TCalculateMoneyInfo& CalculateMoney);
public:
    explicit ExitTollItem(QObject *parent = 0);
    ~ExitTollItem();
    CPayType getPayType();
    void setPayType(CPayType type);
    void setFreeType(FREE_REGION_TYPE type);
signals:

public slots:

};

#endif // EXITTOLLITEM_H
