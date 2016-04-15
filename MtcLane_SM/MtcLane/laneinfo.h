#ifndef LANEINFO_H
#define LANEINFO_H

#include <QObject>
#include "ParamModule/qoperparamfile.h"
#include "ParamModule/qshiftparamfile.h"
#include "datafilemgr.h"
#include "CommonModule/LaneType.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "CommonModule/MsgDefine.h"
#include "CommonModule/CardFileDef.h"
#ifdef Q_OS_UNIX
#include <sys/vfs.h>
#else
#include <windows.h>
#endif
enum emShiftState
{
    emPrepare,
    emOnWork
};
enum emCloseType
{
    emCloseApp,
    emRestartApp,
    emClosePC,
    emRestartPC
};
class LaneInfo : public QObject
{
    Q_OBJECT
public:
    explicit LaneInfo(QObject *parent = 0);
    bool loadDataFromIni();
    bool isMtcLane();
    bool isEntryLane();
    bool isAutoLane();
    int getCloseType();
    void setCloseType(int nType);
    bool getDiskSize(int &ntotal,int &navail);
public:
    //返回当前流水号，然后再加一
    quint8 GetTransID();
    //读取当前流水号，保持不变
    quint8 ReadTransID();
    //查询程序版本(返回数字,去掉.)
    quint32 GetAppVer();
    //查询程序版本字符串
    QString GetAppVerStr();
    //是否使用计重收费
    int getSpCardType()
    {
        return m_nSpCard;
    }
    void setSpCardType(int nSpCard)
    {
        m_nSpCard = nSpCard;
    }

    int getLaneType()
    {
        return m_nLaneType;
    }
    void setLaneType(int nLaneType)
    {
        m_nLaneType = nLaneType;
    }
    int getLaneCode()
    {
        return m_nLaneCode;
    }
    void setLaneCode(int nLaneCode)
    {
        m_nLaneCode = nLaneCode;
    }

    int getLapTop()
    {
        return m_nLapTop;
    }
    void setLapTop(int nLapTop)
    {
        m_nLapTop = nLapTop;
    }
    //StationInfo
    QString getDataSrvIp()
    {
        return m_sDataSrvIp;
    }
    int getControlDevType()
    {
        return m_controlDevType;
    }

    void setDataSrvIp(const QString& sDataSrvIp)
    {
        m_sDataSrvIp = sDataSrvIp;
    }
    quint32 getDataSrvPort()
    {
        return m_uDataSrvPort;
    }
    void setDataSrvPort(quint32 uDataSrvPort)
    {
        m_uDataSrvPort = uDataSrvPort;
    }

    QString getMonSrvIp()
    {
        return m_sMonSrvIp;
    }

    void setMonSrvIp(const QString& sMonSrvIp)
    {
        m_sMonSrvIp = sMonSrvIp;
    }

    quint16 getMonSrvPort()
    {
        return m_uMonSrvPort;
    }

    void setMonSrvPort(quint32 uPort)
    {
        m_uMonSrvPort = uPort;
    }

    int getPicSaveDay()
    {
        return m_nPicSaveDay;
    }
    void setPicSaveDay(int nPicSaveDay)
    {
        m_nPicSaveDay = nPicSaveDay;
    }
    int getXmlSaveDay()
    {
        return m_nXmlbkSaveDays;
    }
    void setXmlSaveDay(int nXmlSaveDay)
    {
        m_nXmlbkSaveDays = nXmlSaveDay;
    }
    int getSqlSaveDay()
    {
        return m_nSqlDBSaveDay;
    }
    void setSqlDay(int nday)
    {
        m_nSqlDBSaveDay = nday;
    }
    void setDiskSizeLimit(int size)
    {
        m_nDiskLimitSize = size;
    }
    int getDiskSizeLimit()
    {
        return m_nDiskLimitSize;
    }
    //LaneDBSrv
    QString getLaneDBSrvIp()
    {
        return m_sLaneDBSrvIp;
    }
    void setLaneDBSrvIp(const QString& sLaneDBSrvIp)
    {
        m_sLaneDBSrvIp = sLaneDBSrvIp;
    }

    QString getLaneDBSrvDataBase()
    {
        return m_sLaneDBSrvDataBase;
    }

    void setLaneDBSrvDataBase(const QString& sLaneDBSrvDataBase)
    {
        m_sLaneDBSrvDataBase = sLaneDBSrvDataBase;
    }
    QString getLaneDBSrvUser()
    {
        return m_sLaneDBSrvUser;
    }

    void setLaneDBSrvUser(const QString& sLaneDBSrvUser)
    {
        m_sLaneDBSrvUser = sLaneDBSrvUser;
    }
    QString getLaneDBSrvPW()
    {
        return m_sLaneDBSrvPW;
    }
    void setLaneDBSrvPW(const QString& sPw)
    {
        m_sLaneDBSrvPW = sPw;
    }
    QString getYWDataName()
    {
        return m_sYWDataName;
    }
    void setYWDataName(const QString& sYWDataName)
    {
        m_sYWDataName = sYWDataName;
    }
    QString getYWDataBackName()
    {
        return m_sYWDataBackName;
    }
    void setYWDataBackName(const QString& sYWDataBackName)
    {
        m_sYWDataBackName = sYWDataBackName;
    }
    QString getXMLPath()
    {
        return m_sXMLPath;
    }
    void setXMLPath(const QString& sXMLPath)
    {
        m_sXMLPath = sXMLPath;
    }

    int getUseSimulate()
    {
        return m_nUseSimulate;
    }
    void setUseSimulate(int nUseSimulate);
    //    {
    //        m_nUseSimulate = nUseSimulate;
    //    }

    int getUseFrontLoop()
    {
        return m_nUseFrontLoop;
    }

    void setUseFrontLoop(int nUseFrontLoop);


    int getUsePlate()
    {
        return m_nUsePlate;
    }

    void setUsePlate(int nUsePlate);
    //    {
    //        m_nUsePlate = nUsePlate;
    //    }

    int getUseWeight()
    {
        return m_nUseWeight;
    }
    void setUseWeight(int nUseWeight)
    {
        m_nUseWeight = nUseWeight;
    }

    bool getIsUseSpFree()
    {
        return m_bIsUseSpFree;
    }

    void setIsUseSpFree(bool bIsUseSpFree)
    {
        m_bIsUseSpFree = bIsUseSpFree;
    }


    SOperInfoValue& getOper()
    {
        return m_sOper;
    }
    void setOper(const SOperInfoValue& sOper)
    {
        m_sOper = sOper;
    }


    void setCarPlate(QString sCarPlate)
    {
        m_sCarPlate = sCarPlate;
    }
    QString getCarPlate()
    {
        return m_sCarPlate;
    }
    void setCarType(int nCarType)
    {
        m_nCarType = nCarType;
    }
    int getCarType()
    {
        return m_nCarType;
    }
    void setCarKind(QString sCarKind)
    {
        m_sCarKind = sCarKind;
    }
    QString getCarKind()
    {
        return m_sCarKind;
    }
    SShiftValue& getShiftInfo()
    {
        return m_shiftInfo;
    }

    void setShiftInfo(const SShiftValue& shiftInfo)
    {
        m_shiftInfo = shiftInfo;
    }

    CLaneStatus getShiftState()
    {
        return m_shiftState;
    }

    void setShiftState(CLaneStatus shiftState)
    {
        m_shiftState = shiftState;
    }
    SInvInfo getInvInfo()
    {
        return getDataFileMgr()->GetInvInfo();
    }
    QString getVehPlate()
    {
        return m_sVehPlate;
    }
    void setVehPlate(const QString& sVehPlate)
    {
        m_sVehPlate = sVehPlate;
    }

    int getVehType()
    {
        return m_nVehType;
    }

    void setVehType(int nVehClass)
    {
        m_nVehType = nVehClass;
    }

    int getVehClass()
    {
        return m_nVehClass;
    }
    int getVehPlateColor()
    {
        return m_nPlateColor;
    }

    void setVehPlateColor(int nColor)
    {
        m_nPlateColor = nColor;
    }

    void setVehClass(int nVehType)
    {
        m_nVehClass = nVehType;
    }
    void clearCurVehInfo();
    void clearVehCount()
    {
        m_nVehCount = 0;
    }
    int getVehCount()
    {
        return m_nVehCount;
    }
    void setVehCount(int nCount)
    {
        m_nVehCount = nCount;
    }

    void setCardNum(const QString& sCardNum)
    {
        m_sCardNum = sCardNum;
    }
    QString getCardNum()
    {
        return m_sCardNum;
    }

    void setVehPass(int nVehPass)
    {
        m_nVehPass = nVehPass;
    }
    int getVehPass()
    {
        return m_nVehPass;
    }

    void setCardCount(int nCardCount)
    {
        m_nCardCount = nCardCount;
    }
    int getCardCount()
    {
        return m_nCardCount;
    }
    QString getPicPath()
    {
        return m_PicPath;
    }
    void setPicPath(QString nPath)
    {
        m_PicPath = nPath;
    }
    int getRootPower()
    {
        return m_nRootPower;
    }
    void setRootPower(int nRootPower)
    {
        m_nRootPower = nRootPower;
    }

    SOrgCodeValue& getOrgInfo()
    {
        return m_iOrgInfo;
    }
    void setOrgInfo(SOrgCodeValue& orgInfo)
    {
        m_iOrgInfo = orgInfo;
    }
    void setLogTime(const QDateTime& iLogTime)
    {
        m_iLogTime = iLogTime;
    }
    QDateTime& getLogTime()
    {
        return m_iLogTime;
    }

    void setLogWaste(quint8 logWaste)
    {
        m_nLogWaste = logWaste;
    }
    quint8 getLogWaste()
    {
        return m_nLogWaste;
    }
    //是否为便携版
    bool IsLaptop()
    {
        return getLaneCode() == LaneType_PortableOut || getLaneCode() == LaneType_PortableIn;
    }

    //在下班状态下更新工班日和工班号，由主程序在下班状态下定时去调用
    void UpdateShiftDateWhenUnlogin();
    //获取车辆入口信息
    CVehEntryInfo* getVehEntryInfo()
    {
        return &m_vehEntryInfo;
    }
    //应收通行费信息
    TTollMoneyInfo* getTollMoneyInfo()
    {
        return &m_tollMoneyInfo;
    }

    //本次实收通行费
    TLastMoneyInfo* getLastMoneyInfo()
    {
        return &m_lastMoneyInfo;
    }
    //通行卡基本信息
    TTollCardBasicInfo* getTollCardBasicInfo()
    {
        return &m_tollCardBasicInfo;
    }

    //云通卡基本信息
    TProCardConsumeInfo* getProCardInfo()
    {
        return &m_proCardConsumeInfo;
    }
    //交易信息
    TCardTollInfo* getCardTollInfo()
    {
        return &m_CardTollInfo;
    }

    //设置监控类型
    void setMonType(TMonReqID reqId)
    {
        m_nMonType = reqId;
    }
    //获取监控类型
    TMonReqID getMonType()
    {
        return m_nMonType;
    }
    //获取车辆通过数
    int getAllowPass()
    {
        return m_nAllowPass;
    }
    void setAllowPass(int nAllow)
    {
        m_nAllowPass = nAllow;
    }
    void addAllowPass()
    {
        m_nAllowPass++;
    }
    void subAllowPass()
    {
        if(m_nAllowPass > 0)
        {
            m_nAllowPass--;
        }
    }

    bool getSubPay()
    {
        return m_bSubPay;
    }

    void setSubPay(bool bSubPay)
    {
        m_bSubPay = bSubPay;
    }

    void setDigRect(const QRect& rect)
    {
        m_rect = rect;
    }
    QRect getDigRect()
    {
        return m_rect;
    }
    int getBoxIndex(int index)
    {
        if(index > 3)
        {
            return 0;
        }
        return m_boxIndex[index];
    }
    void setBoxIndex(int index, int boxIndex)
    {
        if(index > 3)
        {
            return;
        }
        m_boxIndex[index] = boxIndex;
    }
    void setManualCapture(bool isUse)
    {
        m_manualCapture = isUse;
    }
    bool getUseManualCapture()
    {
        return m_manualCapture;
    }

signals:

public slots:
private:
    QRect m_rect;
    //VPRManualCapture
    bool m_manualCapture;
    //LaneInfo
    int m_nLaneType;            //车型
    int m_nLaneCode;            //车道号
    int m_nLapTop;              //
    //StationInfo
    QString m_sDataSrvIp;
    quint32 m_uDataSrvPort;
    QString m_sMonSrvIp;
    quint16 m_uMonSrvPort;
    int m_nPicSaveDay;
    int m_nXmlbkSaveDays;
    int m_nDiskLimitSize;
    int m_nSqlDBSaveDay;
    //LaneDBSrv
    QString m_sLaneDBSrvIp;
    QString m_sLaneDBSrvDataBase;
    QString m_sLaneDBSrvUser;
    QString m_sLaneDBSrvPW;
    QString m_sYWDataName;
    QString m_sYWDataBackName;
    QString m_sXMLPath;
    //Setup
    int m_nUseSimulate;
    int m_nUseFrontLoop;
    int m_nUsePlate;
    int m_nUseWeight;
    //SpFreeRoad
    bool m_bIsUseSpFree;
    //出口车辆信息
    //-
    QString m_sCarPlate;
    int m_nCarType;
    QString m_sCarKind;
    //-
    //操作员工班信息
    SOperInfoValue m_sOper;
    //工班信息
    SShiftValue m_shiftInfo;
    CLaneStatus m_shiftState;
    //当前车辆信息
    //车型
    int m_nVehClass;
    //车牌-
    QString m_sVehPlate;
    //车牌颜色-
    int m_nPlateColor;
    //车类
    int m_nVehType;
    //应收金额
    double m_dMoney;
    //卡号
    QString m_sCardNum;
    //优惠金额
    double m_dFavorMoney;
    //其他金额
    double m_dOtherMoney;
    //卡片数量
    int m_nCardCount;
    //过车数量
    int m_nVehPass;
    //车队过车数量
    int m_nVehCount;
    //图像路径
    QString m_PicPath;
    //本机构信息
    SOrgCodeValue m_iOrgInfo;
    //流水序列号
    quint8 m_nTransId;
    //登陆时间
    QDateTime m_iLogTime;
    //上班序列号
    quint8 m_nLogWaste;
    quint32 m_FBarDownDelay;   //落杆延时
    bool m_FbVPRManualCapture;  //车牌识别主动识别

    //linux系统中root权限 0无1有
    int m_nRootPower;
    //系统版本号
    QString m_sAppVer;
    //车辆入口信息
    CVehEntryInfo m_vehEntryInfo;
    //应收通行费
    TTollMoneyInfo m_tollMoneyInfo;
    //本次实收通信费
    TLastMoneyInfo m_lastMoneyInfo;

    //收费信息
    TCardTollInfo m_CardTollInfo;
    //通行卡基本信息
    TTollCardBasicInfo m_tollCardBasicInfo;
    //云通卡交易信息
    TProCardConsumeInfo m_proCardConsumeInfo;
    //请求监控类型
    TMonReqID m_nMonType;
    //是否使用云通卡和现金分段付费
    bool m_bSubPay;
    //特殊卡片类型
    int m_nSpCard;
    //关闭类型
    int m_nCloseType;
private:
    bool m_bShutDown;
    int m_nAllowPass;
    int m_boxIndex[4];
    //使用费显orIO控制通行灯与报警 0-IO, 1-FD
    int m_controlDevType;
};

LaneInfo* getLaneInfo();

#endif // LANEINFO_H
