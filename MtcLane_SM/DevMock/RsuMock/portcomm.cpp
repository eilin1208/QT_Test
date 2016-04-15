#include "portcomm.h"
#include <QSettings>
#include <QtSerialPort/QSerialPort>
#include <QFile>
#include <QDateTime>
#include <QDebug>
using namespace TNX;
bool g_Read = false;
QFile file("RSUMock.log");
int num = 0;
void LogMsg(const QString& sLog)
{
    if(!file.isOpen())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    file.write((QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") +
                sLog).toAscii().data());
    file.write("\n");
    file.flush();
}
PortComm::PortComm(QObject *parent) :
    QThread(parent)
{   
}
QSerialPort* getSerialPort(char* sConnPara = NULL)
{
    static QSerialPort port(sConnPara);
    port.setBaudRate(QPortSettings::BAUDR_115200);
    return &port;
}
//将Read到的数据转换成可显示的十六进制
void byte2hex(char ch, char outch[2])
{
    static const char *tab = "0123456789ABCDEF";
    outch[0] = tab[ch >> 4 & 0x0f];
    outch[1] = tab[ch & 0x0f];
}
inline quint8 Char2Hex(char c)
{
    if ('0'<=c && c<='9') return c-'0';
    if ('a'<=c && c<='f') return c-'a'+10;
    if ('A'<=c && c<='F') return c-'A'+10;
    return -1;
}
//将从INI文件中读取的数据转换成十六进制
bool Hex2Raw(quint8 *pData, const char *szMsg, int nMsgLen)
{
    int hi, lo;
    for (int i=0;i<nMsgLen/2;i++)
    {
        hi=Char2Hex(szMsg[i*2]);
        lo=Char2Hex(szMsg[i*2+1]);
        if (hi<0 || lo<0) return false;
        pData[i]=(hi<<4)+lo;
    }
    return true;
}
//不断运行，读取数据
void PortComm::run()
{
    long dwRead;
    QByteArray bytea;
    while(g_Read)
    {
        C0_data.clear();
        C1_data.clear();
        C2_data.clear();
        C6_data.clear();
        C3_data.clear();
        dwRead = getSerialPort()->bytesAvailable();
        if(dwRead < 0)
        {
           continue;
        }
        bytea = getSerialPort()->readAll();
        if(bytea.size()<5)
        {
            continue;
        }
        char ch;
        char charch[2];
        ch = bytea.at(3);
        byte2hex(ch,charch);
        if(charch[0] == 'C')
        {
            if(charch[1] == '1')
            {
                C1_data = bytea;
            }
            if(charch[1] == '2')
            {
                C2_data = bytea;
            }
            if( charch[1] == '3')
            {
                C3_data = bytea;
            }
            if( charch[1] == '6')
            {
                C6_data = bytea;
            }
            if( charch[1] == '0')
            {
                C0_data = bytea;
            }
            m_str = QObject::tr("/C%1\n%2").arg(charch[1]).arg(bytea.toHex().data());
            LogMsg(m_str);
            getFrame();
        }
    }
}
//开始运行RSU
void PortComm::StartRsu()
{
    QString strb;
    QByteArray temp;
    QSettings settings("./Rsu.ini", QSettings::IniFormat);
    temp = settings.value(QString("Port") + "/Port").toByteArray();
    m_port = temp.data();
    bool ret;
    ret = getSerialPort(m_port)->open();
    if(!ret)
    {
        strb = QString("open Rsu fail  ")+QString(temp);
        LogMsg(strb);
    }
    strb = QString("open Rsu success  ")+ QString(temp);
    LogMsg(strb);
    m_str = strb;
    emit ReadyString();
    g_Read = true;
    this->start();
    m_status = B0_frame;
    m_dealnum = 0;
    m_b4deal = 0;
    m_Name = "./RSU.ini";
}
//关闭RSU
void PortComm::CloseRsu()
{
    m_str = QObject::tr("在这次RSU开启中，共进行了%1笔交易，其中完全完成%2笔，到B4的%3笔").arg(m_dealnum+m_b4deal)
            .arg(m_dealnum).arg(m_b4deal);
    LogMsg(m_str);
    emit ReadyString();
    g_Read = false;
    if(getSerialPort()->isOpen())
    {
        getSerialPort()->close();
    }
}

//发送数据，为槽，定时发送
void PortComm::SendFrame()
{
    const char* datac;
    switch(m_status)
    {
      case B0_frame:
        {
            datac = "/bo";
            WriteData(datac);
            break;
        }
      case B2_frame:
        {
          datac = "/B2";
          WriteData(datac);
          break;
        }
      case B3_frame:
        {
          datac = "/B3";
          WriteData(datac);
          break;
        }
      case B4_frame:
        {
          datac = "/B4";
          WriteData(datac);
          break;
        }
      case B5_frame:
        {
          datac = "/B5";
          WriteData(datac);
          break;
        }
    default:break;
    }
}

//为槽，当收到数据时就触发，并且根据当前状态判断是否为所需要的帧
void PortComm::getFrame()
{
    emit ReadyString();
    if(C1_data.isEmpty())
    {
        if(num > 2)
        {
            num= 0;
            return;
        }
    }
    else
    {
        num = 0;
    }
    if(!C0_data.isEmpty())
    {
        const char* datac;
        datac = "/B0";
        WriteData(datac);
        if(m_status == B0_frame)
        {
            m_status = B2_wait;
            emit StatusChange();

        }
        m_str.clear();
        m_str = QString("初始化成功！");
        emit ReadyString();
        return;
    }
    switch(m_status)
    {
        case B2_frame:
            {
                if(!C2_data.isEmpty())
                {
                    num++;
                    m_status = B2_wait;
                    emit StatusChange();
                    m_str = QString("B2收到C2，交易终止");
                    emit ReadyString();
                    break;
                }
                m_status = B3_frame;
                emit StatusChange();
                break;
            }
        case B3_frame:
            {
                if(!C2_data.isEmpty())
                {
                    num++;
                    m_status = B2_wait;
                    emit StatusChange();
                    m_str = QString("B3收到C2，交易终止");
                    emit ReadyString();
                    break;
                }
                m_status = B4_frame;
                emit StatusChange();
                break;
            }
        case B4_frame:
            {
                if(!C2_data.isEmpty())
                {
                    m_b4deal++;
                    m_status = B2_wait;
                    emit StatusChange();
                    m_str = QString("B4收到C2，交易终止");
                    emit ReadyString();
                    break;
                }
                m_status = B5_frame;
                emit StatusChange();
                break;
            }
        case B5_frame:
            {
                num = 0;
                m_status = B2_wait;
                emit StatusChange();
                m_str = QString("交易成功，等待下次交易！\n");
                m_dealnum++;
                if(strcmp(m_Name,"./RSU.ini") == 0)
                {
                    m_Name = "./RSU1.ini";
                }
                else
                {
                    m_Name = "./RSU.ini";
                }
                m_str +=QString("开始进行新的交易，读取自：：")+ QString(m_Name);
                emit ReadyString();
                break;
            }
    default:break;
    }
}
//从INI文件读取B帧数据,并写到串口中
void PortComm::WriteData(const char *datac)
{    
    QSettings settings(m_Name, QSettings::IniFormat);
    QByteArray databy;
    databy = settings.value(QString("RSU")+ datac).toByteArray();
    const char* datad;
    quint8* pData;
    int size;
    m_str.clear();
    m_str = QString(datac)+QString("\n")+QString(databy);
    LogMsg(m_str);
    size = databy.size();
    datad = databy.constData();
    pData = (quint8*)malloc(size/2);
    Hex2Raw(pData,datad,size);
    getSerialPort()->write((const char*)pData,size/2);
    emit ReadyString();
}

QString PortComm::ReadStr()
{
    return m_str;
}

void PortComm::SetStatus(BStatus status)
{
    m_status = status;
}

BStatus PortComm::GetStatus()
{
    return m_status;
}
