#include "wtthread.h"
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QMutexLocker>
QFile file("Wt.log");
QMutex g_mutex;
char tmpBuf[2049];
void LogMsg(const QString& sLog)
{
    QMutexLocker locker(&g_mutex);
    file.write((QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") +
                sLog).toAscii().data());
    file.write("\n");
    file.flush();
}

void byte2hex(char ch, char *out)
{
    static const char *tab = "0123456789ABCDEF";
    out[0] = tab[ch >> 4 & 0x0f];
    out[1] = tab[ch & 0x0f];
}

bool raw2hex(char* in, int insize, char* out, int outsize)
{
    if(insize == 0 || outsize == 0 || in == NULL || out == NULL)
    {
        return false;
    }
    if(insize * 2 + 1 > outsize)
    {
        return false;
    }
    memset(out, 0, outsize);
    for(int i = 0; i < insize; i++)
    {
        byte2hex(in[i], out + i * 2);
    }
    out[insize * 2] = '\0';
    return true;
}

//命令字加校验码
unsigned short GetCRC16(unsigned char * aData, unsigned long aSize )
{
    unsigned long  i;
    unsigned short nAccum = 0;
    unsigned short aLeft;
    unsigned short aRight;
    unsigned short aIndex;
    unsigned short aDataItem;
    try
    {
        for ( i = 0; i < aSize; i++ )
        {
            aLeft = nAccum << 8;
            aRight = nAccum >> 8;
            aRight = aRight & 0xff;
            aDataItem = (*aData++) & 0xff;
            aIndex = aRight ^ aDataItem;
            nAccum = aLeft ^ (unsigned short )crctab[aIndex];
        }
    }
    catch (...)
    {
        return -1;
    }
    return nAccum;
}



typedef struct _CARINF
{
    char valid;
    char GMflag;
    int AxleNum;
    long AxleWt[50];
    int AxleGroupNum;
    long AxleGroupWt[50];
    char AxleGroupType[50];
    int  Speed;

} CARINF, *pCARINF;
CARINF  car[10];

int TypeNum[10]={0,1,1,2,2,2,3,3,3,3};//分别是0123456789型轴型轴组对应的轴数
unsigned char RcdCarNum=0;
unsigned char GameOverFlag=0;

#define xcrc(crc,cp) (crctab[((crc>>8)&0xff)^(cp&0xff)]^(crc<<8))

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
    QThread(parent), m_pPort(NULL)
{
    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
}

WtThread::~WtThread()
{
    if(m_pPort != NULL)
    {
        delete m_pPort;
    }
}

void WtThread::run()
{
    int i;
    long len,dwRead;
    unsigned char RXD_Temp[256];
    unsigned char RXD_Buf[256];
    unsigned char TXD_Buf[10];
    unsigned short crc;

    static quint32 cnt=0;

    while(1)
    {
        msleep(50);
        if(TestCnt<300)
            TestCnt++;
        if(GameOverFlag)
        {
            break;
        }
        m_mutex.lock();
        dwRead=m_pPort->read((char*)RXD_Temp, m_pPort->bytesAvailable());
        if(dwRead>0)
        {
            for(i=0;i<dwRead;i++)
                RXD_Buf[i+cnt]=RXD_Temp[i];
            cnt+=dwRead;
        }
        else
        {
            if(RXD_Buf[0]!=0xff)
                goto end;


            switch(RXD_Buf[2])
            {
            case 4:    //复位
                len=RXD_Buf[3];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2); //校验
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                    goto end;
                Comm_Serial=RXD_Buf[1];

                TXD_Buf[0]=0xff;
                TXD_Buf[1]=Comm_Serial;     //命令序列号
                TXD_Buf[2]=4;     //命令号
                TXD_Buf[3]=0;
                crc=GetCRC16(TXD_Buf,4);
                TXD_Buf[4]=crc/256;
                TXD_Buf[5]=crc%256;
                m_pPort->write((char*)TXD_Buf,6);  //回复
                break;
            case 5:
                crc=GetCRC16(RXD_Buf,4);
                TestCnt=0;
                Status=RXD_Buf[3];
                if(cnt==13)
                {
                    if(RXD_Buf[2]==9)
                    {
                        len=RXD_Buf[3];
                        crc=GetCRC16(RXD_Buf,len-2); //校验
                        if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                            goto end;
                    }
                }
                break;
            case 2:   //称重信息帧
                len=RXD_Buf[3]*256+RXD_Buf[4];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2);
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                {
                    goto end;
                }
                if(RXD_Buf[1]==Comm_Serial)
                    goto rcv1;
                else
                    Comm_Serial=RXD_Buf[1];

                if(RcdCarNum>9)
                    WimDevClearOneImpl();
                RcdCarNum++;
                LogMsg(tr("收到称重数据,RcdCarNum=%1\n").arg(RcdCarNum));
                if(car[RcdCarNum-1].valid==0)
                {
                    car[RcdCarNum-1].valid=1;
                    car[RcdCarNum-1].Speed=RXD_Buf[13]*256+RXD_Buf[14];
                    Status=RXD_Buf[12];
                    car[RcdCarNum-1].AxleNum=RXD_Buf[16];
                    LogMsg(tr("             轴数=%1\n").arg(car[RcdCarNum-1].AxleNum));
                    if(car[RcdCarNum-1].AxleNum>20||car[RcdCarNum-1].AxleNum<2)
                    {
                        car[RcdCarNum-1].AxleNum=2;
                        LogMsg(tr("             出错轴数赋值为2\n"));
                    }
                    for(i=0;i<car[RcdCarNum-1].AxleNum;i++)
                    {
                        car[RcdCarNum-1].AxleWt[i]=(RXD_Buf[19+2*i]*256+RXD_Buf[20+2*i])*10;
                        LogMsg(tr("             轴%1重=%2\n").arg(i).arg(car[RcdCarNum-1].AxleWt[i]));
                    }
                    car[RcdCarNum-1].AxleGroupNum=RXD_Buf[17];
                    LogMsg(tr("             轴组数=%1\n").arg(car[RcdCarNum-1].AxleGroupNum));
                    if(car[RcdCarNum-1].AxleGroupNum>20||car[RcdCarNum-1].AxleGroupNum<2)
                    {
                        car[RcdCarNum-1].AxleGroupNum=2;
                        LogMsg(tr("             出错轴组数赋值为2\n"));
                    }
                    for(i=0;i<car[RcdCarNum-1].AxleGroupNum;i++)
                    {
                        car[RcdCarNum-1].AxleGroupWt[i]=(RXD_Buf[19+2*car[RcdCarNum-1].AxleNum+2*i]*256+RXD_Buf[20+2*car[RcdCarNum-1].AxleNum+2*i])*10;
                        LogMsg(tr("             轴组%1重=%2\n").arg(i).arg(car[RcdCarNum-1].AxleGroupWt[i]));
                    }
                    for(i=0;i<car[RcdCarNum-1].AxleGroupNum;i++)
                    {
                        car[RcdCarNum-1].AxleGroupType[i]=RXD_Buf[19+2*car[RcdCarNum-1].AxleNum+2*car[RcdCarNum-1].AxleGroupNum+i];
                        LogMsg(tr("             轴组%1类型=%2\n").arg(i).arg(car[RcdCarNum-1].AxleGroupType[i]));
                    }
                    for(i=car[RcdCarNum-1].AxleGroupNum;i<50;i++)
                        car[RcdCarNum-1].AxleGroupType[i]=0;
                }


rcv1:
                TXD_Buf[0]=0xff;
                TXD_Buf[1]=Comm_Serial;     //命令序列号
                TXD_Buf[2]=2;     //命令号
                TXD_Buf[3]=0;
                crc=GetCRC16(TXD_Buf,4);
                TXD_Buf[4]=crc/256;
                TXD_Buf[5]=crc%256;
                m_pPort->write((char*)TXD_Buf,6); //回复
                break;
            case 7:              //倒车
                len=RXD_Buf[3];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2); //校验
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                    goto end;
                if(RXD_Buf[1]==Comm_Serial)
                    goto rcv2;
                else
                    Comm_Serial=RXD_Buf[1];
rcv2:
                TXD_Buf[0]=0xff;
                TXD_Buf[1]=Comm_Serial;     //命令序列号
                TXD_Buf[2]=7;     //命令号
                TXD_Buf[3]=0;
                crc=GetCRC16(TXD_Buf,4);
                TXD_Buf[4]=crc/256;
                TXD_Buf[5]=crc%256;
                m_pPort->write((char*)TXD_Buf,6);  //回复
                if(RcdCarNum>0)
                {
                    for(int i=0;i<RcdCarNum;i++)
                    {
                        memset(&car[i],0,sizeof(car[0]));
                    }
                    RcdCarNum=0;
                }
                LogMsg(tr("收到倒车数据,删除缓存后RcdCarNum=%1\n").arg(RcdCarNum));
                break;
            case 1:
                len=RXD_Buf[3];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2); //校验
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                    goto end;
                if(RXD_Buf[1]==Comm_Serial)
                    goto rcv3;
                else
                    Comm_Serial=RXD_Buf[1];
rcv3:
                TXD_Buf[0]=0xff;
                TXD_Buf[1]=Comm_Serial;     //命令序列号
                TXD_Buf[2]=1;     //命令号
                TXD_Buf[3]=0;
                crc=GetCRC16(TXD_Buf,4);
                TXD_Buf[4]=crc/256;
                TXD_Buf[5]=crc%256;
                m_pPort->write((char*)TXD_Buf,6);  //回复
                break;
            case 9:
                len=RXD_Buf[3];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2); //校验
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                    goto end;

                break;
            case 3:
                len=RXD_Buf[3]*256+RXD_Buf[4];
                if(len<3)
                    goto end;
                crc=GetCRC16(RXD_Buf,len-2);
                if((RXD_Buf[len-2]!=crc/256)||(RXD_Buf[len-1]!=crc%256))
                    goto end;
                if(RXD_Buf[1]==Comm_Serial)
                    goto rcv4;
                else
                    Comm_Serial=RXD_Buf[1];
                RcdCarNum++;
                car[0].valid=1;
                car[0].GMflag=RXD_Buf[5];
                car[0].Speed=RXD_Buf[14]*256+RXD_Buf[15];
                Status=RXD_Buf[13];
                car[0].AxleNum=RXD_Buf[17];
                for(i=0;i<car[0].AxleNum;i++)
                    car[0].AxleWt[i]=(RXD_Buf[20+2*i]*256+RXD_Buf[21+2*i])*10;
                car[0].AxleGroupNum=RXD_Buf[18];
                for(i=0;i<car[0].AxleGroupNum;i++)
                    car[0].AxleGroupWt[i]=(RXD_Buf[20+2*car[0].AxleNum+2*i]*256+RXD_Buf[20+2*car[0].AxleNum+2*i])*10;

                for(i=0;i<car[0].AxleGroupNum;i++)
                    car[0].AxleGroupType[i]=RXD_Buf[20+2*car[0].AxleNum+2*car[0].AxleGroupNum+i];

rcv4:
                TXD_Buf[0]=0xff;
                TXD_Buf[1]=Comm_Serial;     //命令序列号
                TXD_Buf[2]=3;     //命令号
                TXD_Buf[3]=0;
                crc=GetCRC16(TXD_Buf,4);
                TXD_Buf[4]=crc/256;
                TXD_Buf[5]=crc%256;
                m_pPort->write((char*)TXD_Buf,6); //回复
                GetData_flag=1;
                break;
            default:break;
            }
            //////////////////////////////////////////////////////////
end:
            cnt=0;
            memset(RXD_Buf,0,256);
        }
        m_mutex.unlock();
    }

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

    unsigned char TXD_Buf[10];
    unsigned short crc;
    TXD_Buf[0]=0xff;
    TXD_Buf[1]=Comm_Serial;     //命令序列号
    TXD_Buf[2]=6;     //删除命令
    TXD_Buf[3]=0;
    crc=GetCRC16(TXD_Buf,4);
    TXD_Buf[4]=crc/256;
    TXD_Buf[5]=crc%256;
    QMutexLocker locker(&m_mutex);
    m_pPort->write((char*)TXD_Buf,6); //回复
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

    unsigned char TXD_Buf[10];
    unsigned short crc;
    TXD_Buf[0]=0xff;
    TXD_Buf[1]=Comm_Serial;     //命令序列号
    TXD_Buf[2]=6;     //删除命令
    TXD_Buf[3]=0;
    crc=GetCRC16(TXD_Buf,4);
    TXD_Buf[4]=crc/256;
    TXD_Buf[5]=crc%256;
    QMutexLocker locker(&m_mutex);
    m_pPort->write((char*)TXD_Buf,6); //回复
    return 0;
}

int WtThread::WimDevTestImpl(char *bStatus)
{
    *bStatus=Status;
    return 0;
}

int WtThread::WimDevInitImpl(int IniType)
{
    int i = 0;
    LogMsg(tr("执行WimDev_Init"));
    if(m_pPort == NULL)
    {
        LogMsg(tr("执行WimDev_Init，IniType=%1_1,返回值0").arg(IniType));
        return 0;
    }
    unsigned char TXD_Buf[10];
    unsigned short crc;

    if(IniType==0)
    {
        //强制清空数据
        memset(&car[0],0,sizeof(car[0]));
        memset(&car[1],0,sizeof(car[0]));
        memset(&car[2],0,sizeof(car[0]));
        memset(&car[3],0,sizeof(car[0]));
        memset(&car[4],0,sizeof(car[0]));
        LogMsg(tr("执行WimDev_Init，IniType=%1_2,返回值0").arg(IniType));
        return 0;
    }
    TXD_Buf[0]=0xff;
    TXD_Buf[1]=Comm_Serial;     //命令序列号
    TXD_Buf[2]=0x0c;
    TXD_Buf[3]=0x0c;
    crc=GetCRC16(TXD_Buf,4);
    TXD_Buf[4]=crc/256;
    TXD_Buf[5]=crc%256;
    //    WriteComm(TXD_Buf,6);
    m_pPort->write((char*)TXD_Buf, 6);
    bool rlt = m_pPort->waitForBytesWritten(50);
    if(!rlt)
    {
        LogMsg(tr("执行WimDev_Init，IniType=%1_3,返回值0").arg(IniType));
        return 0;
    }
    this->start();
    while(!GetData_flag)
    {
        msleep(50);
        if(++i>20)
        {
            LogMsg(tr("执行WimDev_Init，IniType=%1_4,返回值0").arg(IniType));
            return 0;   //超时
        }
    }
    GetData_flag=0;
    LogMsg(tr("执行WimDev_Init，IniType=%1_1,返回值1").arg(IniType));
    return 1;
}

int WtThread::WimDevSetComImpl(char *comNo, int bps)
{
    m_sComName = comNo;
    m_nBps = bps;
    QString settings = QString::number(m_nBps) + "8,n,1";
    m_pPort = new QSerialPort(m_sComName, settings);
    LogMsg(tr("执行WimDev_SetCom"));
    if(!m_pPort->open())
    {
        return -1;
    }
    return 0;
}

int WtThread::WimDevGetCountImpl()
{
    return RcdCarNum;
}

int WtThread::WimDevAxisDataImpl(int VehicleID, int AxisID, int *AxisType, long TyreWeight[])
{
    int type;
    int AxleID=0;

    if(car[VehicleID-1].valid==0)
    {
        return -1;
    }

    type=car[VehicleID-1].AxleGroupType[0]*1000+car[VehicleID-1].AxleGroupType[1]*100+car[VehicleID-1].AxleGroupType[2]*10+car[VehicleID-1].AxleGroupType[3];

    TyreWeight[0]=0;
    TyreWeight[1]=0;
    TyreWeight[2]=0;
    TyreWeight[3]=0;

    *AxisType=car[VehicleID-1].AxleGroupType[AxisID-1];
    if(AxisID==1)
    {
        TyreWeight[0]=car[VehicleID-1].AxleWt[0];
        return 0;
    }
    else
    {
        for(int i=0;i<AxisID-1;i++)
            AxleID+=TypeNum[car[VehicleID-1].AxleGroupType[i]];
        for(int i=0;i<TypeNum[*AxisType];i++)
            TyreWeight[i]=car[VehicleID-1].AxleWt[AxleID+i];
        return 0;
    }
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
    if(car[VehicleID-1].valid==0)
    {
        return -1;
    }
    return car[VehicleID-1].AxleGroupNum;
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
    QMutexLocker locker(&m_mutex);
    m_pPort->close();
    LogMsg(tr("WimDev_Close,返回值=%1\n").arg(result));
    return result;
}
