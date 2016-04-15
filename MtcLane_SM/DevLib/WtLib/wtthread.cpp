#include "wtthread.h"
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDateTime>
QFile file("Wt.log");
char tmpBuf[2049];
void LogMsg(const QString& sLog)
{
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
void ConCRC(unsigned char cCon[],int ilen)
{
    unsigned short crc16;
    crc16=0;
    for(int i=0;i<ilen;i++)
        crc16=xcrc(crc16,cCon[i]);
    cCon[ilen]=(unsigned char)(crc16>>8);
    cCon[ilen+1]=(unsigned char)(crc16&0xff);
}

WtThread::WtThread(QObject *parent) :
    QThread(parent), m_pPort(NULL)
{
    del_id=0;
    del_sign=0;
    nBreakFrame=0;
    g_Synchro_sig=0;
    ComTerNum=0;
    dss=0;
    lStaticVal=-1000;
    bReSend=0;
    count=0;
    gcnt=0;
    bTerm=false;
    cEquSta=0;
    cVehNum=0;
    cCurVeh=0;
    cLastVeh=0;
    for(int i=0;i<CACHNUM;i++)
    {
        ClearVDOne(i);
    }
    m_fleCondition.wakeAll();
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
    LogMsg("weight thread begin running...");
    //打开串口
    if(m_pPort == NULL)
    {
        return;
    }
    //数据接受
    static int d_all_b=0;
    static int ch_all_num = 0;
    while(!bTerm)
    {
        msleep(TIMERDELAY);
        d_all_b++;
        if(d_all_b==3)
        {
            if(del_sign==1)
            {
                SendDelCommand(del_id,cVehNum);
            }
            d_all_b=0;
        }

        ch_all_num++;
        if (ch_all_num >= 40)
        {
            ch_all_num = 0;
            Chech_Num++;
            if (Chech_Num > 1000)
            {
                Chech_Num = 5;
            }
        }
        DataReceive();
    }
}

void WtThread::SendDelCommand(unsigned char del_id_b, unsigned char Cac_num)
{
    clowConDel[0]=0xff;
    clowConDel[1]=0;
    clowConDel[2]=CDE;
    clowConDel[3]=del_id_b;
    clowConDel[4]=Cac_num;
    ConCRC(clowConDel,SEND_LEN-2);
    //串口操作加锁
    QMutexLocker locker(&m_portMutex);
    qint64 rgd = m_pPort->write((const char*)clowConDel, SEND_LEN);
    if(rgd==SEND_LEN)
    {
    }
    else
    {
        m_pPort->write((const char*)clowConDel, SEND_LEN);
    }
}

void WtThread::SendChkCommand()
{
    clowConChk[0]=0xff;
    clowConChk[1]=0;
    clowConChk[2]=CHK;
    clowConChk[3]=0;
    clowConChk[4]=0;
    ConCRC(clowConChk,SEND_LEN-2);
    //串口操作加锁
    QMutexLocker locker(&m_portMutex);
    qint64 rgd = m_pPort->write((const char*)clowConChk, SEND_LEN);
    if(rgd==SEND_LEN)
    {
    }
    else
    {
        m_pPort->write((const char*)clowConChk, SEND_LEN);
    }
}

void WtThread::SenOthCommand(unsigned char Con_chr)
{
    clowCon[0]=0xff;
    clowCon[1]=0;
    clowCon[2]=Con_chr;
    clowCon[3]=0;
    clowCon[4]=0;
    ConCRC(clowCon,SEND_LEN-2);
    //串口操作加锁
    QMutexLocker locker(&m_portMutex);
    qint64 rgd = m_pPort->write((const char*)clowCon, SEND_LEN);
    if(rgd==SEND_LEN)
    {
    }
    else
    {
        m_pPort->write((const char*)clowCon, SEND_LEN);
    }
}

bool WtThread::DataCRC()
{
    unsigned short crc16;
    crc16=0;
    for(int i=0;i<(DataBuf[3]-2);i++)
    {
        crc16=xcrc(crc16,DataBuf[i]);
    }
    if((DataBuf[count-2]==(unsigned char)(crc16>>8))
            && (DataBuf[count-1]==(unsigned char)(crc16&0xff)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void WtThread::DataReceive()
{
    static int syncount=0;

    if(m_pPort == NULL)
    {
        return;
    }
    if(!m_pPort->isOpen())
    {
        return;
    }

    if (nBreakFrame==0)
    {
        gcnt=0;
        count=0;
    }
    nBreakFrame=0;
    if(g_Synchro_sig==1)
    {
        syncount++;
        if(syncount>=2)
        {
            m_synCondition.wakeAll();
            syncount=0;
        }
    }
    //    COMSTAT cs;
    //    unsigned long dwError,nRe;
    //    ClearCommError(hinst,&dwError,&cs);
    //    //dss++;
    //    if(cs.cbInQue>=1024)  cs.cbInQue=0;
    //    if(cs.cbInQue>0)
    //    {
    //        nBreakFrame=1;
    //        syncount=0;
    //        ReadFile(hinst,&buf,cs.cbInQue,&nRe,NULL);
    //#ifdef WRITELOG_OK
    //        char a[20];
    //        int i_ll=0;
    //        fsf=_open("aaa.txt",_O_RDWR | _O_CREAT|_O_APPEND, _S_IREAD | _S_IWRITE);
    //#endif
    //检查是否有可读取数据，进行数据读取
    qint64 byteAvailable = m_pPort->bytesAvailable();
    if(byteAvailable > 0)
    {
        nBreakFrame = 1;
        m_portMutex.lock();
        m_pPort->read((char*)buf, byteAvailable);
        m_portMutex.unlock();

        for(unsigned int i=0;i<byteAvailable;i++)
        {
            //数据组合
            if(gcnt==0)
            {
                if(buf[i]==0xff)
                {
                    DataBuf[0]=buf[i];
                    gcnt=1;
                }
            }
            else if(gcnt==1)
            {
                if(buf[i]==0)
                {
                    DataBuf[1]=buf[i];
                    gcnt=gcnt+1;
                }
                else
                {
                    if(buf[i]==0xff)
                    {
                        DataBuf[0]=buf[i];
                        gcnt=1;
                    }
                    else
                        gcnt=0;
                }
            }
            else if(gcnt==2)
            {
                switch(buf[i])
                {
                case WVG:case WVA:case VGI:
                case GAI:case CHK:case SYN:
                case CLN:case INI:case LIN:
                case DYN:case STC:case REP:
                case CAP:case CDE:
                    DataBuf[2]=buf[i];
                    gcnt=gcnt+1;
                    break;
                default:
                    if(buf[i]==0xff)
                    {
                        DataBuf[0]=buf[i];
                        gcnt=1;
                    }
                    else
                        gcnt=0;
                    break;
                }
            }
            else if(gcnt==3)
            {
                if((buf[i]<=256)&&(buf[i]>=7))
                {
                    count=buf[i];
                    DataBuf[gcnt]=buf[i];
                    gcnt=gcnt+1;
                }
                else
                {
                    if(buf[i]==0xff)
                    {
                        DataBuf[0]=buf[i];
                        gcnt=1;
                    }
                    else
                        gcnt=0;
                }
            }
            else if(gcnt==count-1)
            {
                DataBuf[gcnt]=buf[i];
                DataProc();
                count=0;
                gcnt=0;
            }
            else if(gcnt>=4)
            {
                DataBuf[gcnt]=buf[i];
                gcnt++;
            }
            else
            {
                if(buf[i]==0xff)
                {
                    DataBuf[0]=buf[i];
                    gcnt=1;
                }
                else
                    gcnt=0;
            }
        }
    }
}
#define CON_BN 19
void WtThread::DataProc()
{

    if(DataCRC())
    {
        bReSend=0;
        switch(DataBuf[2])
        {
        case WVG:
            LogMsg(QObject::tr("weight thread has data to read, the count is %1").arg(count));
            raw2hex((char*)DataBuf, count, tmpBuf, 2049);
            LogMsg(QObject::tr("weight thread's new data is [%1]").arg(QString(tmpBuf)));
            if(DataBuf[3]>CON_BN+2)
            {
                VD[cLastVeh].AxisNumber=DataBuf[18];
                if (VD[cLastVeh].AxisNumber>MAX_AXLE_NUM) VD[cLastVeh].AxisNumber=MAX_AXLE_NUM;
                VD[cLastVeh].SingleAxisNum=DataBuf[17];
                if (VD[cLastVeh].SingleAxisNum>MAX_AXLE_NUM) VD[cLastVeh].SingleAxisNum=MAX_AXLE_NUM;
                VD[cLastVeh].Veh_SN=DataBuf[4];
                VD[cLastVeh].Veh_CAC=DataBuf[5];
                VD[cLastVeh].speed=(DataBuf[15]+DataBuf[14]*256)/10;
                VD[cLastVeh].whloelimit=0;
                VD[cLastVeh].whloeweight=0;
                VD[cLastVeh].Acc=DataBuf[16];
                VD[cLastVeh].over_sign=DataBuf[13];
                VD[cLastVeh].v_year_m_d=(DataBuf[6]<<24)+(DataBuf[7]<<16)+(DataBuf[4]<<8)+(DataBuf[5]);
                VD[cLastVeh].v_hour_m_s=(DataBuf[8]<<24)+(DataBuf[9]<<16)+(DataBuf[10]<<8);
                for(int i=0;i<VD[cLastVeh].AxisNumber;i++)
                {
                    VD[cLastVeh].AxisWeight[i]=(DataBuf[CON_BN+2*i]*256+DataBuf[CON_BN+2*i+1])*10;
                    VD[cLastVeh].AxisCode[i]=DataBuf[CON_BN+2*VD[cLastVeh].AxisNumber+i]&0x0f;
                    VD[cLastVeh].AxisSign[i]=(DataBuf[CON_BN+2*VD[cLastVeh].AxisNumber+i]&0xf0)>>4;
                    VD[cLastVeh].whloeweight=VD[cLastVeh].whloeweight+VD[cLastVeh].AxisWeight[i];
                }
                DivideAxleGrp((unsigned char*)(VD[cLastVeh].AxisCode),(unsigned char*)VD[cLastVeh].AxisSign,VD[cLastVeh].AxisNumber);
                Un_Deal_data();
                for(int i=0;i<VD[cLastVeh].AxisNumber;i++)
                {
                    if((VD[cLastVeh].AxisCode[i]>0)&&(VD[cLastVeh].AxisCode[i]<10))
                        VD[cLastVeh].whloelimit=VD[cLastVeh].whloelimit+axislimt[VD[cLastVeh].AxisCode[i]-1];
                }

                for(int i=0;i<VD[cLastVeh].SingleAxisNum;i++)
                {
                    VD[cLastVeh].AxisDistance[i]=DataBuf[CON_BN+3*VD[cLastVeh].AxisNumber+2*i]*256+
                            DataBuf[CON_BN+3*VD[cLastVeh].AxisNumber+2*i+1];
                    VD[cLastVeh].Axle_Speed[i]=DataBuf[CON_BN+3*VD[cLastVeh].AxisNumber+2*VD[cLastVeh].SingleAxisNum-2+i];
                }

                if(g_Synchro_sig==1)
                {
                    //                    if(Read_Synchro_data())
                    //                    {
                    //                        ClearVDOne(cLastVeh);
                    //                    }
                    //                    else
                    {
                        cLastVeh++;
                        if(cLastVeh==CACHNUM) cLastVeh=0;
                        if (cVehNum<CACHNUM)
                        {
                            cVehNum++;
                        }
                        else
                        {
                            cCurVeh++;
                            if(cCurVeh==CACHNUM)
                                cCurVeh=0;
                        }
                        //							PostMessage(HWND_BROADCAST, WM_LOWNEWDATACOME, VD[cLastVeh].AxisNumber, 0);
                    }
                }
                else
                {
                    cLastVeh++;
                    if(cLastVeh==CACHNUM) cLastVeh=0;
                    if (cVehNum<CACHNUM)
                    {
                        cVehNum++;
                    }
                    else
                    {
                        cCurVeh++;
                        if(cCurVeh==CACHNUM)
                            cCurVeh=0;
                        //_write(fvf,"覆盖一辆车\n",11);
                    }
                    //						PostMessage(HWND_BROADCAST, WM_LOWNEWDATACOME, VD[cLastVeh].AxisNumber, 0);
                }
            }
            break;

        case CLN:
            if (cVehNum>0)
            {
                cVehNum--;
                if (cLastVeh>0)
                {
                    ClearVDOne(cLastVeh-1);
                    cLastVeh--;
                }
                else
                {
                    ClearVDOne(CACHNUM-1);
                    cLastVeh=CACHNUM-1;
                }
            }
            //				PostMessage(HWND_BROADCAST, WM_LOWNEWDATACLEAR, 0, 0);
            break;
        case WVA:
            LogMsg(QObject::tr("weight thread has data to read, the count is %1").arg(count));
            raw2hex((char*)DataBuf, count, tmpBuf, 2049);
            LogMsg(QObject::tr("weight thread's new data is [%1]").arg(QString(tmpBuf)));
            if(DataBuf[3]>CON_BN+2)
            {
                VD[cLastVeh].AxisNumber=DataBuf[18];
                if (VD[cLastVeh].AxisNumber>MAX_AXLE_NUM) VD[cLastVeh].AxisNumber=MAX_AXLE_NUM;
                VD[cLastVeh].SingleAxisNum=DataBuf[17];
                if (VD[cLastVeh].SingleAxisNum>MAX_AXLE_NUM) VD[cLastVeh].SingleAxisNum=MAX_AXLE_NUM;
                VD[cLastVeh].Veh_SN=DataBuf[4];
                VD[cLastVeh].Veh_CAC=DataBuf[5];
                VD[cLastVeh].speed=(DataBuf[15]+DataBuf[14]*256)/10;
                VD[cLastVeh].whloelimit=0;
                VD[cLastVeh].whloeweight=0;
                VD[cLastVeh].Acc=DataBuf[16];
                VD[cLastVeh].over_sign=DataBuf[13];
                VD[cLastVeh].v_year_m_d=(DataBuf[6]<<24)+(DataBuf[7]<<16)+(DataBuf[4]<<8)+(DataBuf[5]);
                VD[cLastVeh].v_hour_m_s=(DataBuf[8]<<24)+(DataBuf[9]<<16)+(DataBuf[10]<<8);
                for(int i=0;i<VD[cLastVeh].SingleAxisNum;i++)
                {
                    VD[cLastVeh].SingleAxisWeight[i]=(DataBuf[CON_BN+i*2]*256+DataBuf[CON_BN+1+2*i])*10;
                    VD[cLastVeh].whloeweight=VD[cLastVeh].whloeweight+VD[cLastVeh].SingleAxisWeight[i];
                    VD[cLastVeh].AxisDistance[i]=DataBuf[CON_BN+2*VD[cLastVeh].SingleAxisNum+
                            VD[cLastVeh].AxisNumber+2*i]*256+DataBuf[CON_BN+2*VD[cLastVeh].SingleAxisNum+
                            VD[cLastVeh].AxisNumber+2*i+1];
                    VD[cLastVeh].Axle_Speed[i]=DataBuf[CON_BN+4*VD[cLastVeh].SingleAxisNum-2+
                            VD[cLastVeh].AxisNumber+i];
                }
                //计算轴数
                int AxisCop=0;
                for(int i=0;i<VD[cLastVeh].AxisNumber;i++)
                {
                    VD[cLastVeh].AxisCode[i]=DataBuf[CON_BN+2*VD[cLastVeh].SingleAxisNum+i]&0x0f;
                    VD[cLastVeh].AxisSign[i]=(DataBuf[CON_BN+2*VD[cLastVeh].SingleAxisNum+i]&0xf0)>>4;
                    //						if((VD[cLastVeh].AxisCode[i]>0)&&(VD[cLastVeh].AxisCode[i]<10))
                    //							VD[cLastVeh].whloelimit=VD[cLastVeh].whloelimit+axislimt[VD[cLastVeh].AxisCode[i]-1];
                    AxisCop=AxisCop+AxisGN[VD[cLastVeh].AxisCode[i]];
                    VD[cLastVeh].AxisWeight[i]=0;
                    if(AxisCop<=VD[cLastVeh].SingleAxisNum)
                        for(unsigned int j=0;j<AxisGN[VD[cLastVeh].AxisCode[i]];j++)
                            VD[cLastVeh].AxisWeight[i]=VD[cLastVeh].AxisWeight[i]
                                    +VD[cLastVeh].SingleAxisWeight[AxisCop-j-1];

                }
                /*if((VD[cLastVeh].SingleAxisNum<6)&&(VD[cLastVeh].SingleAxisNum>0))
                        {
                            VD[cLastVeh].whloelimit=VehWholeLimit[VD[cLastVeh].SingleAxisNum-1];
                        }
                        else if(VD[cLastVeh].SingleAxisNum>=6)
                        {
                            VD[cLastVeh].whloelimit=VehWholeLimit[5];
                        }*/
                VD[cLastVeh].Acc=DataBuf[16];
                VD[cLastVeh].v_year_m_d=(DataBuf[6]<<24)+(DataBuf[7]<<16)+(DataBuf[4]<<8)+(DataBuf[5]);
                VD[cLastVeh].v_hour_m_s=(DataBuf[8]<<24)+(DataBuf[9]<<16)+(DataBuf[10]<<8);
                DivideAxleGrp((unsigned char*)(VD[cLastVeh].AxisCode),(unsigned char*)VD[cLastVeh].AxisSign,VD[cLastVeh].AxisNumber);
                Un_Deal_data();
                //-------------add by rivera_20070325-------------------
                for(int i=0;i<VD[cLastVeh].SingleAxisNum;i++)
                {
                    if((VD[cLastVeh].AxisCode[i]>0)&&(VD[cLastVeh].AxisCode[i]<10))
                        VD[cLastVeh].whloelimit=VD[cLastVeh].whloelimit+axislimt[VD[cLastVeh].AxisCode[i]-1];
                }
                //------------------------------------------------------
                if(g_Synchro_sig==1)
                {
                    //                    if(Read_Synchro_data())
                    //                    {
                    //                        ClearVDOne(cLastVeh);
                    //                    }
                    //                    else
                    {
                        cLastVeh++;
                        if(cLastVeh==CACHNUM) cLastVeh=0;
                        if (cVehNum<CACHNUM)
                        {
                            cVehNum++;
                        }
                        else
                        {
                            cCurVeh++;
                            if(cCurVeh==CACHNUM)
                                cCurVeh=0;
                        }
                        //							PostMessage(HWND_BROADCAST, WM_LOWNEWDATACOME, VD[cLastVeh].AxisNumber, 0);
                    }
                }
                else
                {
                    cLastVeh++;
                    if(cLastVeh==CACHNUM) cLastVeh=0;
                    if (cVehNum<CACHNUM)
                    {
                        cVehNum++;
                        /*	fac=fac+1;
                                        char bb[20];
                                        sprintf(bb,"增加一辆车%d\n",fac);
                                        _write(fvf,bb,14);*/
                    }
                    else
                    {
                        cCurVeh++;
                        if(cCurVeh==CACHNUM)
                            cCurVeh=0;
                        //_write(fvf,"覆盖一辆车\n",11);
                    }
                    //						PostMessage(HWND_BROADCAST, WM_LOWNEWDATACOME, VD[cLastVeh].AxisNumber, 0);
                }
            }
            break;
        case VGI:
            break;
        case GAI:
            break;
        case CHK:
            cEquSta=DataBuf[4];
            Chech_Num = 0;
            //SetEvent(cComTan);
            m_comCondition.wakeAll();
            break;
        case STC:
            lStaticVal=(DataBuf[4]-DataBuf[4]/16*6)+(DataBuf[5]-DataBuf[5]/16*6)*100+
                    (DataBuf[6]-DataBuf[6]/16*6)*10000;
            //				PostMessage(HWND_BROADCAST, WM_LOWSTATICDATA, lStaticVal, 0);
            break;
        case CAP:
            //			PostMessage(HWND_BROADCAST, WM_LOWCAPTURE, 0, 0);
            break;
        case CDE:
            del_sign=0;
            //SetEvent(CDelTan);
            m_delCondition.wakeAll();
            break;

        }
    }
    //校验失败，请求重发
    else
    {
        if (bReSend<3)
        {
            bReSend=bReSend+1;
            clowConRe[0]=0xff;
            clowConRe[1]=0;
            clowConRe[2]=REP;
            clowConRe[3]=0;
            clowConRe[4]=0;
            ConCRC(clowConRe,SEND_LEN);

            //WriteFile(hinst,clowConRe,SEND_LEN,&rgd,NULL);
            m_portMutex.lock();
            qint64 length = m_pPort->write((const char*)clowConRe, SEND_LEN);
            m_portMutex.unlock();
            if(length==SEND_LEN)
            {
            }
            else
            {
                //WriteFile(hinst,clowConRe,SEND_LEN,&rgd,NULL);
                m_portMutex.lock();
                m_pPort->write((const char*)clowConRe, SEND_LEN);
                m_portMutex.unlock();

            }
            return;
        }
        else
        {
        }
    }
}
#define BYTE_EACHVEH 20
unsigned char Synchro_file_data_input[CACHNUM*BYTE_EACHVEH];
unsigned char Synchro_file_data_output[CACHNUM*BYTE_EACHVEH];
unsigned char Synchro_OneVeh[BYTE_EACHVEH];
unsigned char Synchro_temp[BYTE_EACHVEH];

void WtThread::Un_Deal_data()
{
    //两轴11 3吨变 12
    if((VD[cLastVeh].AxisCode[0]==1)&&(VD[cLastVeh].AxisCode[1]==1)&&
            (VD[cLastVeh].whloeweight>=3000)&&(VD[cLastVeh].SingleAxisNum==2))
    {
        VD[cLastVeh].AxisCode[1]=2;
    }
}

void WtThread::AddAxleGrp(unsigned char *axleGrpType, int index, int &axleGrpSum, unsigned char val1, unsigned char val2)
{
    axleGrpType[index]=val1;
    for (int j=axleGrpSum-1;j>index;j--)
    {
        axleGrpType[j+1]=axleGrpType[j];
    }
    axleGrpType[index+1]=val2;
    axleGrpSum++;
}

bool WtThread::Read_Synchro_data()
{
    return false;
}

bool WtThread::Write_Synchro_data()
{
    return true;
}

int WtThread::WimDevClearOneImpl()
{
    del_id=VD[cCurVeh].Veh_SN;
    SendDelCommand(del_id,cVehNum);
    //ResetEvent(CDelTan);
    m_delCondition.wakeAll();
    if(!m_pPort->isOpen())
    {
        return -1;
    }
    if (cVehNum>0)
    {
        ClearVDOne(cCurVeh);
        cVehNum--;
        cCurVeh++;
        if (cCurVeh==CACHNUM) cCurVeh=0;
    }
    else
    {
        return -1;
    }

    m_del.lock();
    bool nRlt = m_delCondition.wait(&m_del, 500);
    m_del.unlock();
    if (!nRlt)
    {
        del_sign=1;
        return 0;
    }
    else
    {
        return 0;
    }
}

int WtThread::WimDevClearOneDataImpl(int VehicleID)
{
    if ((VehicleID<=0)||(VehicleID>cVehNum))
    {
        return -1;
    }
    if(!m_pPort->open())
    {
        return -2;
    }

    if (cVehNum>0)
    {
        if((VehicleID<=cVehNum)&&(VehicleID>0))
        {
            int numb=(VehicleID-1)+cCurVeh;
            if (numb>=CACHNUM) numb=numb-CACHNUM;
            // Write_Synchro_data();
            if (VehicleID==1)
            {
                del_id=VD[numb].Veh_SN;
                cVehNum--;
                ClearVDOne(numb);
                cCurVeh++;
                if (cCurVeh==CACHNUM) cCurVeh=0;
                SendDelCommand(del_id,cVehNum);
                //ResetEvent(CDelTan);
                //DWORD dRet=WaitForSingleObject(CDelTan,500);
                m_delCondition.wakeAll();

                m_del.lock();
                bool rlt = m_delCondition.wait(&m_del, 500);
                m_del.unlock();

                if (!rlt)
                {
                    del_sign=1;
                    return 0;
                }
                else
                {
                    return 1;
                }
                /*
                    ResetEvent(CDelTan);
                    DWORD dRet=WaitForSingleObject(CDelTan, 500);
                    if (dRet==WAIT_TIMEOUT)
                    {
                    ClearLowCach(numberit);
                    }
                    */
            }
            else
            {
                del_id=VD[numb].Veh_SN;
                for (int aa=VehicleID;aa<cVehNum;aa++)
                {
                    int bb=(aa-1)+cCurVeh;
                    if (bb>=CACHNUM) bb=bb-CACHNUM;
                    int cc=aa+cCurVeh;
                    if (cc>=CACHNUM) cc=cc-CACHNUM;
                    VD[bb]=VD[cc];
                }
                int dd=cVehNum;
                cVehNum--;
                int num=(dd-1)+cCurVeh;
                if (num>=CACHNUM) num=num-CACHNUM;
                ClearVDOne(num);
                if(cLastVeh==0)
                {
                    cLastVeh=9;
                }
                else
                {
                    cLastVeh--;
                }

                SendDelCommand(del_id,cVehNum);
                m_delCondition.wakeAll();

                m_del.lock();
                bool rlt = m_delCondition.wait(&m_del, 500);
                m_del.unlock();
                //                ResetEvent(CDelTan);
                //                DWORD dRet=WaitForSingleObject(CDelTan,500);
                if (!rlt)
                {
                    del_sign=1;
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return 1;
}

int WtThread::WimDevTestImpl(char *bStatus)
{
    if (Chech_Num >= 3)
    {
        cEquSta = COMERROR;
        *bStatus = cEquSta;
    }
    else
    {
        cEquSta = cEquSta & 0x1f;
        *bStatus = cEquSta;
    }


    return 0;
}

int WtThread::WimDevInitImpl(int IniType)
{
    nBreakFrame=0;
    ComTerNum=0;
    count=0;
    gcnt=0;
    bTerm=false;
    bReSend=0;
    cEquSta=0;
    if(!m_pPort->isOpen())
    {
        return INVALIDDATA;
    }
    //PurgeComm(hinst,PURGE_RXCLEAR);
    char *test = new char[10];
    if((-1==WimDevTestImpl(test))&&(IniType>-1))
    {
        delete []test;
        return INVALIDDATA;
    }
    delete []test;
    if(IniType==0)
    {
        cVehNum=0;
        cCurVeh=0;
        cLastVeh=0;
        for(int i=0;i<CACHNUM;i++)
        {
            ClearVDOne(i);
        }
        SenOthCommand(IAI);
        return 0;
    }
    else if(IniType==-1)
    {
        SenOthCommand(INI);
        return cVehNum;
    }
    else if(IniType>0)
    {
        int iDelVehNum=cVehNum-IniType;
        for(int i=0;i<iDelVehNum;i++)
        {
            WimDevClearOneImpl();
        }
        SenOthCommand(INI);
        return cVehNum;
    }
    else
    {
        return -1;
    }
}

int WtThread::WimDevSetComImpl(char *comNo, int bps)
{
    m_sComName = comNo;
    m_nBps = bps;
    QString settings = QString::number(m_nBps) + "8,n,1";
    m_pPort = new QSerialPort(m_sComName, settings);

    if(!m_pPort->open())
    {
        return -1;
    }
    this->start();
    return 0;
}

int WtThread::WimDevGetCountImpl()
{
    return cVehNum;
}

int WtThread::WimDevAxisDataImpl(int VehicleID, int AxisID, int *AxisType, long TyreWeight[])
{
    for (int k=0; k<4; k++)
    {
        TyreWeight[k]=0;
    }
    if((AxisID>=1)&&(AxisID<=MAX_AXLE_NUM)&&(VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        if (AxisID <= VD[numb].AxisNumber)
        {
            *AxisType=VD[numb].AxisCode[AxisID-1];     //得到指定车辆的指定轴组
            int tmpAxisNum = 0;
            for (int i = 0; i < AxisID - 1; i++)
            {
                int tmpAxisCode = VD[numb].AxisCode[i];
                tmpAxisNum += AxisConsistSinAxis[tmpAxisCode];
            }
            int tmpAxistype = VD[numb].AxisCode[AxisID - 1];
            int curaxisNum = AxisConsistSinAxis[tmpAxistype];
            for (int j = 0; j < curaxisNum; j++)
            {
                TyreWeight[j] = VD[numb].SingleAxisWeight[tmpAxisNum + j];
            }
            return 0;
        }
        else
        {
            for (int i = 0; i < 4; i++)
                TyreWeight[i] = 0;
            *AxisType = 0;
            return -1;
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
            TyreWeight[i] = 0;
        *AxisType = 0;
        return -1;
    }
}

int WtThread::WimDevDataImpl(int VehicleID, long *Weight, long *WtLimit, int AxisType[], double *speed)
{
    if((VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        *speed = VD[numb].speed;           //jjn
        *Weight = VD[numb].whloeweight;
        *WtLimit = VD[numb].whloelimit;
        AxisType[0] = VD[numb].AxisNumber;
        for (int i = 0; i < AxisType[0]; i++)
            //AxisType[i + 1] = VD[numb].AxisWeight[i];   jjn20060616
            AxisType[i + 1] = VD[numb].AxisCode[i];
    }
    else
    {
        AxisType[0] =0;
        return -1;
    }
    return 0;
}

int WtThread::WimDevAxisInfoImpl(int VehicleID, long AxisWeight[], double *speed)
{
    if((VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        int SinAxisCount = VD[numb].SingleAxisNum;
        AxisWeight[0] = SinAxisCount;

        for (int i = 1; i <= SinAxisCount; i++)
        {
            AxisWeight[i] = VD[numb].SingleAxisWeight[i - 1];
        }
        *speed = VD[numb].speed;
    }
    else
    {
        AxisWeight[0]=0;
        *speed = 0;
    }
    return 0;
}

int WtThread::WimDevAxisCountImpl(int VehicleID)
{
    if ((VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        return VD[numb].AxisNumber;
    }
    else
    {
        return INVALIDDATA;
    }
}

int WtThread::WimDevAxisCount1Impl(int VehicleID)
{
    if ((VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        return VD[numb].SingleAxisNum;
    }
    else
    {
        return INVALIDDATA;
    }
}

int WtThread::WimDevAxisData1Impl(int VehicleID, int AxisID, int *AxisSpeed, int *Weight)
{
    if((AxisID>=1)&&(AxisID<=MAX_AXLE_NUM)&&(VehicleID<=cVehNum)&&(VehicleID>0))
    {
        int numb=(VehicleID-1)+cCurVeh;
        if (numb>=CACHNUM) numb=numb-CACHNUM;
        *AxisSpeed = VD[numb].Axle_Speed[AxisID-1];
        *Weight = VD[numb].SingleAxisWeight[AxisID-1];
        return 0;
    }
    else
    {
        return INVALIDDATA;
    }
}


int WtThread::WimDevGetVerNoImpl(char *VerNo)
{
    if(!strcpy(VerNo , VERSION))
    {
        return -1;
    }
    else
    {
        return 0 ;
    }
}


int WtThread::WimDevGetInfoImpl(char *Info)
{
    *Info = 1;
    return 0;
}

int WtThread::WimDevSynchroImpl()
{
    nBreakFrame=0;
    ComTerNum=0;
    count=0;
    gcnt=0;
    bTerm=false;
    bReSend=0;
    cEquSta=0;
    if(!m_pPort->isOpen())
    {
        return false;
    }
    char *test = new char[10];
    if(-1 == WimDevTestImpl(test))
    {
        delete []test;
        return -1;
    }
    delete []test;
    cVehNum=0;
    cCurVeh=0;
    cLastVeh=0;
    for(int i=0;i<CACHNUM;i++)
        ClearVDOne(i);
    g_Synchro_sig=1;
    m_synCondition.wakeAll();
    SenOthCommand(SYN);
    //DWORD dRet=WaitForSingleObject(cSynTan,2000);

    m_syn.lock();
    bool bRlt = m_synCondition.wait(&m_syn, 2000);
    m_syn.unlock();
    g_Synchro_sig=0;
    return 0;
}

int WtThread::WimDevStopImpl()
{
    bTerm = true;
    this->wait();
    return true;
}
void WtThread::ClearVDOne(unsigned char a)
{
    VD[a].AxisNumber=0;
    VD[a].SingleAxisNum=0;
    VD[a].speed=0;
    for(int j=0;j<8;j++)
    {
        VD[a].AxisCode[j]=0;
        VD[a].AxisWeight[j]=0;
        VD[a].SingleAxisWeight[j]=0;
        VD[a].AxisDistance[j]=0;
    }
    VD[a].whloeweight=0;
    VD[a].whloelimit=0;
}

void WtThread::DivideAxleGrp(unsigned char *axleGrpType, unsigned char *axleGrpDiscription, int &axleGrpSum)
{
    int j;
    for(int i=0;i<axleGrpSum;i++)
    {
        if ((axleGrpType[i]>=10) && (axleGrpType[i]<=15))
        {
            for (j=axleGrpSum-1;j>i;j--)
            {
                axleGrpDiscription[j+1]=axleGrpDiscription[j];
            }
            switch(axleGrpDiscription[i])
            {
            case 0:
                AddAxleGrp(axleGrpType,i,axleGrpSum,3,3);
                axleGrpDiscription[i]=0;
                axleGrpDiscription[i+1]=0;
                break;
            case 1:
                AddAxleGrp(axleGrpType,i,axleGrpSum,3,4);
                axleGrpDiscription[i]=0;
                axleGrpDiscription[i+1]=1;
                break;
            case 2:
                AddAxleGrp(axleGrpType,i,axleGrpSum,3,4);
                axleGrpDiscription[i]=0;
                axleGrpDiscription[i+1]=2;
                break;
            case 4:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,3);
                axleGrpDiscription[i]=1;
                axleGrpDiscription[i+1]=0;
                break;
            case 8:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,3);
                axleGrpDiscription[i]=2;
                axleGrpDiscription[i+1]=0;
                break;
            case 3:
                AddAxleGrp(axleGrpType,i,axleGrpSum,3,5);
                axleGrpDiscription[i]=0;
                axleGrpDiscription[i+1]=3;
                break;
            case 12:
                AddAxleGrp(axleGrpType,i,axleGrpSum,5,3);
                axleGrpDiscription[i]=3;
                axleGrpDiscription[i+1]=0;
                break;
            case 5:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,4);
                axleGrpDiscription[i]=1;
                axleGrpDiscription[i+1]=1;
                break;
            case 10:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,4);
                axleGrpDiscription[i]=2;
                axleGrpDiscription[i+1]=2;
                break;
            case 6:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,4);
                axleGrpDiscription[i]=1;
                axleGrpDiscription[i+1]=2;
                break;
            case 9:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,4);
                axleGrpDiscription[i]=2;
                axleGrpDiscription[i+1]=1;
                break;
            case 7:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,5);
                axleGrpDiscription[i]=1;
                axleGrpDiscription[i+1]=3;
                break;
            case 11:
                AddAxleGrp(axleGrpType,i,axleGrpSum,4,5);
                axleGrpDiscription[i]=2;
                axleGrpDiscription[i+1]=3;
                break;
            case 13:
                AddAxleGrp(axleGrpType,i,axleGrpSum,5,4);
                axleGrpDiscription[i]=3;
                axleGrpDiscription[i+1]=1;
                break;
            case 14:
                AddAxleGrp(axleGrpType,i,axleGrpSum,5,4);
                axleGrpDiscription[i]=3;
                axleGrpDiscription[i+1]=2;
                break;
            case 15:
                AddAxleGrp(axleGrpType,i,axleGrpSum,5,5);
                axleGrpDiscription[i]=3;
                axleGrpDiscription[i+1]=3;
                break;
            default:
                break;
            }
        }
    }
}
