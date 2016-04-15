#include "rsuctrl.h"
#include "QDebug"
#include <QMessageBox>
#include "laneinfo.h"
rsuctrl::rsuctrl(const QString& sName, QObject *parent) :
    DevBaseInterface(sName,parent)
{
    qRegisterMetaType<QVariant>("QVariant");
    //内部数据初始化
    RsudataInit();
    //只有接受到完整的帧数据时，才触发槽函数；
    connect(GetRsuArmCommInstance(),SIGNAL(readyReadFrameData(QByteArray))
            ,this,SLOT(OnFrameDataReceived(QByteArray)));
    OnRsuDevStatusTimer();
    SetCanAnalyze(false);
    //设置日志是否打印
    GetRsuArmCommInstance()->SetDebugflag(getLaneInfo()->getbDebug());
}

void rsuctrl::RsudataInit()
{

    qRegisterMetaType<QVariant>("QVariant");//dafang
    mComNumber=0;
    mbaterate=0;
    bInitSucc=false;
    RSUInfo.initQRSUState();
    sMessage.clear();
    mLaneMode=3;
    mWaitTime=getLaneInfo()->getOBURetryTime();
    mPower=getLaneInfo()->GetRSUPower();
    m_SendingRsctl = 0x89;
    m_RecivedRsctl = 0;
    mFrameType=0;
    bIsBCCPassed=false;
    sB0Info.initQRSUState();
    sB2Info.initQFrame_B2();
    sB3Info.initQFrame_B3();
    sB4Info.initQFrame_B4();
    sB5Info.initQFrame_B5();
    sB6Info.initQFrame_B6();
    GetRsuArmCommInstance();
    nowRecivedOBUID = 0;
    isFirstOpen = true;
}

bool rsuctrl::InitRSUDev()
{
    QDateTime CurrentTime;
    char buf[18];
    quint32 UnixTime;
    QTSYSTEMTIME SystemTime;
    //保存设备状态
    SetDevStatus(RsuInit);
    //报文数据结构的填充
    buf[0]=0x89;//m_SendingRsctl;
    buf[1]=0xc0;
    CurrentTime=QDateTime::currentDateTime();
    UnixTime=QDateTimeToUnixTime2000(CurrentTime);
    UnixTime=Swapquint32(UnixTime);
    memcpy(buf+2,&UnixTime,4);
    QDateTime2SysTime(CurrentTime,&SystemTime);
    buf[6]=char(BYTEToBCD(SystemTime.wYear/100));
    buf[7]=char(BYTEToBCD(SystemTime.wYear%100));
    buf[8]=char(BYTEToBCD(SystemTime.wMonth));
    buf[9]=char(BYTEToBCD(SystemTime.wDay));
    buf[10]=char(BYTEToBCD(SystemTime.wHour));
    buf[11]=char(BYTEToBCD(SystemTime.wMinute));
    buf[12]=char(BYTEToBCD(SystemTime.wSecond));
    buf[13]=char(mLaneMode);
    buf[14]=char(mWaitTime);
    buf[15]=char(mPower);
    //信道号
    buf[16]=0;
    //是否处理标识站信息
    buf[17]=1;
    //通过天线进行发送指令    
    if(GetRsuArmCommInstance()->sendFrameData(buf,18)<=0)
        return false;
    return true;
}

bool rsuctrl::ContinueDeal(quint32 OBUID)
{
    char buf[6];
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc1;
    OBUID=Swapquint32(OBUID);
    memcpy(buf+2,&OBUID,4);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,6)<=0)
        return false;
    return true;
}

bool rsuctrl::StopDeal(quint32 OBUID)
{
    char buf[6];
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc2;
    OBUID=Swapquint32(OBUID);
    memcpy(buf+2,&OBUID,4);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,6)<=0)
        return false;
    return true;
}

bool rsuctrl::OpenRSU()
{
    char buf[3];
    buf[0]=char(m_SendingRsctl);
    buf[1]=0x4c;
    buf[2]=0x01;
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,3)<=0)
        return false;
    return true;
}

bool rsuctrl::CloseRSU()
{
    char buf[3];
    buf[0]=char(m_SendingRsctl);
    buf[1]=0x4c;
    buf[2]=0x00;
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,3)<=0)
        return false;
    return true;
}

bool rsuctrl::WriteEntryInfo(quint32 OBUID, const TCardTollInfo &EntryInfo, bool bOldCard)
{
    char buf[53];
    QFrame_C3Raw raw;
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc3;
    GetRsuTransFrameInstance()->TransC3(raw,OBUID,EntryInfo,bOldCard);
    memcpy(buf+2,&raw,51);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,53)<=0)
        return false;
    return true;
}

bool rsuctrl::WriteExitInfo(quint32 OBUID, quint32 Money, const TCardTollInfo &ExitInfo, bool bOldCard)
{
    char buf[57];
    QFrame_C6Raw raw;
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc6;
    GetRsuTransFrameInstance()->TransC6(raw,OBUID,Money,ExitInfo,bOldCard);
    memcpy(buf+2,&raw,55);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,57)<=0)
        return false;
    return true;
}

bool rsuctrl::WriteFlagStationInfo(const QFrame_C4 &FlagStation)
{
    char buf[15];
    QFrame_C4Raw raw;
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc4;
    GetRsuTransFrameInstance()->TransC4(raw,FlagStation);
    memcpy(buf+2,&raw,13);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,15)<=0)
        return false;
    return true;
}

bool rsuctrl::GetFreeArea(quint32 OBUID)
{
    char buf[6];
    buf[0]=char(m_SendingRsctl);
    buf[1]=0xc5;
    OBUID=Swapquint32(OBUID);
    memcpy(buf+2,&OBUID,4);
    //通过天线进行发送指令
    if(GetRsuArmCommInstance()->sendFrameData(buf,6)<=0)
        return false;
    return true;
}

void rsuctrl::TodoProcessOBU(QFrame_B2 &Aobu,RSUERRORCODE errCode)
{
    LogMessageToFile(tr("将要处理OBU信息(B2)"));
    enumCurrentState=RsuEvent_OBUBaseInfo;
    frameVar.setValue(Aobu);
    transErrVar.setValue(errCode);
    emit signalWaitProcessOBUBaseInfo(frameVar,transErrVar);
}

void rsuctrl::TodoProcessOBUInfo(QFrame_B3 &Aobu, RSUERRORCODE errCode)
{
    LogMessageToFile(tr("处理OBU属性信息(B3)"));
    enumCurrentState=RsuEvent_OBUVehInfo;

    frameVar.setValue(Aobu);
    transErrVar.setValue(errCode);
    emit signalWaitProcessOBUinfo(frameVar,transErrVar);
}

void rsuctrl::TodoProcessCardInfo(QFrame_B4 &Aobu, RSUERRORCODE errCode)
{
    LogMessageToFile(tr("处理卡信息(B4)"));
    enumCurrentState=RsuEvent_IccInfo;
    frameVar.setValue(Aobu);
    transErrVar.setValue(errCode);
    emit signalWaitProcessCardInfo(frameVar,transErrVar);
}

void rsuctrl::TodoProcessSuccInfo(QFrame_B5 &Aobu, RSUERRORCODE errCode)
{
    LogMessageToFile(tr("开始处理(B5)信息"));
    enumCurrentState=RsuEvent_TransResult;

    frameVar.setValue(Aobu);
    transErrVar.setValue(errCode);
    emit signalWaitProcessSuccInfo(frameVar,transErrVar);
}

void rsuctrl::TodoProcessFreeInfo(QFrame_B6 &Aobu, RSUERRORCODE errCode)
{
    LogMessageToFile(tr("处理免费信息"));
    enumCurrentState=RsuEvent_FreeArea;

    frameVar.setValue(Aobu);
    transErrVar.setValue(errCode);
    emit signalWaitProcessFreeInfo(frameVar,transErrVar);
}

quint16 rsuctrl::GetComNumber()
{
    return mComNumber;
}

quint16 rsuctrl::GetBaterate()
{
    return mbaterate;
}

bool rsuctrl::GetflagInitComm()
{
    return bInitSucc;
}

QRSUState rsuctrl::GetRsuInfo()
{
    return RSUInfo;
}

QString rsuctrl::GetLastMessageLog()
{
    return sMessage;
}

bool rsuctrl::InitComm()
{
    //bool bRet = false;
    //打开串口
    if(!GetRsuArmCommInstance()->isOpenComm())
    { //串口参数设置
        GetRsuArmCommInstance()->setPortName(m_sConStr1);
        GetRsuArmCommInstance()->portSettings = QPortSettings(m_sConStr2);
        GetRsuArmCommInstance()->setPortSettings(GetRsuArmCommInstance()->portSettings);

        if(!GetRsuArmCommInstance()->openComm())
        {
            LogMessageToFile(tr("串口打开失败"));
            return false;
        }
        else
        {
            LogMessageToFile(tr("串口打开成功"));
            return true;
        }
    }
    else
    {
        LogMessageToFile(tr("串口已经打开，不要重复打开"));
        return true;
    }
}

void rsuctrl::LogMessageToFile(const QString &Message)
{
    LogMsg("lane",Message);
}

void rsuctrl::doDisplayETCMsg(const QString &message)
{
    LogMsg("lane",message);
}

void rsuctrl::OnRsuDevStatusTimer()
{
    mTimerRsuDevStatus.stop();
    disconnect(&mTimerRsuDevStatus,SIGNAL(timeout()),0,0);
    mTimerRsuDevStatus.setSingleShot(true);
    connect(&mTimerRsuDevStatus,SIGNAL(timeout()),
            this,SLOT(onRsuDevStatusTimerOver()));
    mTimerRsuDevStatus.start(90000);
}

quint8 rsuctrl::GetLaneMode()
{
    return mLaneMode;
}

quint8 rsuctrl::GetWaitTime()
{
    return mWaitTime;
}

quint8 rsuctrl::GetPower()
{
    return mPower;
}

void rsuctrl::SetComNumber(quint16 comNumber)
{
    mComNumber=comNumber;
}

void rsuctrl::SetBaterate(quint16 BateRate)
{
    mbaterate=BateRate;
}

void rsuctrl::SetFlagInit(bool flagInit)
{
    bInitSucc=flagInit;
}

void rsuctrl::SetRSuInfo(QRSUState RSuInfo)
{
    RSUInfo.CopyData(RSuInfo);
}

void rsuctrl::SetLaneMode(quint8 laneMode)
{
    mLaneMode=laneMode;
}

void rsuctrl::SetWaitTime(quint8 WaitTime)
{
    mWaitTime=WaitTime;
}

void rsuctrl::SetPower(quint8 power)
{
    mPower=power;
}

void rsuctrl::LogAndDebugToConsle(const QString &logMessage)
{
    qDebug()<<"The rsuctrl Debug Message:"<<logMessage;
}
rsuctrl *GetRsuInstance()
{
    static rsuctrl rsuctl("RSU");
    return &rsuctl;
}
bool rsuctrl::LoadFunction()
{
    return true;
}

bool rsuctrl::ReleaseFunction()
{
    return true;
}

bool rsuctrl::Close()
{
    //关闭天线设备
    if(!GetRsuArmCommInstance()->isOpenComm())
    {
        LogMessageToFile(tr("天线正常关闭失败,串口提前关闭了"));
        return false;
    }
    if(!CloseRSU())
    {
        LogMessageToFile(tr("天线正常关闭失败,C4命令发送失败"));
        return false;
    }
    GetRsuArmCommInstance()->closeComm();
    return true;
}

bool rsuctrl::Init()
{
    //页面日志
    emit signalMessageToFrmMain(tr("开始初始化RSU"));
    qDebug()<<"开始初始化RSU";
    //打开天线设备
    if(!InitComm())
    {
        LogMessageToFile(tr("打开天线失败,串口失败"));
        return false;
    }
    if(isFirstOpen == false)
    {
        if(!OpenRSU())
        {
            LogMessageToFile(tr("打开天线失败,4c命令发送失败"));
            return false;
        }
        if(!InitRSUDev())
        {
            LogMessageToFile(tr("打开天线失败,C0命令发送失败"));
            return false;
        }
    }
    else
    {
        isFirstOpen = false;
    }
    mTimerRSUInit.stop();
    disconnect(&mTimerRSUInit,SIGNAL(timeout()),0,0);
    return true;
}
void rsuctrl::OnFrameDataReceived(QByteArray FrameData)
{
    //测试程序用
    emit signalRecivedFrameData(FrameData);
    //检测是否允许进行解析数据
    if(!m_canAnalyze)
        return;
    //接收到帧数据后的处理流程  没有帧头ffff 和帧尾ff
    int ret=FrameDataProcess(FrameData);
    //日志信息调试打印
    QString temp=GetErrorMessage(ret);
    if(!temp.isEmpty())
        LogMessageToFile(GetErrorMessage(ret));
}

int rsuctrl::FrameDataProcess(QByteArray &message)
{
    emit signalRSUStateMessageToFrmMain(0);
    OnRsuDevStatusTimer();
    //数据长度检测
    if(message.length()<3)
    {
        return rsuLenghError;
    }
    //数据(CRC)校验
    if(!GetRsuArmCommInstance()->FrameBCCPass(message))
    {
        SetBCCPassed(false);
        return rsuBCCNoPassed;
    }
    SetBCCPassed(true);
    //记录当前帧的类型
    SetFrameType((quint8)message.at(1));
    //当前帧序列号的转换和记录
    m_RecivedRsctl = (quint8)message.at(0);
    SetBeSendingRsctl(m_RecivedRsctl);

    //按照帧类型进行分发信号
    return FrameDataAnalyze(message);
}

void rsuctrl::SetBeSendingRsctl(quint8 Rsctl)
{
    m_SendingRsctl=Swap4Bits(Rsctl);
}

void rsuctrl::SetBCCPassed(bool BCCPassed)
{
    bIsBCCPassed=BCCPassed;
}

quint8 rsuctrl::GetBeSendingRsctl()
{
    return m_SendingRsctl;
}

bool rsuctrl::GetBCCPassed()
{
    return bIsBCCPassed;
}

void rsuctrl::SetFrameType(quint8 FrameType)
{
    mFrameType=FrameType;
}

quint8 rsuctrl::GetFrameType()
{
    return mFrameType;
}

int rsuctrl::FrameDataAnalyze(QByteArray &message)
{
    ret = rsuNoError;
    switch(GetFrameType())
    {
    case 0xb0:
        //进行B0帧的解析
        ret=GetRsuTransFrameInstance()->TransB0(sB0Info,message);
        TodoProcessRSU(sB0Info,ret);
        //保存设备状态
        SetDevStatus(RsuNoError);
        //判断是否重新上电
        if(m_RecivedRsctl==0x98)
        {
            LogMessageToFile(tr("RSU 重新上电"));
            if(InitRSUDev())
            {
                LogMessageToFile(tr("RSU 重新上电,成功"));
            }
            else
            {
                LogMessageToFile(tr("RSU 重新上电,失败"));
            }
        }
        //get the rsctl ==0x08
        if(m_RecivedRsctl == 0x08)
        {
            //发送C1指令给RSU
            ContinueDeal(nowRecivedOBUID);
        }
        break;
    case 0xb2:
        //心跳判断  1分钟一次
        if((quint8)message.at(6) == 0x80)
        {
            QString msg = QString(tr("心跳帧数据：%1")).arg(message.toHex().data());
            StopDeal(0x0000);
            //            TodoProcessOBU(sB2Info,rsuHeartBeat);
            emit signalMessageToFrmMain(tr("RSU心跳信息"));
            LogMessageToFile(msg);
        }
        else
        {
            //刀刀亮--交易时间测试
            GetEtcLaneInstance()->SetTimeDiff(QDateTime::currentMSecsSinceEpoch());
            //进行B2帧的解析
            ret=GetRsuTransFrameInstance()->TransB2(sB2Info,message);
            nowRecivedOBUID=sB2Info.OBUID;
            TodoProcessOBU(sB2Info,ret);
        }
        break;
    case 0xb3:
        //刀刀亮--交易时间测试
        LogMsg("RSU",tr("处理完B2,回复C1,现在接收到B3，共费时：%1毫秒")
               .arg(QDateTime::currentMSecsSinceEpoch()-
                    GetEtcLaneInstance()->GetTimeDiff()));
        GetEtcLaneInstance()->SetTimeDiff(QDateTime::currentMSecsSinceEpoch());
        //进行B3帧的解析
        ret=GetRsuTransFrameInstance()->TransB3(sB3Info,message);
        nowRecivedOBUID=sB3Info.OBUID;
        TodoProcessOBUInfo(sB3Info,ret);
        break;
    case 0xb4:
        //刀刀亮--交易时间测试
        LogMsg("RSU",tr("处理完B3,回复C1,现在接收到B4，共费时：%1毫秒")
               .arg(QDateTime::currentMSecsSinceEpoch()-
                    GetEtcLaneInstance()->GetTimeDiff()));
        GetEtcLaneInstance()->SetTimeDiff(QDateTime::currentMSecsSinceEpoch());

        //进行B4帧的解析
        ret=GetRsuTransFrameInstance()->TransB4(sB4Info,message);
        nowRecivedOBUID=sB4Info.OBUID;
        TodoProcessCardInfo(sB4Info,ret);
        break;
    case 0xb5:
        //刀刀亮--交易时间测试
        LogMsg("RSU",tr("处理完B4,回复C1,现在接收到B5，共费时：%1毫秒")
               .arg(QDateTime::currentMSecsSinceEpoch()-
                    GetEtcLaneInstance()->GetTimeDiff()));
        GetEtcLaneInstance()->SetTimeDiff(QDateTime::currentMSecsSinceEpoch());

        //进行B5帧的解析
        ret=GetRsuTransFrameInstance()->TransB5(sB5Info,message);
        nowRecivedOBUID=sB5Info.OBUID;
        TodoProcessSuccInfo(sB5Info,ret);
        break;
    case 0xb6:
        //进行B6帧的解析
        ret=GetRsuTransFrameInstance()->TransB6(sB6Info,message);
        nowRecivedOBUID=sB6Info.OBUID;
        TodoProcessFreeInfo(sB6Info,ret);
        break;
    }
    return ret;
}

void rsuctrl::TodoProcessRSU(QRSUState &Aobu,RSUERRORCODE errCode)
{
    LogMessageToFile(tr("将要处理RSU信息(B0)"));
    enumCurrentState=RsuEvent_RsuState;
    QString tempdata=QString::fromLocal8Bit((char*)Aobu.RSUTerminalID1,6);
    if(!tempdata.isEmpty())
    {
        sPsam1=Utf8toGB2312(tempdata);
    }
    tempdata=QString::fromLocal8Bit((char *)Aobu.RSUTerminalID2,6);
    if(!tempdata.isEmpty())
    {
        sPsam2=Utf8toGB2312(tempdata);
    }
    emit signalWaitInit();
}

const QRSUState *rsuctrl::GetB0Info()
{
    return &sB0Info;
}
//错误代码的解释
QString rsuctrl::GetErrorMessage(const quint16 errorCode)
{
    QString temp;
    temp.clear();
    switch(errorCode)
    {
    case rsuNoError:
        temp=QObject::tr("RSU解析此帧，没有任何异常");
        break;
    case rsuLenghError:
        temp=QObject::tr("RSU解析时，数据帧长度出现错误");
        break;
    case rsuFormatError:
        temp=QObject::tr("RSU数据格式化时错误");
        break;
    case rsuNoCardInsert:
        temp=QObject::tr("RSU解析时，发现OBU没有插入IC卡");
        break;
    case rsuCardAbnormal:
        temp=QObject::tr("RSU解析时，发现卡异常");
        break;
    case rsuCardLocked:
        temp=QObject::tr("RSU解析时，发现卡被锁住");
        break;
    case rsuCardDisassembled:
        temp=QObject::tr("RSU解析时，发现OBU被拆卸");
        break;
    case rsuCardPowerAbnormal:
        temp=QObject::tr("RSU解析时，发现电量不足");
        break;
    case rsuRecivedB3CodeError:
        temp=QObject::tr("RSU解析时，B3帧状态码异常");
        break;
    case rsuRecivedB4CodeError:
        temp=QObject::tr("RSU解析时，B4帧状态码异常");
        break;
    case rsuCardTypeUnknown:
        temp=QObject::tr("RSU解析时，发现卡类型未知");
        break;
    case rsuRecivedB5CodeError:
        temp=QObject::tr("RSU解析时，B5帧状态码异常");
        break;
    case rsuRecivedB6CodeError:
        temp=QObject::tr("RSU解析时，B6帧状态码异常");
        break;
    case rsuBCCNoPassed:
        temp=QObject::tr("RSU解析时，发现CRC检验不通过");
        break;
    case rsuHeartBeat:
        temp=QObject::tr("RSU,Heart Beat");
        break;
    }
    return temp;
}

quint32 rsuctrl::GetOBUID()
{
    return nowRecivedOBUID;
}

void rsuctrl::onRsuDevStatusTimerOver()
{
    emit signalMessageToFrmMain(tr("RSU通信错误,30秒后重新初始化RSU"));
    emit signalRSUStateMessageToFrmMain(2);
    mTimerRSUInit.stop();
    disconnect(&mTimerRSUInit,SIGNAL(timeout()),0,0);
    mTimerRSUInit.start(1000*30);
    mTimerRSUInit.setSingleShot(true);
    connect(&mTimerRSUInit,SIGNAL(timeout()),this,SLOT(OnRSUInitTimerOver()));
    //保存设备状态
    SetDevStatus(RsuCommError);
}

void rsuctrl::OnRSUInitTimerOver()
{
    Init();
    disconnect(&mTimerRSUInit,SIGNAL(timeout()),0,0);
    qDebug()<<"daodaoliang"<<QDateTime::currentDateTime().toString();
}

void rsuctrl::ReadAndAnalzyIni(QString iniData)
{
    //默认的一共四位，分别是波特率，校验位，数据位，停止位
    QStringList tempList=iniData.split(',');
    SetBaterate(tempList.at(0).toInt());
    //其余几位对程序没有影响就不再设置
}

QByteArray rsuctrl::GetPsam1()
{
    return sPsam1;
}

QByteArray rsuctrl::GetPsam2()
{
    return sPsam2;
}

void rsuctrl::SetDevStatus(QRSUDevStatus status)
{
    if(FRsuDevStatus!=status)
        FRsuDevStatus=status;
    //正常上班的情况下才保存设备状态到报文中
    if(getLaneInfo()->getShiftState()!=lsNormalWorking)
        return;
    //保存设备状态
    if (RsuNoError==FRsuDevStatus)
        GetEtcLaneInstance()->Getetctollitem()->SetDevStatus(DT_RSU,DV_OK);
    else
        GetEtcLaneInstance()->Getetctollitem()->SetDevStatus(DT_RSU,DV_FAILURE);
}

bool rsuctrl::GetCanAnalyze()
{
    return m_canAnalyze;
}

void rsuctrl::SetCanAnalyze(bool flag)
{
    m_canAnalyze=flag;
}
