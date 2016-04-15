#include "wtdevunit.h"
#include <LogModule/qfilelogger.h>
#include <BaseInterface/devbaseinterface.h>
#include <QLibrary>
#include "CommonModule/laneutils_new.h"



const int WAIT_TIME = 500; //线程执行间隔时间
const int TEST_TIME = 4;   //检测设备状态的频率(几次WAIT_TIME后检测设备状态)

//设置串口，Ccomm为串口号，bps为波特率（约定为9600），返回值: 0 成功; -1 失败
typedef int (*Func_WimDev_SetCom)(char *comNo, int bps);
//参数：0   初始化并强制清空缓存； -1 初始化并返回缓存的车辆数，不清除缓存；
//      n   初始化并部分清除缓存，只保留最后n辆车的数据；
//返回：>=0 初始化成功，返回初始化完成后实际缓存的车辆数据量；-1 初始化失败。
typedef int (*Func_WimDev_Init)(int IniType);
//状态查询
typedef int (*Func_WimDev_Test)(char * bStatus);
//同步数据，返回值: 0 成功; -1 失败
typedef int (*Func_WimDev_Synchro)();
//删除首车，返回值: 0 成功; -1 失败
typedef int (*Func_WimDev_ClearOne)();
//删除指定车辆，返回值: 0 成功; -1 失败
typedef int (*Func_WimDev_ClearOneData)(int AIndex);
//取当前总车数 返回：>=0  车辆总数，0为没有车；-1  失败。
typedef int (*Func_WimDev_GetCount)();
// 取指定序号车辆的总轴重、总轴限、轴型、车速数据
typedef int (*Func_WimDev_Data)(int VehicleID,
                                quint32* Weight,
                                quint32* WtLimit,
                                char *AxisType,
                                double * speed);
// 取指定序号的车辆总轴组数
typedef int (*Func_WimDev_AxisCount)(int VehicleID);
// 取指定车辆指定轴组数据
typedef int (*Func_WimDev_AxisData)(int VehicleID,
                                    int AxisID,
                                    int * AxisType,
                                    char *TyreWeight);
// 取指定序号车辆的总轴数、各轴重、车速数据
typedef int (*Func_WimDev_AxisInfo)(int VehID,
                                    char * Axisweight,
                                    double *speed);

Func_WimDev_SetCom WimDev_SetCom=NULL;
Func_WimDev_Init WimDev_Init=NULL;
Func_WimDev_Test WimDev_Test=NULL;
Func_WimDev_Synchro WimDev_Synchro=NULL;
Func_WimDev_ClearOne WimDev_ClearOne=NULL;
Func_WimDev_ClearOneData WimDev_ClearOneData=NULL;
Func_WimDev_GetCount WimDev_GetCount=NULL;
Func_WimDev_Data WimDev_Data=NULL;
Func_WimDev_AxisCount WimDev_AxisCount=NULL;
Func_WimDev_AxisData WimDev_AxisData=NULL;
Func_WimDev_AxisInfo WimDev_AxisInfo=NULL;

TWtDev::TWtDev(const QString& sName,QObject *parent):DevBaseInterface(sName,parent)
{
    m_bTerm = false;
    m_FOldStatus = 0xff;
    m_FDiscountRate = 0;
    m_FDevVehCount = 0;
    m_FWaitCount = 0;
    m_FBackVehCount = 0;
    qRegisterMetaType<TWtVehicle*>("TWtVehicle");
}

void TWtDev::SynWeightData()
{
    QMutexLocker locker(&m_mutex);
    TWtVehicle *Veh = NULL;
    int ret = WimDev_GetCount();
    if (ret == -1)
    {
        LogMsg("lane", tr("[TWtDev.SynWeightData]WimDev_GetCount失败"));
        return ;
    }

    if (ret == m_FDevVehCount)
    {
        return;
    }
    //1.ret > FDevVehCount 新来了车辆
    //2.ret < FDevVehCount 倒车
    //注意:倒车与主动删车的区别,删车时先将FDevVehCount-1,应该不会出现ret < FDevVehCount的情况

    if (ret > m_FDevVehCount)
    {
        for (int i = m_FDevVehCount;i < ret;i++)
        {
            //取指定序号的车辆,放到队列
            //如果取车辆失败,还继续取下一辆吗?????
            Veh = GetVehFromDev(i);

            if (Veh != NULL && !Veh->isEmpty())
            {
                //先放在新车队列中,等离开临界区后在触发新车事件, 防止死锁
                m_newVehList.append(Veh);
                m_FDevVehCount = m_FDevVehCount + 1;
                LogMsg("lane",QObject::tr("取第%1辆成功").arg(i));
            }
        }

        if (m_FDevVehCount != ret)
        {
            LogMsg("lane",QObject::tr("GetCount的车辆数与实际取出的车辆不一致,可能有取数据失败的情况"));
        }
    }
    else
    {
        LogMsg("lane",QObject::tr("计重倒车了"));
        m_FBackVehCount = m_FDevVehCount - ret;
        m_FDevVehCount = ret;
    }
}
//加载动态库
bool TWtDev::LoadFunction()
{
    if (m_bDriverLoaded)
    {
        return true;
    }
    WimDev_SetCom = (Func_WimDev_SetCom)m_qLibrary.resolve("WimDev_SetCom");
    WimDev_Init =(Func_WimDev_Init)m_qLibrary.resolve("WimDev_Init");
    WimDev_Test =(Func_WimDev_Test)m_qLibrary.resolve("WimDev_Test");
    WimDev_Synchro =(Func_WimDev_Synchro)m_qLibrary.resolve("WimDev_Synchro");
    WimDev_ClearOne =(Func_WimDev_ClearOne)m_qLibrary.resolve("WimDev_ClearOne");
    WimDev_ClearOneData =(Func_WimDev_ClearOneData)m_qLibrary.resolve("WimDev_ClearOneData");
    WimDev_GetCount =(Func_WimDev_GetCount)m_qLibrary.resolve("WimDev_GetCount");
    WimDev_Data =(Func_WimDev_Data)m_qLibrary.resolve("WimDev_Data");
    WimDev_AxisCount =(Func_WimDev_AxisCount)m_qLibrary.resolve("WimDev_AxisCount");
    WimDev_AxisData =(Func_WimDev_AxisData)m_qLibrary.resolve("WimDev_AxisData");
    WimDev_AxisInfo =(Func_WimDev_AxisInfo)m_qLibrary.resolve("WimDev_AxisInfo");
    if((WimDev_SetCom == NULL) ||
            (WimDev_Init == NULL) ||
            (WimDev_Test == NULL) ||
            (WimDev_Synchro == NULL) ||
            (WimDev_ClearOne == NULL) ||
            (WimDev_ClearOneData == NULL) ||
            (WimDev_GetCount == NULL) ||
            (WimDev_Data == NULL) ||
            (WimDev_AxisCount == NULL) ||
            (WimDev_AxisData == NULL) ||
            (WimDev_AxisInfo == NULL))
    {
        LogMsg("lane",QObject::tr("[TWtDev.LoadLibs]加载计重dll时,至少有一个函数加载错误!"));
        return false;
    }
    return true;
}
//释放动态库
bool TWtDev::ReleaseFunction()
{
    m_bDriverLoaded = false;
    WimDev_SetCom=NULL;
    WimDev_Init=NULL;
    WimDev_Test=NULL;
    WimDev_Synchro=NULL;
    WimDev_ClearOne=NULL;
    WimDev_ClearOneData=NULL;
    WimDev_GetCount=NULL;
    WimDev_Data=NULL;
    WimDev_AxisCount=NULL;
    WimDev_AxisData=NULL;
    WimDev_AxisInfo=NULL;
    return true;
}

//检测设备状态
void TWtDev::TestDevStatus()
{
    char sta;
    int ret = WimDev_Test(&sta);
    if (ret == 0)
    {
        if (sta != m_FOldStatus)
        {
            m_FOldStatus = sta;
            DoDevStatusChange();
        }
    }
}
//指定车辆的计重信息
TWtVehicle* TWtDev::GetVehFromDev(int AIndex)
{
    int ret, ZhouZuShu, i, j;
    quint32 TyreWeight[4];
    quint32 AxisWeight[10];
    int AxisType;
    TAxis A;
    TWtVehicle *pV = NULL;
    double Speed = 0;
    quint32 ZhouZuWeight;
    AIndex++;
    //取轴组数
    //取各轴组轴型以及各轴重
    //取车速
    ret = WimDev_AxisCount(AIndex);
    if (ret == -1)
    {
        LogMsg("lane",QObject::tr("[TWtDev.GetVehFromDev]WimDev_AxisCount失败"));
        return NULL;
    }

    if (ret == 0)
    {
        LogMsg("lane",QObject::tr("[TWtDev.GetVehFromDev]WimDev_AxisCount返回0,车辆序号不存在"));
        return NULL;
    }
    pV = new TWtVehicle();
    pV->setFDataStatus(dsRaw);
    ZhouZuShu = ret;
    for(i = 1;i <= ZhouZuShu;i++)
    {
        memset(TyreWeight, 0, sizeof(TyreWeight));
        ret = WimDev_AxisData(AIndex, i, &AxisType, (char *)TyreWeight);
        if (ret == -1)
        {
            LogMsg("lane",QObject::tr("[TWtDev.GetVehFromDev]WimDev_AxisData调用失败"));
            //            V.Free; //只要取任何轴组出错,就返回nil
            pV->clear();
            delete pV;
            return NULL;
        }

        ZhouZuWeight = 0;
        for(j = 0; j < 4;j++)
        {
            if((TyreWeight[j]) > 0)
            {
                A.FWeight=round_int32(TyreWeight[j]*(1-m_FDiscountRate));
                pV->append(A);
                ZhouZuWeight = ZhouZuWeight + A.FWeight;
            }
        }

        if (i <= 12) //最多12个轴组信息
        {
            QString nStr = pV->getFAxisInfo() + tr("%1").arg(ZhouZuWeight, 6, 10, QLatin1Char('0'));
            pV->setFAxisInfo(nStr);
        }
    }

    //取车速
    Speed = 0;
    memset(AxisWeight, 0, sizeof(AxisWeight));
    ret = WimDev_AxisInfo(AIndex, (char *)AxisWeight, &Speed);
    if (ret ==0)
    {
        pV->setSpeed(Speed);
    }
    else
    {
        LogMsg("lane",QObject::tr("[TWtDev.GetVehFromDev]WimDev_AxisInfo调用失败"));
    }
    return pV;
}
//处理设备状态变化
void TWtDev::DoDevStatusChange()
{
    emit StatusChanged((int)m_FOldStatus);
}
//删除第一辆车
bool TWtDev::DeleteFirstVehicle()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    bool Result = true;
    int VehCount = WimDev_GetCount();
    if (VehCount > 0)
    {
        Result = (WimDev_ClearOne() == 0);
    }
    //if Result and (FDevVehCount > 0) then
    if (Result && m_FDevVehCount > 0)
    {
        m_FDevVehCount = m_FDevVehCount - 1;
    }
    return Result;
}


//处理新车数据
void TWtDev::DoNewVeh()
{
    //qDebug() << "出现新车计重";
    emit newVehEvent(m_pCurrVeh);
}
//处理倒车数据
void TWtDev::DoBackVeh()
{
    //qDebug() << "出现倒车";
    for (int i= 1; i<= m_FBackVehCount;i++)
    {
        emit backVehEvent();
    }
}

void TWtDev::run()
{
    LogMsg("lane",QObject::tr("计重线程进入!"));
    DoDevStatusChange(); //先通知一下设备状态
    while (!m_bTerm)
    {
        msleep(WAIT_TIME);

        if(m_bTerm)
        {
            break;
        }
        //1.设备状态       状态粒度和数据粒度不一样 是数据粒度的倍数
        m_FWaitCount = m_FWaitCount + 1;
        if (m_FWaitCount >= TEST_TIME)
        {
            TestDevStatus();
            m_FWaitCount = 0;
        }
        //同步计重数据
        SynWeightData();
        //通知上层
        NotifyMainThread();
    }
    LogMsg("lane",QObject::tr("计重线程退出！"));
}
//删除指定车辆
bool TWtDev::DeleteVehicle(int AIndex)
{
    bool Result;
    Result = true;
    QMutexLocker locker(&m_mutex);
    int VehCount = WimDev_GetCount();
    if (VehCount > 0)
    {
        Result = (WimDev_ClearOneData(AIndex) == 0);
        if (m_FDevVehCount > 0)
        {
            m_FDevVehCount = m_FDevVehCount - 1;
        }
    }
    return Result;
}
//通知上层有车辆计重数据发生变化
void TWtDev::NotifyMainThread()
{
    //通知新车
    int tmpCount = m_newVehList.count();
    for(int i= 0;i<tmpCount;i++)
    {
        m_pCurrVeh = m_newVehList.first();
        m_newVehList.removeFirst();
        DoNewVeh();
    }

    //通知倒车
    if (m_FBackVehCount > 0)
    {
        DoBackVeh();
        m_FBackVehCount = 0;
    }
}

bool TWtDev::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    int nRlt = WimDev_SetCom(m_sConStr1.toLocal8Bit().data(), m_sConStr2.toInt());
    if(0 != nRlt)
    {
        LogMsg("lane", tr("打开计重设备失败，Error Code=%1").arg(nRlt));
        emit StatusChanged(DEV_STATUS_ParamErr);
        return false;
    }
    LogMsg("lane", tr("打开%1成功").arg(objectName()));
    nRlt = WimDev_Synchro();
    if(0 != nRlt)
    {
        LogMsg("lane", tr("程序启动时进行计重设备同步失败，Error Code=%1").arg(nRlt));
        return false;
    }
    nRlt = WimDev_Init(6);
    if(nRlt < 0)
    {
        LogMsg("lane", tr("程序启动时调用WimDev_Init失败，Error Code=%1").arg(nRlt));
        return false;
    }
    //改变状态
    emit StatusChanged(DEV_STATUS_OK);
    //线程启动
    this->start();
    return true;
}

bool TWtDev::Close()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    //线程中断
    m_bTerm = true;
    this->wait();
    return true;
}
























