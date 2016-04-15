#include "wtdevunit.h"
#include <QLibrary>
const int  WAIT_TIME = 500;      //线程执行间隔时间
const int  TEST_TIME = 4;        //检测设备状态的频率(几次WAIT_TIME后检测设备状态)
// THandle DLLHandle ;
//TCriticalSection  CallLock;

//设置串口，Ccomm为串口号，bps为波特率（约定为9600），返回值: 0 成功; -1 失败
//int WimDev_SetCom(char comNo, int bps);
typedef int (*WimDev_SetCom)(const char *szComNo, int nBps);
//参数：0   初始化并强制清空缓存； -1 初始化并返回缓存的车辆数，不清除缓存；
//      n   初始化并部分清除缓存，只保留最后n辆车的数据；
//返回：>=0 初始化成功，返回初始化完成后实际缓存的车辆数据量；-1 初始化失败。
typedef int  (*WimDev_Init)(int IniType);
//状态查询
typedef int (*WimDev_Test)(char *bStatus);
//同步数据，返回值: 0 成功; -1 失败
typedef int  (*WimDev_Synchro)();
//删除首车，返回值: 0 成功; -1 失败
typedef int (*WimDev_ClearOne)();
//删除指定车辆，返回值: 0 成功; -1 失败
typedef int  (*WimDev_ClearOneData)(int AIndex);
//取当前总车数 返回：>=0  车辆总数，0为没有车；-1  失败。
typedef int  (*WimDev_GetCount)();
// 取指定序号车辆的总轴重、总轴限、轴型、车速数据
typedef int (*WimDev_Data)(int VehicleID,
                           long *Weight,long *WtLimit,int AxisType[10],
                           double & speed);
// 取指定序号的车辆总轴组数
typedef int (*WimDev_AxisCount)(int VehicleID);
// 取指定车辆指定轴组数据
typedef int (*WimDev_AxisData)(int VehicleID,
                               int AxisID,
                               int *AxisType, long TyreWeight[4]);
// 取指定序号车辆的总轴数、各轴重、车速数据
typedef int (*WimDev_AxisInfo)(int VehID,
                               long AxisWeight[10],
                               double speed);
WimDev_SetCom Dev_SetCom;
WimDev_Init Dev_Init;
WimDev_Test Dev_Test;
WimDev_Synchro Dev_Synchro;
WimDev_ClearOne Dev_ClearOne;
WimDev_ClearOneData Dev_ClearOneData;
WimDev_GetCount Dev_GetCount;
WimDev_Data Dev_Data;
WimDev_AxisCount Dev_AxisCount;
WimDev_AxisData Dev_AxisData ;
WimDev_AxisInfo Dev_AxisInfo ;

TWtDev::TWtDev(QObject *parent) :
    QPausableThread(parent)
{
    m_bLoadflag = false;
}

TWtDev::~TWtDev()
{
    qDebug() << "release wt dev thread...";
}

bool TWtDev::LoadFunction()
{
    if( !m_bLoadflag)
    {
        return true;
    }
    Dev_SetCom=(WimDev_SetCom)m_qLibrary.resolve("WimDev_SetCom");
    Dev_Init=(WimDev_Init)m_qLibrary.resolve("WimDev_Init");
    Dev_Test=(WimDev_Test)m_qLibrary.resolve("WimDev_Test");
    Dev_Synchro=(WimDev_Synchro)m_qLibrary.resolve("WimDev_Synchro");
    Dev_ClearOne=(WimDev_ClearOne)m_qLibrary.resolve("WimDev_ClearOne");
    Dev_ClearOneData=(WimDev_ClearOneData)m_qLibrary.resolve("WimDev_ClearOneData");
    Dev_GetCount=(WimDev_GetCount)m_qLibrary.resolve("WimDev_GetCount");
    Dev_Data=(WimDev_Data)m_qLibrary.resolve("WimDev_Data");
    Dev_AxisCount=(WimDev_AxisCount)m_qLibrary.resolve("WimDev_AxisCount");
    Dev_AxisData=(WimDev_AxisData)m_qLibrary.resolve("WimDev_AxisData");
    Dev_AxisInfo=(WimDev_AxisInfo)m_qLibrary.resolve("WimDev_AxisInfo");
    if(NULL==Dev_SetCom||NULL==Dev_Init||NULL==Dev_Test||NULL==Dev_Synchro||NULL==Dev_ClearOne||NULL==Dev_ClearOneData||NULL==Dev_GetCount||NULL==Dev_Data||NULL==Dev_AxisData||NULL==Dev_AxisCount||NULL==Dev_AxisInfo)
    {
        LogMsg("lane",tr("计重函数错误！"));
        return false;
    }
    m_bLoadflag=true;
    return true;
}

bool TWtDev::ReleaseFunction()
{
    m_bLoadflag=false;
    Dev_SetCom=NULL;
    Dev_Init=NULL;
    Dev_Test=NULL;
    Dev_Synchro=NULL;
    Dev_ClearOne=NULL;
    Dev_ClearOneData=NULL;
    Dev_GetCount=NULL;
    Dev_Data=NULL;
    Dev_AxisData=NULL;
    Dev_AxisCount=NULL;
    Dev_AxisInfo=NULL;
    return true;
}
bool TWtDev::Init(quint8 nPort, int bps)
{
    if (!m_bLoadflag)
    {
        LogMsg("lane",tr("[TWtDev.Connect]加载DLL失败"));
        return false;
    }
    QString p=QObject::tr("Com%1").arg(nPort);
    int ret=Dev_SetCom(p.toLocal8Bit().data(), bps);;

    if (ret==0)
    {
        Resume();
        return true;
    }
    else
    {
        LogMsg("lane",tr("[TWtDev.OpenDev]WimDev_SetCom失败"));
        return false;
    }
}

bool TWtDev::Close()
{
    //Terminate;
    if ( m_bLoadflag)
    {
        Resume();
    }

    this->Stop();

    return ReleaseFunction();

}

void TWtDev::SynWeightData()
{

    int ret, i;
    TWtVehicle Veh;
    ret=Dev_GetCount();
    if(ret == -1)
    {
        LogMsg("lane",tr("[TWtDev.SynWeightData]WimDev_GetCount失败"));
        return;
    }

    if (ret == FDevVehCount)
    {
        return;
    }

    //1.ret > FDevVehCount 新来了车辆
    //2.ret < FDevVehCount 倒车
    //注意:倒车与主动删车的区别,删车时先将FDevVehCount-1,应该不会出现ret < FDevVehCount的情况

    if(ret > FDevVehCount)
    {
        for( i= FDevVehCount + 1;i <= ret;i++)
        {
            //取指定序号的车辆,放到队列
            //如果取车辆失败,还继续取下一辆吗?????
            Veh= GetVehFromDev(i);

            if (!Veh.isEmpty())
            {
                //先放在新车队列中,等离开临界区后在触发新车事件, 防止死锁
                //FNewVehList.Add(Veh);
                FNewVehList.append(Veh);
                FDevVehCount= FDevVehCount + 1;
                LogMsg("lane",tr("取第%d辆成功', [i]"));
            }
        }

        if (FDevVehCount != ret)
        {
            LogMsg("lane",tr("GetCount的车辆数与实际取出的车辆不一致,可能有取数据失败的情况"));

        }
    }
    else
    {
        LogMsg("lane",tr("计重倒车了"));
        FBackVehCount = FDevVehCount - ret;
        //DoBackVeh(); //放到NotifyMainThread中去处理,防止死锁
        FDevVehCount= ret;
    }

}

void TWtDev::TestDevStatus()
{
    char sta;
    int  ret;
    ret= Dev_Test(&sta);
    if (ret==0 )
    {
        if (sta!= FOldStatus)
        {
            FOldStatus = sta;
            m_mutex.lock();
            DoDevStatusChange();
            m_mutex.unlock();
        }
    }
}

TWtVehicle TWtDev::GetVehFromDev(int AIndex)
{
    int ret, ZhouZuShu;
    long TyreWeight[4];
    long AxisWeight[10];
    TAxis A;
    TWtVehicle V;
    double  Speed;
    quint32 ZhouZuWeight;
    int AxisType;
    //取轴组数
    //取各轴组轴型以及各轴重
    //取车速
    // Result= nil;

    ret = Dev_AxisCount(AIndex);
    if(ret == -1)
    {
        V.Clear();
        LogMsg("lane",tr("[TWtDev.GetVehFromDev]WimDev_AxisCount失败"));
        return V;
    }
    if (ret==0)
    {
        V.Clear();
        LogMsg("lane",tr("[TWtDev.GetVehFromDev]WimDev_AxisCount返回0,车辆序号不存在"));
        return V;
    }

    //V = TWtVehicle;
    ZhouZuShu= ret;
    for(int i= 1; i<= ZhouZuShu; i++)
    {
        qMemSet(TyreWeight, 0, sizeof(TyreWeight));
        ret =Dev_AxisData(AIndex, i, &AxisType, TyreWeight);
        if( ret==-1)
        {
            LogMsg("lane",tr("[TWtDev.GetVehFromDev]WimDev_AxisData调用失败"));
            V.clear(); //只要取任何轴组出错,就返回nil
            return V;
        }

        ZhouZuWeight = 0;
        for( int j= 0;j<sizeof(TyreWeight);j++)
        {
            if( TyreWeight[j] > 0)
            {
                A.setWeight((quint32)qRound(TyreWeight[j] * (1 -FDiscountRate)));
                V.append(A);
                ZhouZuWeight = ZhouZuWeight + A.getFWeight();
            }
        }

        if (i <= 12 ) //最多12个轴组信息
        {
            QString info= V.GetFAxisInfo() + QObject::tr("%1").arg(AxisType) +QObject::tr("%1").arg(ZhouZuWeight,6,10,QLatin1Char('0'));
            V.setFAxisInfo(info);
        }
    }

    //取车速
    Speed = 0;
    qMemSet(AxisWeight,0, sizeof(AxisWeight));
    ret = Dev_AxisInfo(AIndex, AxisWeight, Speed);
    if( ret==0)
    {
        V.setFSpeed(Speed);
    }
    else
    {
        V.Clear();
        LogMsg("lane",tr("[TWtDev.GetVehFromDev]WimDev_AxisInfo调用失败"));
    }
    return V;
}
void TWtDev::DoDevStatusChange()
{
    //            if( FOnDevStatusChange.is)
    //                    {
    //                   FOnDevStatusChange((quint8)FOldStatus);
    //                  }
}

bool TWtDev::DeleteFirstVehicle()
{
    bool rlt = false;
    m_mutex.lock();
    int VehCount=Dev_GetCount();
    if( VehCount > 0)
    {
        rlt = Dev_ClearOne() == 0;
        if (FDevVehCount > 0)
        {
            FDevVehCount= FDevVehCount - 1;
        }
    }
    m_mutex.unlock();
    return rlt;
}

void TWtDev::DoNewVeh()
{
    //            if(FOnNewVehData)
    //              {
    //             FCurrVeh.clear();
    //               return;

    //            }

    // FOnNewVehData(FCurrVeh); //注意:Veh由事件接收方释放
}

void TWtDev::DoBackVeh()
{/*
    if(FOnBackVeh)
     {
       for (int i= 1;i< FBackVehCount;i++)
     {
         FOnBackVeh();
      }
      }*/
}

bool TWtDev::DeleteVehicle(int AIndex)
{
    bool ret;
    int VehCount;
    m_mutex.lock();
    VehCount=Dev_GetCount();

    if (VehCount > 0)
    {
        ret = (Dev_ClearOneData(AIndex)== 0);
        if (FDevVehCount > 0)
        {
            FDevVehCount= FDevVehCount - 1;
        }
        m_mutex.unlock();
    }
    return true;
}

void TWtDev::NotifyMainThread()
{
    int j;

    //通知新车
    j = FNewVehList.count();
    for (int i= 0 ;0< j - 1;i++)
    {
        // FCurrVeh= FNewVehList.Extract(FNewVehList.First) as TWtVehicle;

        FCurrVeh = FNewVehList.first();
        DoNewVeh();
        FCurrVeh.clear();
    }

    //通知倒车
    if (FBackVehCount > 0)
    {
        DoBackVeh();
        FBackVehCount= 0;
    }
}

QString TWtDev::GetFDllFileName()
{
    return FDllFileName;
}

void TWtDev::setFDllFileName(QString value)
{
    FDllFileName=value;
}
void TWtDev::RunOnce()
{

    //1.设备状态       状态粒度和数据粒度不一样 是数据粒度的倍数
    FWaitCount= FWaitCount + 1;
    if (FWaitCount >= TEST_TIME)
    {
        TestDevStatus();
        FWaitCount= 0;
    }

    //同步计重数据
    SynWeightData();
}

float TWtDev::GetFDiscountRate()
{
    return 0;
}

void TWtDev::setFDiscountRate(float countRate)
{
}

void TWtDev::setLibName(QString name)
{
    m_qLibrary.fileName()= name;
}

