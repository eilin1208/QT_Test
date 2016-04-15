#ifndef TOLLITEM_H
#define TOLLITEM_H
#include "CommonModule/LaneType.h"
#include "CommonModule/MsgDefine.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/CardFileDef.h"
#include "CardReaderArm/qcardreader.h"
#include <QObject>
class TollItem : public QObject
{
    Q_OBJECT
public:
    explicit TollItem(QObject *parent = 0);
    //*** 收费员输入信息 ***
    //车辆输入信息
    CVehInfo m_VehInfo;
    void setCardType(qint16 type);
    int getCardType();
    void setPhysicalCardType(int cardType)
    {
       FPhysicalCardType =(CARD_TYPE_PHYSICAL)cardType;
    }
    CARD_TYPE_PHYSICAL getPhysicalCardType()
    {
        return FPhysicalCardType;
    }
    int getYWType()
    {
        return FnYwType;
    }
    void setYWType(int ywType)
    {
        FnYwType = ywType;
    }
    void setViolateHeldBack(int nType)
    {
        m_nViolateHeldBack = nType;
    }

protected:
    //**********图像信息*************
    //抓拍时间
    QDateTime FCaptureTime;
    //抓拍图像文件名
    QString FCapImageFile;
    //车牌图像抓拍时间
    QDateTime FVLPTime;
    //车牌图像文件名
    QString FVLPImageFile;
    //**********通信有关**********
    //当前业务类型，供发送流水用。
    //有分段付款(LST_TYPE_SUBPAY)
    //车辆引出LST_TYPE_CARLEADOUT
    //正常业务，LST_TYPE_NORMAL
    //其他业务类型在各具体报文内单独赋值
    int FnYwType;
    //业务流水
    TMsgToSend FMsgToSend;
    //上一笔业务报文备份(用来处理弃票，重打，废票)
    TMsgToSend FLastMsgToSend;
    //上次发送报文时间
    QDateTime FLastSendTime;
    //对冲流水
    TMsgToSend m_HdgMsgToSend;
    //存SQLite业务数据
    TDataToSave FYWDataToSave;
    //业务开始时间
    QDateTime m_dOccTime;
    //压线圈时间
    QDateTime FTiggerTime;
    //第一次输入车型时间
    QDateTime  FVTInputTime;
    //业务结束时间
    QDateTime FEndWasteTime;
    //**********保存卡片信息**************
    //CPU卡基本信息
    TProCardBasicInfo m_ProCardBasicInfo;
    //联网收费信息文件定义
    TCardTollInfo FCardTollInfo;
    //通行卡基本信息
    TTollCardBasicInfo FTollCardBasicInfo;
    //CPU卡交易前余额
    quint32 FdwBalance;
    //物理卡类型 物理卡类型(取用山东代码)
    int  BasicMaterialCardType;
    CARD_TYPE_PHYSICAL FPhysicalCardType;
    //卡类型,缺省为-1
    short int m_nCardType;
    //*********界面显示历史流水时使用******
    //历史流水变化
    /********************************************************************/
    //保存历史流水
    THistoryRec FHistRec;
    //***********填充特情位时使用**********
    //闯关 0-未知 20-下班闯关 21-闯关 22-误报警 23-闯关被拦截
    int m_nViolateHeldBack;
    //车辆黑名单
    bool m_bVehBlack;
    //车辆灰名单
    bool m_bVehGray;
    //卡黑名单
    bool m_bCardBlack;
    //**************************
    //设备状态
    char FDevStatus[32];
    TAuthorizeInfo FAuthInfo;
public:
    //入口信息变化
    /********************************************************************/
    //************** 报文填充函数 ********************
    //填充基本信息
    virtual void FillBasicInfo(const QDateTime& OccurTime,TMsgToSend &msg,bool bPeekSN = false);
    //填充车辆基本信息
    virtual void FillVehInfo(TMsgToSend &msg);
    //填充卡基本信息
    void FillICCardInfo(TMsgToSend &msg);
    //填充支付卡信息
    virtual void FillPayCardInfo(TMsgToSend &msg);
    //填充OBU信息
    void FillOBUInfo(TMsgToSend &msg);
    //填充卡箱信息
    void FillCardBoxInfo(TMsgToSend &msg);
    //按键信息
    void FillKeyInfo(TMsgToSend &msg);
    //填充交易状态信息
    virtual void FillDealInfo(TMsgToSend &msg);
    //授权信息
    void FillAuthInfo(TMsgToSend &msg);
    //计重信息
    virtual void FillWeightInfo(TMsgToSend &msg);
    //填充图片信息
    void FillImageInfo(TMsgToSend &msg,int nPicType);
    //填充冲证报文
    void FillHedgeMsg(TMsgToSend &msg);
    //填充单一位元
    virtual void FillSingleMsg(int nPos,TMsgToSend &Msg,bool bPeekSN);
    //根据位元表填MsgToSend各位元
    void FillMsgToSend(int MsgID,int MsgType,TMsgToSend &Msg,bool bPeekSN = false);
    //填充业务流水,在派生类内实现    abstract?
    virtual void FillYwWaste(QDateTime lpTime) = 0;
    //***************报文保存发送*********************
    //将MsgToSend 按位元压缩后存入数据库
    bool SaveMsg(int nSendType,int MsgID,int MsgType,TMsgToSend &msg,bool RemoveTmpData = false,void* PSynData = NULL,int nSynDataLen = 0);
    //保存防死机流水
    bool SaveTmpMsg(int MsgID,int MsgType,TMsgToSend &msg,void* PSynData = NULL,int nSynDataLen = 0);
    //清空报文内容
    void ClearSendMsg(TMsgToSend &msg);
    //设置第一次输入车型的时间
    void setVTInputTime(const QDateTime& sTime);
    TTollCardBasicInfo& getTollBasicInfo()
    {
        return FTollCardBasicInfo;
    }
    void setCardBlack(bool bCardBlack)
    {
        m_bCardBlack = bCardBlack;
    }
    TProCardBasicInfo& getProCardBasicInfo()
    {
        return m_ProCardBasicInfo;
    }
    quint32 getBalance()
    {
        return FdwBalance;
    }
    void setBalance(quint32 dwBa)
    {
        FdwBalance = dwBa;
    }

protected:
    //清除保存的时间
    void ClearSaveTime();
    //清除上一辆车的备份报文
    void ClearLastSendMsg();
    //清除图像信息
    void ClearImageInfo();

public:
    //清除卡片信息
    void ClearCardInfo();
    //清除保存的输入信息
    void ClearSavedInput();
    //清除保存的信息
    virtual void ClearSavedInfo();
    void Clear();
public:
    //设置计重信息
    virtual bool SetVehAxisInfo();
    //返回当前报文信息
    TMsgToSend GetMsgToSend();
    void SetViolateHeldBack(int nBack);
    //设置发生时间
    void SetOccurTime(const QDateTime& time);
public:
    //主界面显示业务流水
    void AddHistWaste(TMsgToSend &msg);
    //保存CPU卡信息
    void SetProCardInfo(TProCardBasicInfo &AProCardInfo);

    //保存卡中的通行信息
    void SetCardTollInfo(TCardTollInfo &ACardTollInfo);
    //设置授权信息
    void SetAuthInfo(TAuthorizeInfo &AuthInfo);
    //保存通行卡信息
    void SetTollCardInfo(TTollCardBasicInfo &ATollCardInfo);
    //设置设备状态
    void SetDevStatus(const TDevType ADevType,char AStatus);
    //SaveDevStatus(nType:Integer;bStatus:Byte);
    void SaveDevStatus(int nType,char bStatus);
    //单独设置IO状态
    void SetIOStatus(int nType,int IoIndex,char bStatus);
    //根据抓拍时间等，返回图像应该保存的相对目录和文件名（文件名根据图像时间及类型生成）
    QString FormatPicFileName(const QDateTime& CapTime,int nPicType,QString &sFileName);
    //保存图像信息
    void SetPicPath(QString szPicPath,const QDateTime &CapTime,int nPicType);
    void setFdwBlance(quint32 balance)
    {
        FdwBalance = balance;
    }

public:
    //大工班切换
    void NotifyWorkShiftChange();
    //获取上次存报文时间
    QDateTime GetLastSendTime();
    void Complete(const QDateTime& OccutTime,int nAllowPassCount,bool bFinished);
    //********** 报文发送相关函数 **************
    void SendStartMsg();
    //发送程序启动/关闭报文
    void SendAppMsg(bool bAppStart);
    //发送上班报文
    void SendLoginMsg();
    //发送下班报文(同时发送小班汇总) procedure SendLogoutMsg(); overload;
    void SendLogoutMsg();
    //补发下班报文 overload;
    void SendLogoutMsg(const TMsgShiftSum& ShiftSum);
    //发送小班合计
    virtual void SendSmallShiftSum(const char sEndTime[14]);
    //发送大班合计
    void  SendBigShiftSum();
    //发送冲减报文  入口倒车   出口冲减
    void SendHedgeMsg();
    //发送业务流水  procedure SendYewuWasteMsg(nInductCnt:Integer); virtual; abstract;
    virtual void SendYewuWasteMsg(int nInductCnt) = 0;
    //发送车队过车报文
    void SendMotorCarPassMsg(int nInductCount);
    //发送闯关报文
    void SendRushMsg(int rtype);
    //卡箱操作报文 1--上箱   2--下箱  3--强制下箱
    void SendCardBox(char AOperType);
    //发送参数启用报文
    void SendParamLoadMsg(int nFiled);
    //********以下报文为单独发送监控***************
    //发送刷鲁通卡报文 overload;
    void SendProCardInfo(TProCardBasicInfo &ProCardBasicInfo,quint32 dwBalance);
    //overload
    void SendProCardInfo(TProCardBasicInfo &ProCardBasicInfo,quint32 dwBalance,CVehEntryInfo& pEntryInfo);
    //监控回控信息
    bool SendRequestMonInfo(TMonReqID AMonReqID);
    //网络状态报文
    void SendNetStatus();
    //车辆信息输入变化报文
    void SendVehInputChange(TVehChangeType AVehChange);
    //车道事件报文
    void SendLaneEvent(TLaneEventType AEvent);
    //发送车队开始报文
    void SendMotorBeginMsg();
    //发送图片报文
    void SendImageMsg(const QString& imageFile);
    //发送车队结束报文
    void SendMotorEndMsg(int CarCount);
    //刷通行卡报文
    void SendTollCardInfo(TTollCardBasicInfo &TollCardBasicInfo);
    void SendTollCardInfo(TTollCardBasicInfo &TollCardBasicInfo,CVehEntryInfo &pEntryInfo);
signals:

public slots:

private:
    char m_zlibBuf[10240];
};

#endif // TOLLITEM_H
