#ifndef RSUCTRL_H
#define RSUCTRL_H
/**********************************************/
/*文件说明:rsu的控制文件,控制天线并完成天线数据的解析**/
/*通过信号的方式通知外界,天线的当前工作阶段和工作数据***/
/**********************************************/
#include "rsutransframe.h"
#include <QTimer>
#include "rsuarmcomm.h"
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "CommonModule/ilogmsg.h"
#include "RSUArm/rsudatatype.h"

class rsuctrl :public DevBaseInterface
{
    Q_OBJECT
public:
    explicit rsuctrl(const QString& sName, QObject *parent = 0);

public:
    //设备接口用
    //加载驱动
    virtual bool LoadFunction();
    //卸载驱动
    virtual bool ReleaseFunction();
    //关闭
    virtual bool Close();
    //初始化
    virtual bool Init();

signals:
    //处理RSU属性的信号
    void signalWaitInit();
    //处理OBU属性的信号
    void signalWaitProcessOBUBaseInfo(QVariant,QVariant);
    //处理OBU信息的信号
    void signalWaitProcessOBUinfo(QVariant,QVariant);
    //处理卡信息的信号
    void signalWaitProcessCardInfo(QVariant,QVariant);
    //处理交易成功的信号
    void signalWaitProcessSuccInfo(QVariant,QVariant);
    //处理免费信息的信号
    void signalWaitProcessFreeInfo(QVariant,QVariant);
    //发送给主页面(日志)显示的信号
    void signalMessageToFrmMain(QString);
    //发送给主页面(天线设备状态)的信号
    void signalRSUStateMessageToFrmMain(int);
    //发送给费显显示的信号
    void signalMessageToDisplay(QString);
    //add by  dafang for test
    void signalRecivedFrameData(QByteArray FrameData);

private slots:
    //接收到帧数据后的处理槽
    void OnFrameDataReceived(QByteArray FrameData);
    //检测而天线设备的定时器所关联的槽
    void onRsuDevStatusTimerOver();
    //天线自动初始化的槽
    void OnRSUInitTimerOver();

public:
    //指令集的封装(供PC—>RSU用)
    //初始化RSU   C0指令
    bool InitRSUDev();
    //继续交易指令 C1指令
    bool ContinueDeal(quint32 OBUID);
    //停止交易指令 C2指令
    bool StopDeal(quint32 OBUID);
    //打开天线    4C指令
    bool OpenRSU();
    //关闭天线    4C指令
    bool CloseRSU();
    //写入口信息   C3指令
    bool WriteEntryInfo(quint32 OBUID,
                        const TCardTollInfo& EntryInfo,bool bOldCard);
    //写出口信息   C6指令
    bool WriteExitInfo(quint32 OBUID,
                       quint32 Money,const TCardTollInfo& ExitInfo,bool bOldCard);
    //写标识站信息 C4指令
    bool WriteFlagStationInfo(const QFrame_C4& FlagStation);
    //获取免费区间指令 C5指令
    bool GetFreeArea(quint32 OBUID);

public:
    //数据处理的封装(主要通过信号用来通知外界模块应该做何处理)
    //发出进行处理RSU的信息
    void TodoProcessRSU(QRSUState &Aobu,RSUERRORCODE errCode);
    //发出进行处理OBU的信息
    void TodoProcessOBU(QFrame_B2 &Aobu,RSUERRORCODE errCode);
    //发出处理OBU信息的信息
    void TodoProcessOBUInfo(QFrame_B3 &Aobu,RSUERRORCODE errCode);
    //发出处理卡信息的信息
    void TodoProcessCardInfo(QFrame_B4 &Aobu,RSUERRORCODE errCode);
    //发出处理交易成功信息的信息
    void TodoProcessSuccInfo(QFrame_B5 &Aobu,RSUERRORCODE errCode);
    //发出处理免费信息的信息
    void TodoProcessFreeInfo(QFrame_B6 &Aobu,RSUERRORCODE errCode);

public:
    //状态位以及内部数据的Get封装
    //串口号的Get操作
    quint16 GetComNumber();
    //波特率的Get操作
    quint16 GetBaterate();
    //初始化标记位的Get操作
    bool GetflagInitComm();
    //RSUInfo的Get操作
    QRSUState GetRsuInfo();
    //日志信息的Get操作
    QString GetLastMessageLog();
    //车道模式的Get操作
    quint8 GetLaneMode();
    //最小重读时间的Get操作
    quint8 GetWaitTime();
    //功率级数的Get操作
    quint8 GetPower();
    //帧的序列号的Get操作
    quint8 GetBeSendingRsctl();
    //帧的CRC校验值的Get操作
    bool GetBCCPassed();
    //帧类型的Get操作
    quint8 GetFrameType();
    //Get Error Desc
    QString GetErrorMessage(const quint16 errorCode);
    //Get OBUID
    quint32 GetOBUID();
    //获取PSAM1
    QByteArray GetPsam1();
    //获取PSAM2
    QByteArray GetPsam2();
    //获取是否允许解析数据
    bool GetCanAnalyze();

public:
    //串口操作和日志操作
    bool InitComm();
    //记录日志(文件)
    void LogMessageToFile(const QString &Message);
    //打印日志(deBug)
    void LogAndDebugToConsle(const QString &logMessage);

public:
    //状态位以及内部数据的Set封装
    //串口号的Set操作
    void SetComNumber(quint16 comNumber);
    //波特率的Set操作
    void SetBaterate(quint16 BateRate);
    //初始化标记位的Set操作
    void SetFlagInit(bool flagInit);
    //RSuInfo的Set操作
    void SetRSuInfo(QRSUState RSuInfo);
    //车道模式的Set操作
    void SetLaneMode(quint8 laneMode);
    //最小重读时间的Set操作
    void SetWaitTime(quint8 WaitTime);
    //功率级数的Set操作
    void SetPower(quint8 power);
    //帧的序列号的Set操作
    void SetBeSendingRsctl(quint8 Rsctl);
    //帧的CRC校验值的Set操作
    void SetBCCPassed(bool BCCPassed);
    //帧的类型的Set操作
    void SetFrameType(quint8 FrameType);
    //设置是否允许解析数据
    void SetCanAnalyze(bool flag);
private:
    //类内部数据的初始化
    void RsudataInit();
    //帧的完整性校验(异或值校验)
    bool FrameBCCPass(QByteArray data);
    //计算异或
    quint8 GetXOR(const QString &str);
    quint8 GetXOR(const char *Buff,int nLen);
    //配置文件读取并设置
    void ReadAndAnalzyIni(QString iniData);

private:
    //rsu接受数据相关的函数
    //打印接收到的报文
    void doDisplayETCMsg(const QString &message);
    //RSU设备状态检测时钟
    void OnRsuDevStatusTimer();
    //数据帧处理
    int FrameDataProcess(QByteArray &message);
    //数据帧解析并且发送外部信号
    int FrameDataAnalyze(QByteArray &FrameType);

private:
    //天线工作串口号
    quint16 mComNumber;
    //天线工作的波特率
    quint16 mbaterate;
    //设备端口是否初始化成功
    bool bInitSucc;
    //当前日志信息
    QString sMessage;
    //车道类型
    quint8 mLaneMode;
    //最小重读时间
    quint8 mWaitTime;
    //功率级数
    quint8 mPower;
    //Rsu状态检测时钟，在指定时间内收不到Rsu报文，
    //认定Rsu通讯中断
    QTimer mTimerRsuDevStatus;
    QTimer mTimerRSUInit;
    //通信组件
    RsuArmComm *ETCComm;
    //针转换对象
    rsutransframe TransFrame;
    //Psam卡号1
    QByteArray sPsam1;
    //Psam卡号2
    QByteArray sPsam2;
    //天线通信状态
    QRSUDevStatus FRsuDevStatus;
    RSUERRORCODE ret ;
public:
    //RSU设备信息
    QRSUState RSUInfo;
    const QRSUState * GetB0Info();
    //设备状态改变
    void SetDevStatus(QRSUDevStatus status);

private:
    //当前工作状态
    QRSUWorkStatus enumCurrentState;
    //当前要发送帧的序列号
    quint8 m_SendingRsctl;
    //当前接收到帧的序列号
    quint8 m_RecivedRsctl;
    //当前帧的CRC检验结果
    bool bIsBCCPassed;
    //当前帧类型
    quint8 mFrameType;
    //当前B0帧解析
    QRSUState sB0Info;
    //当前B2帧解析
    QFrame_B2 sB2Info;
    //当前B3帧解析
    QFrame_B3 sB3Info;
    //当前B4帧解析
    QFrame_B4 sB4Info;
    //当前B5帧解析
    QFrame_B5 sB5Info;
    //当前B6帧解析
    QFrame_B6 sB6Info;
    //当前OBUID
    quint32 nowRecivedOBUID;
    bool isFirstOpen;
    QVariant frameVar;
    QVariant transErrVar;
    //是否允许进行解析数据
    bool m_canAnalyze;
};
//获取程序的唯一实例
rsuctrl* GetRsuInstance();
#endif // RSUCTRL_H
