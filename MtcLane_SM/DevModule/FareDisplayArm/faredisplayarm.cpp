#include "faredisplayarm.h"
#include<QDateTime>
#include<QLibrary>
// 初始化
typedef int ( *Func_FD_Open)(char *lpszConnPara);
// 设置费显工作模式
typedef int ( *Func_FD_SetMode)(int nLaneType, bool bTollByWeight);
// 关闭设备，释放占用的资源
typedef int ( *Func_FD_Close)();
// 显示车型：nVehClass小于0表示清空车型信息（若超出费额显示器显示范围，应清空，不应显示错误值）
typedef int ( *Func_FD_VehClass)(int nVehClass);
// 显示车货总重（kg）、限载（kg）、超限率（千分数）
typedef int ( *Func_FD_Weight)(int nTotalWeight, int nWeightLimit, int nOverweightRate);
// 显示应收金额：nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
typedef int ( *Func_FD_Fare)(int nFare);
// 显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
typedef int ( *Func_FD_BalanceBeforeDeduct)(int nBalanceBeforeDeduct);
// 显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
typedef int ( *Func_FD_BalanceAfterDeduct)(int nFarePaid, int nBalanceAfterDeduct);
// 显示入口站
typedef int ( *Func_FD_EntrySta)(char *szEntrySta);
// 显示文字 - 若费额显示器支持编号短语，需在动态连接库内部进行优化
typedef int ( *Func_FD_Text)(char *lpszText, int nColor);
// 同步信息 - 完成实际数据发送
typedef int ( *Func_FD_SynData)();
// 清空所有信息
typedef int ( *Func_FD_ClearAll)();
// 设置亮度
typedef int ( *Func_FD_Brightness)(int nBrightness);
// 黄闪报警灯控制
typedef int ( *Func_FD_AlarmLight)(bool bStatus);
// 报警声音控制
typedef int ( *Func_FD_AlarmSound)(bool bStatus);
// 通行信号灯控制
typedef int ( *Func_FD_PassLight)(bool bIsGreen);

//BOOL CFareDisplayer::m_bDriverLoaded=false;
//HINSTANCE FareDisplayArm::m_hLibModule=NULL;

Func_FD_Open FD_Open=NULL;
Func_FD_SetMode FD_SetMode=NULL;
Func_FD_Close FD_Close=NULL;
Func_FD_VehClass FD_VehClass=NULL;
Func_FD_Weight FD_Weight=NULL;
Func_FD_Fare FD_Fare=NULL;
Func_FD_BalanceBeforeDeduct FD_BalanceBeforeDeduct=NULL;
Func_FD_BalanceAfterDeduct FD_BalanceAfterDeduct=NULL;
Func_FD_EntrySta FD_EntrySta=NULL;
Func_FD_Text FD_Text=NULL;
Func_FD_SynData FD_SynData=NULL;
Func_FD_ClearAll FD_ClearAll=NULL;
Func_FD_Brightness FD_Brightness=NULL;
Func_FD_AlarmLight FD_AlarmLight=NULL;
Func_FD_AlarmSound FD_AlarmSound=NULL;
Func_FD_PassLight FD_PassLight=NULL;


FareDisplayArm::FareDisplayArm(const QString& sName, QObject *parent) :
    DevBaseInterface(sName,parent)
{
    m_bNewVehStarted=false;
    m_timerId = 0;
}
FareDisplayArm * getFareDisplay()
{
    static FareDisplayArm g_devFareDisplay("FareDisplay");
    return & g_devFareDisplay;
}
void FareDisplayArm::NotifyNewVeh()
{
    if (m_bNewVehStarted)
    {
        // 不允许重复调用：如显示车型后，前线圈再检测到车辆到来，不认为有新业务需要处理，直到业务结束
        return;
    }
    m_bNewVehStarted=true;
    if (!m_bDriverLoaded)
    {
        return;
    }
    FD_VehClass(-1);
    FD_Weight(-1, -1, -1);
    FD_Fare(-1);
    FD_BalanceBeforeDeduct(-1);
    FD_BalanceAfterDeduct(-1, -1);
    FD_EntrySta("");
    FD_Text("", 0);
}
//清空所有
bool FareDisplayArm::ClearAll()
{

    if (!m_bDriverLoaded)
    {
        return false;
    }
    bool bet =  FD_ClearAll();

    return 0==bet;
}

//通知费额显示器车辆已经离开
void FareDisplayArm::NotifyVehPassed()
{
    if (!m_bNewVehStarted)
    {
        ClearAll();
    }
}

bool FareDisplayArm::SetMode(int nLaneType, bool bTollByWeight)
{
    LogMsg("lane", tr("费显设置工作模式为：LaneType[%1],TollByWeight[%2]").arg(nLaneType).arg(bTollByWeight));
    m_nLaneType=nLaneType;
    m_bTollByWeight=bTollByWeight;
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_SetMode(nLaneType, bTollByWeight))
    {
        return false;
    }
    return true;
}

bool FareDisplayArm::ShowVehClass(int nVehClass, bool bIsNewVeh, bool bSynData)
{
    LogMsg("lane", tr("费显显示车型，VehClass[%1],IsNewVeh[%2],SynData[%3]").arg(nVehClass).arg(bIsNewVeh).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (nVehClass>0 && bIsNewVeh)
    {
        // NOTE: 输入车型是一辆车处理的起点，此时清空其它信息
        NotifyNewVeh();
    }
    if (0!=FD_VehClass(nVehClass))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::ShowWeightInfo(int nTotalWeight, int nWeightLimit, int nOverWeightRate, bool bSynData)
{
    LogMsg("lane", tr("费显显示计重信息，nTotalWeight[%1],nWeightLimit[%2],nOverWeightRate[%3],bSynData[%4]")
           .arg(nTotalWeight).arg(nWeightLimit).arg(nOverWeightRate).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_Weight(nTotalWeight, nWeightLimit, nOverWeightRate))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::ShowFare(int nFare, bool bSynData)
{
    LogMsg("lane", tr("费显显示收费金额,Fare[%1]，SynData[%2]").arg(nFare).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_Fare(nFare))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::ShowBalanceBeforeDeduct(int nBalanceBeforeDeduct, bool /*bIsTallyCard*/, bool bSynData)
{
    LogMsg("lane", tr("费显显示扣款前余额，nBalanceBeforeDeduct[%1],bSynData[%2]").arg(nBalanceBeforeDeduct).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_BalanceBeforeDeduct(nBalanceBeforeDeduct))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::ShowBalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool /*bIsTallyCard*/, bool bSynData)
{
    LogMsg("lane", tr("费显显示扣款后余额，nFarePaid[%1],nBalanceAfterDeduct[%2],bSynData[%3]")
           .arg(nFarePaid/100.0).arg(nBalanceAfterDeduct/100.0).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_BalanceAfterDeduct(nFarePaid, nBalanceAfterDeduct))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }

    return true;
}
//显示入口站
bool FareDisplayArm::ShowEntrySta(char *szEntrySta, bool bSynData)
{
    LogMsg("lane", tr("费显显示入口站,szEntrySta[%1],bSynData[%2]").arg(szEntrySta).arg(bSynData));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    if (0!=FD_EntrySta(szEntrySta))
    {
        return false;
    }
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::ShowText(char *szText, SHORT_MSG_TYPE nMsgType,bool bSynData)
{
    LogMsg("lane",tr("费显显示文字，Text[%1],nMsgType[%2],bSynData[%3]")
           .arg(szText).arg(nMsgType).arg(bSynData));    
    if (!m_bDriverLoaded)
    {
        return false;
    }
    int nColor=SM_Yellow;
    switch (nMsgType)
    {
    case SM_Welcome:
        NotifyNewVeh();
        break;
    case SM_Error:
        nColor=SM_Red;
        break;
    case SM_Prompt:
        break;
    case SM_Goodbye:
        m_bNewVehStarted=false;
        break;
    case SM_NoVeh:
        NotifyVehPassed();
        break;
    default:
        break;
    }
    //modify-----by xujingliang
    FD_Text(szText,1);
    if (bSynData)
    {
        return SynData();
    }
    return true;
}

bool FareDisplayArm::SynData()
{
    LogMsg("lane",tr("费显同步信息到设备"));
    if (!m_bDriverLoaded)
    {
        return false;
    }
    return 0==FD_SynData();

}

bool FareDisplayArm::Alarm(bool bAlarm, int nAutoStopTime)
{
    if (!m_bDriverLoaded)
    {
        return false;
    }
    FD_AlarmLight(bAlarm);
    FD_AlarmSound(bAlarm);
    if (bAlarm && nAutoStopTime>0)
    {
        if(m_timerId != 0)
        {
            killTimer(m_timerId);
        }
        m_timerId = startTimer(nAutoStopTime);
    }
    return true;
}

bool FareDisplayArm::AlarmSound(bool bAlarm, int nAutoStopTime)
{
    if (!m_bDriverLoaded)
    {
        return false;
    }
    int nRlt=FD_AlarmSound(bAlarm);
    if (bAlarm && nAutoStopTime>0)
    {
        if(m_timerId != 0)
        {
            killTimer(m_timerId);
        }
        m_timerId = startTimer(nAutoStopTime);
    }
    return 0==nRlt;
}

bool FareDisplayArm::AlarmLight(bool bAlarm, int nAutoStopTime)
{
    if (!m_bDriverLoaded)
    {
        return false;
    }
    int nRlt=FD_AlarmLight(bAlarm);
    if (bAlarm && nAutoStopTime>0)
    {
        if(m_timerId != 0)
        {
            killTimer(m_timerId);
        }
        m_timerId = startTimer(nAutoStopTime);
    }
    return 0==nRlt;

}

bool FareDisplayArm::SetBrightness(int nBrightness)
{
    if (!m_bDriverLoaded)
    {
        return false;
    }
    return 0==FD_Brightness(nBrightness);

}

bool FareDisplayArm::SetPassLight(bool bGreen)
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    return 0==FD_PassLight(bGreen);
    return true;
}

bool FareDisplayArm::LoadFunction()
{
    qDebug() << "begin load fd function...";
    if(m_bDriverLoaded)
    {
        qDebug() << "driver has loaded";
        return true;
    }
    qDebug() << "load fd open";
    FD_Open=(Func_FD_Open)m_qLibrary.resolve("FD_OpenDev");
    qDebug() << "load fd set mode";
    FD_SetMode=(Func_FD_SetMode)m_qLibrary.resolve("FD_SetMode");
    qDebug() << "load fd close";
    FD_Close=(Func_FD_Close)m_qLibrary.resolve("FD_CloseDev");
    qDebug() << "load fd veh class";
    FD_VehClass=(Func_FD_VehClass)m_qLibrary.resolve("FD_VehClass");
    qDebug() << "load fd weight";
    FD_Weight=(Func_FD_Weight)m_qLibrary.resolve("FD_Weight");
    qDebug() << "load fd fare";
    FD_Fare=(Func_FD_Fare)m_qLibrary.resolve("FD_Fare");
    qDebug() << "load fd fare";

    FD_BalanceBeforeDeduct=(Func_FD_BalanceBeforeDeduct)m_qLibrary.resolve("FD_BalanceBeforeDeduct");    qDebug() << "load fd fare";
    qDebug() << "load fd fare";

    FD_BalanceAfterDeduct=(Func_FD_BalanceAfterDeduct)m_qLibrary.resolve("FD_BalanceAfterDeduct");
    qDebug() << "load fd fare";
    FD_EntrySta=(Func_FD_EntrySta)m_qLibrary.resolve("FD_EntrySta");
    qDebug() << "load fd fare";
    FD_Text=(Func_FD_Text)m_qLibrary.resolve("FD_Text");
    qDebug() << "load fd fare";
    FD_SynData=(Func_FD_SynData)m_qLibrary.resolve("FD_SynData");
    qDebug() << "load fd fare";
    FD_ClearAll=(Func_FD_ClearAll)m_qLibrary.resolve("FD_ClearAll");
    qDebug() << "load fd fare";
    FD_Brightness=(Func_FD_Brightness)m_qLibrary.resolve("FD_Brightness");
    qDebug() << "load fd fare";
    FD_AlarmLight=(Func_FD_AlarmLight)m_qLibrary.resolve("FD_AlarmLight");
    qDebug() << "load fd fare";
    FD_AlarmSound=(Func_FD_AlarmSound)m_qLibrary.resolve("FD_AlarmSound");
    qDebug() << "load fd fare";
    FD_PassLight=(Func_FD_PassLight)m_qLibrary.resolve("FD_PassLight");
    if (NULL==FD_Open ||
            NULL==FD_SetMode ||
            NULL==FD_Close ||
            NULL==FD_VehClass ||
            NULL==FD_Weight ||
            NULL==FD_Fare ||
            NULL==FD_BalanceBeforeDeduct ||
            NULL==FD_BalanceAfterDeduct ||
            NULL==FD_EntrySta ||
            NULL==FD_Text ||
            NULL==FD_SynData ||
            NULL==FD_ClearAll ||
            NULL==FD_Brightness ||
            NULL==FD_AlarmLight ||
            NULL==FD_AlarmSound ||
            NULL==FD_PassLight)
    {
        qDebug() << "load fd driver fail";
        LogMsg("lane",tr("获取费显动态库[%1]中的一个或多个函数错误！").arg(m_sLibName));
        return false;
    }
    qDebug() << "load fd driver success";
    m_bDriverLoaded=TRUE;
    return true;
}

bool FareDisplayArm::ReleaseFunction()
{
    // 标记动态库未加载
    m_bDriverLoaded=false;
    // 函数指针置为NULL
    FD_Open=NULL;
    FD_SetMode=NULL;
    FD_Close=NULL;
    FD_VehClass=NULL;
    FD_Weight=NULL;
    FD_Fare=NULL;
    FD_BalanceBeforeDeduct=NULL;
    FD_BalanceAfterDeduct=NULL;
    FD_EntrySta=NULL;
    FD_Text=NULL;
    FD_SynData=NULL;
    FD_ClearAll=NULL;
    FD_Brightness=NULL;
    FD_AlarmLight=NULL;
    FD_AlarmSound=NULL;
    FD_PassLight=NULL;
    return true;
}

bool FareDisplayArm::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    //Close();
    // NOTE: 费显通讯方式可能是网络或串口
    qDebug() << "fd connstr1" << m_sConStr1 << "fd open";
    if(0!=FD_Open(m_sConStr1.toLocal8Bit().data()))
    {
        LogMsg("lane",tr("费显初始化，打开失败,ConnStr1[%1]").arg(m_sConStr1));
        //ChangeStatus(DEV_STATUS_CommErr);
        return false;
    }
    LogMsg("lane",tr("费显初始化，打开成功,ConnStr1[%1]").arg(m_sConStr1));
    // 打开费显后立即清空以前的信息
    qDebug() << "fd clear all";
    ClearAll();
    return true;
}

bool FareDisplayArm::Close()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    FD_Close();
    return true;
}

void FareDisplayArm::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerId)
    {
        Alarm(false);
        m_timerId = 0;
    }
}








