#include "wtthread.h"
#include <QMutexLocker>
#include <QDebug>
#include <QMutexLocker>
QMutex g_mutex;
char tmpBuf[2049];
typedef struct _CARINF
{
    char valid;
    char GMflag;
    int AxleNum;
    long AxleWt[50];
    int AxleGroupNum;
    long AxleGroupWt[50];
    int AxleGroupType[50];
    int  Speed;
} CARINF, *pCARINF;
CARINF  car[10];
QTcpClient* getTcpClient()
{
    static QTcpClient client;
    return &client;
}
WtThread* getWt()
{
    static WtThread wt;
    return &wt;
}

int TypeNum[10]={0,1,1,2,2,2,3,3,3,3};//分别是0123456789型轴型轴组对应的轴数
int  RcdCarNum=0;
unsigned char GameOverFlag=0;

#define NEWDATA        1
#define BACKWARD       2
#define DUOCHE         3
#define CHUFA          4

#define  Ver   "1.3.3"
quint32 SfMsgID;

quint32 dwError;
quint32 TestCnt=0;   //0表示通讯正常
char Comm_Serial=100;
char Status=0;
char GetData_flag=0;


long WeightLimit(int VehicleID)
{
    int type;

    type=car[VehicleID-1].AxleGroupType[0]*1000+car[VehicleID-1].AxleGroupType[1]*100+car[VehicleID-1].AxleGroupType[2]*10+car[VehicleID-1].AxleGroupType[3];
    switch (type)
    {
    case 1100:
        return 14000;
        break;
    case 1200:
        return 17000;
        break;
    case 1120:
        return 24000;
        break;

    case 1500:
        return 25000;
        break;
    case 1150:
        return 32000;
        break;
    case 1250:
        return 35000;
        break;

    case 1290:
        return 41000;
        break;

    case 1550:
        return 43000;
        break;

    case 1590:
        return 49000;
        break;

    case 1129:      //1129
        return 48000;
        break;
    default :
        return 0;
        break;
    }
}
WtThread::WtThread(QObject *parent) :
    QThread(parent)
{
}

WtThread::~WtThread()
{
}
/*数据发送时按照一定的规则，由于数据大部分为int或long型，可以加'>'或'<'等将各个数据分割
  在按照这个规矩解析。数据依次是：
1.carinfo.valid：//直接当作判断是单轴或轴组，1为轴组，2为单轴
2.carinfo.GMflag://当作倒车的标志位，若为倒车，则设置为1；
3.carinfo.Speed
4.若为单轴则为carinfo.AxleNum，接着是carinfo.AxleWt[i]；
    若为轴组则为carinfo.AxleGroupNum，，carinfo.AxleGroupWt[i]，，carinfo.AxleGroupType[i]
*/
void WtThread::run()
{
}

int WtThread::WimDevClearOneImpl()
{
    if(RcdCarNum<1)
    {
        return -1;
    }
    for(int num=0;num<RcdCarNum-1;num++)
    {
        memcpy(&car[num],&car[num+1],sizeof(car[0]));
    }
    memset(&car[RcdCarNum-1],0,sizeof(car[0]));

    RcdCarNum--;
    return 0;
}

int WtThread::WimDevClearOneDataImpl(int VehicleID)
{
    if(VehicleID>RcdCarNum)
    {
        return -1;
    }
    for(int num=VehicleID-1;num<RcdCarNum-1;num++)
    {
        memcpy(&car[num],&car[num+1],sizeof(car[0]));
    }
    memset(&car[RcdCarNum-1],0,sizeof(car[0]));

    if(RcdCarNum>0)
        RcdCarNum--;
    else
    {
        return -1;
    }
    return 0;
}

int WtThread::WimDevTestImpl(char *bStatus)
{
    *bStatus=Status;
    return 0;
}

int WtThread::WimDevInitImpl(int IniType)
{
    if(getTcpClient()->isConnected() == false)
    {
       getTcpClient()->LogMsg(tr("执行WimDev_Init，IniType=%1_1,返回值0").arg(IniType));
        return 0;
    }
    if(IniType==0)
    {
        //强制清空数据
        memset(&car[0],0,sizeof(car[0]));
        memset(&car[1],0,sizeof(car[0]));
        memset(&car[2],0,sizeof(car[0]));
        memset(&car[3],0,sizeof(car[0]));
        memset(&car[4],0,sizeof(car[0]));
        getTcpClient()->LogMsg(tr("执行WimDev_Init，IniType=%1_2,返回值0").arg(IniType));
        return 0;
    }
    GetData_flag=0;
    getTcpClient()->LogMsg(tr("执行WimDev_Init，IniType=%1_1,返回值1").arg(IniType));
    return 1;
}

int WtThread::WimDevSetComImpl(char *comNo, int bps)
{
    m_nBps = bps;
//    QString settings = QString::number(m_nBps) + "8,n,1";
    getTcpClient()->LogMsg(tr("执行WimDev_SetCom"));
    if (getTcpClient()->isConnected())
    {
        return 0;
    }
    else
    {
        if(getTcpClient()->ConnecttoServer(comNo))
        {
            getTcpClient()->SetType(DEV_WT);
            getTcpClient()->SetFilename("./LogWt.log");
            getTcpClient()->SetBack(GetBack);
            getTcpClient()->SendInit();
            return 0;
        }
        return -1;
    }
}

int WtThread::WimDevGetCountImpl()
{
    return RcdCarNum;
}

int WtThread::WimDevAxisDataImpl(int VehicleID, int AxisID, int *AxisType, long TyreWeight[])
{
//    int type;
//    int AxleID=0;

//    if(car[VehicleID-1].valid==0)
//    {
//        return -1;
//    }

//    type=car[VehicleID-1].AxleGroupType[0]*1000+car[VehicleID-1].AxleGroupType[1]*100+car[VehicleID-1].AxleGroupType[2]*10+car[VehicleID-1].AxleGroupType[3];

    TyreWeight[0]=0;
    TyreWeight[1]=0;
    TyreWeight[2]=0;
    TyreWeight[3]=0;

    *AxisType=car[VehicleID-1].AxleGroupType[AxisID-1];
    qDebug()<<*AxisType<<" AxleGroupType";
    TyreWeight[0]=car[VehicleID-1].AxleWt[AxisID-1];
//    if(AxisID==1)
//    {
//        TyreWeight[0]=car[VehicleID-1].AxleWt[0];
//        return 0;
//    }
//    else
//    {
//        for(int i=0;i<AxisID-1;i++)
//            AxleID+=TypeNum[car[VehicleID-1].AxleGroupType[i]];
//        for(int i=0;i<TypeNum[*AxisType];i++)
//        {
//            TyreWeight[i]=car[VehicleID-1].AxleWt[AxleID+i];
//            qDebug()<<TyreWeight[i]<<"AxleGroupWt"<<car[VehicleID-1].AxleWt[AxleID+i];
//        }
//        return 0;
//    }
}

int WtThread::WimDevDataImpl(int VehicleID, long *Weight, long *WtLimit, int AxisType[], double *speed)
{
    long wt=0;
    if(RcdCarNum==0||VehicleID>RcdCarNum)
    {
        return -1;
    }

    for(int i=0;i<car[VehicleID-1].AxleNum;i++)
        wt+=car[VehicleID-1].AxleWt[i];
    *Weight=wt;
    *WtLimit=WeightLimit(VehicleID);
    AxisType[0]=car[VehicleID-1].AxleGroupNum;

    AxisType[1]=car[VehicleID-1].AxleGroupType[0];
    AxisType[2]=car[VehicleID-1].AxleGroupType[1];
    AxisType[3]=car[VehicleID-1].AxleGroupType[2];
    AxisType[4]=car[VehicleID-1].AxleGroupType[3];

    *speed=car[VehicleID-1].Speed/10;
    return 0;
}

int WtThread::WimDevAxisInfoImpl(int VehicleID, long AxisWeight[], double *speed)
{
    if(car[VehicleID-1].valid==0)
    {
        return -1;
    }
    AxisWeight[0]=car[VehicleID-1].AxleNum;
    for(int i=1;i<=car[VehicleID-1].AxleNum;i++)
    {
        AxisWeight[i]=car[VehicleID-1].AxleWt[i-1];
    }

    *speed=car[VehicleID-1].Speed/10;
    return 0;
}

int WtThread::WimDevAxisCountImpl(int VehicleID)
{
    return car[VehicleID-1].AxleNum;
}

int WtThread::WimDevAxisCount1Impl(int VehicleID)
{
    return 0;
}

int WtThread::WimDevAxisData1Impl(int VehicleID, int AxisID, int *AxisSpeed, int *Weight)
{
    return 0;
}

int WtThread::WimDevGetVerNoImpl(char *VerNo)
{
    memcpy(VerNo,Ver,sizeof(Ver));
    return 0;
}


int WtThread::WimDevGetInfoImpl(char *Info)
{
    return 0;
}

int WtThread::WimDevSynchroImpl()
{
    return 0;
}

int WtThread::WimDevStopImpl()
{
    int result=0;
    QMutexLocker locker(&g_mutex);
    getTcpClient()->Disconnected();
    getTcpClient()->LogMsg(tr("WimDev_Close,返回值=%1\n").arg(result));
    return result;
}

quint8 WtThread::GetXOR(const char *Buff, int nLen)
{
    if(!Buff)
        return 0;
    quint8 tmp=0;
    for(int i=0;i<nLen;++i)
    {
        tmp = tmp ^(quint8)Buff[i];
    }
    return tmp;
}

void GetBack(QByteArray &data)
{
    if(TestCnt<300)
        TestCnt++;
    if(GameOverFlag)
    {
        return;
    }
    if(RcdCarNum>9)
        getWt()->WimDevClearOneImpl();
    RcdCarNum++;
    qDebug()<<RcdCarNum<<" CarNum";
    getTcpClient()->LogMsg(QObject::tr("收到称重数据,RcdCarNum=%1\n").arg(RcdCarNum));
    Status = 0;
    QList<QByteArray> listarr;
    listarr = data.split('>');
    int arrnum;
    arrnum = listarr.count();
    qDebug()<<arrnum<<" listnumb";
    if(listarr[0].size()>1)
    {
        car[RcdCarNum-1].valid = listarr[0].at(0);
        car[RcdCarNum-1].GMflag = listarr[0].at(1);
        car[RcdCarNum-1].Speed = listarr[1].toInt();
    }
    car[RcdCarNum-1].AxleNum = listarr[2].toInt();
    qDebug()<<car[RcdCarNum-1].AxleNum<<" car[RcdCarNum-1].AxleNum";
    QList<QByteArray> lista;
    lista = listarr[3].split('<');
    int numa = lista.count();
    for(int i = 0;i < numa-1; i++)
    {
        car[RcdCarNum-1].AxleWt[i] =lista[i].toLong();
        qDebug()<<car[RcdCarNum-1].AxleWt[i]<<" AxleWt[i]";
    }
//    car[RcdCarNum-1].AxleGroupNum = listarr[4].toInt();
//    QList<QByteArray> listb;
//    listb = listarr[5].split('<');
//    for(int i = 0; i < car[RcdCarNum-1].AxleGroupNum-1;i++)
//    {
//        car[RcdCarNum-1].AxleGroupWt[i] =listb[i].toLong();
//    }
//    QList<QByteArray> listc;
//    listc = listarr[6].split('<');
//    for(int i = 0;i < car[RcdCarNum-1].AxleGroupNum-1; i++)
//    {
//        car[RcdCarNum-1].AxleGroupType[i] =listc[i].toLong();

//    }
    //    根据结构体中的某个值判断是否倒车,如何判断？
    if(car[RcdCarNum-1].GMflag == '1')
    {
        if(RcdCarNum>0)
        {
            for(int i=0;i<RcdCarNum;i++)
            {
                memset(&car[i],0,sizeof(car[0]));
            }
            RcdCarNum=0;
        }
        getTcpClient()->LogMsg(QObject::tr("收到倒车数据,删除缓存后RcdCarNum=%1\n").arg(RcdCarNum));
    }
}
