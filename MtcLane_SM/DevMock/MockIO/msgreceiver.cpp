#include "msgreceiver.h"
#include "idecoder.h"
#include "idisplay.h"
#include <QtNetwork/QHostAddress>
QFile g_Mock("./Mock.log");
MsgReceiver::MsgReceiver(QObject *parent) :
    QObject(parent)
{   
}
void LogMock(const QString& sLog)
{
    if(!g_Mock.isOpen())
    {
        g_Mock.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QString sFinal = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(sLog);
    g_Mock.write(sFinal.toLocal8Bit());
    g_Mock.flush();
}
void Dec2Char(int num,char data[4])
{
    static const char* tab = "0123456789";
    data[0] = tab[num/1000];
    data[1] = tab[(num%1000)/100];
    data[2] = tab[((num%1000)%100)/10];
    data[3] = tab[((num%1000)%100)%10];
}
int  Char2Dec(char* datac)
{
    int num;
    num = (datac[0]-'0')*1000;
    num = num + (datac[1]-'0')*100;
    num = num + (datac[2]-'0')*10;
    num = num + datac[3]-'0';
    return num;
}

void MsgReceiver::beginRead()
{
    //处理接收的数据
    int typenum;
    QString string;
    qint64 numc = 0;
    qint64 numb = m_socket->bytesAvailable();
    qDebug()<<numb<<"  numb";
    while(numc < numb)
    {
        f_buf.clear();
        m_buffer.clear();
        m_str.clear();
        quint8 bcrc;
        if(numb < 3)
        {
            return;
        }
        m_buffer = m_socket->read(1);
        numc = numc + 1;
        qDebug()<<m_buffer.toInt()<<" 1";
        if(f_type == DEVBEGIN)
        {
            f_type = m_buffer.toInt();
        }
        else
        {
            typenum = m_buffer.toInt();
            if(typenum != f_type)
            {
                m_buffer = m_socket->readAll();
                return;
            }
        }        
        m_buffer.clear();
        m_buffer = m_socket->read(4);
        numc = numc + 4;
        f_length = Char2Dec(m_buffer.data());
        qDebug()<<f_length<<" 2";
        m_buffer.clear();
        if(f_length > 0)
        {
            f_buf = m_socket->read(f_length);
            qDebug()<<f_buf<<" 3";
            numc = numc + f_length;
            bcrc = GetXOR(f_buf.data(),f_length);
        }
        else
        {
            bcrc = '0';
        }   
        connect(getDisplay(f_type),SIGNAL(readySend()),this,SLOT(beginSend()));
        if(strcmp(f_buf.data(),"END") == 0)
        {
            disconnect(getDisplay(f_type),SIGNAL(readySend()),this,SLOT(beginSend()));
            m_socket->disconnectFromHost();
            delete m_socket;
            m_socket =NULL;
            GetEditText(f_type);
            m_str = tr("设备%1关闭socket连接").arg(m_readdata);
            emit ReadyStr(m_choice,m_str);
            return;
        }
        //        m_buffer = m_socket->read(1);
        //        numc = numc + 1;
        //        qDebug()<<m_buffer<<" 4";
        //        f_crc = (quint8)m_buffer.toInt();
        //        if(memcmp(&f_crc,&bcrc,1) == 0)
        //            return;
        //        m_buffer.clear();
        if(f_type == DEV_IO)
        {
            getDisplay(DEV_IO)->GetData(f_buf);
            getDisplay(DEV_IO)->beginDisplay();
            string = QString("连接IO设备，成功");
        }
        if(f_type == DEV_PRT)
        {
            getDisplay(DEV_PRT)->GetData(f_buf);
            getDisplay(DEV_PRT)->beginDisplay();
            string = QString("连接Printer设备，成功");
        }
        if(f_type == DEV_FD)
        {
            getDisplay(DEV_FD)->GetData(f_buf);
            getDisplay(DEV_FD)->beginDisplay();
            string = QString("连接FD设备，成功");
        }
        if(f_type == DEV_VPR)
        {
            getDisplay(DEV_VPR)->beginDisplay();
            string = QString("连接Vpr设备，成功");
        }
        if(f_type == DEV_WT)
        {
            getDisplay(DEV_WT)->beginDisplay();
            string = QString("连接Wt设备，成功");
        }
        qDebug()<<"读取数据成功";
        if(f_buf.isEmpty())
        {
            m_choice = 1;
            m_str = string;
        }
        else
        {
            GetEditText(f_type);
            m_str = tr("In:\n%1%2").arg(m_readdata).arg(f_buf.data());
        }
        LogMock(m_str);
        m_str = tr("//////////////////\n%1\n%2").arg(m_time.currentDateTime().toString("hh:mm:ss.zzz")).arg(m_str);
        emit ReadyStr(m_choice,m_str);
    }
}

void MsgReceiver::setSocket(QTcpSocket *socket)
{
    m_str.clear();
    if(socket != NULL)
    {
        m_socket = socket;
        socketDescriptor = m_socket->socketDescriptor();
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(beginRead()));
        QHostAddress peerip = m_socket->peerAddress();
        m_choice = 1;
        m_str = tr("Net:\n建立一个新连接，对方IP为：%1描述符为：%2").arg(peerip.toString()).arg(socketDescriptor);
        LogMock(m_str);
        f_type = DEVBEGIN;
        f_length = 0;
        qDebug()<<m_str.toLocal8Bit()<<"  socket";
        m_str = m_time.currentDateTime().toString("hh:mm:ss.zzz") +m_str;
        emit ReadyStr(m_choice,m_str);
    }
}

quint8 MsgReceiver::GetXOR(const char *Buff, int nLen)
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

void MsgReceiver::beginSend()
{
    QByteArray temp;
    temp = getDisplay(f_type)->getBuffer();
    if(temp.isEmpty())
    {
        return;
    }
    if(f_type == DEV_IO)
    {
        if(temp == f_buf)
        {
            temp.clear();
            return;
        }
    }
    f_buf.clear();
    f_buf = temp;
    temp.clear();
    m_buffer.clear();
    m_str.clear();
    char datac[4];
    temp = QByteArray::number(f_type);
    m_buffer.append(temp);
    temp.clear();
    f_length = f_buf.size();
    Dec2Char(f_length,datac);
    m_buffer.append(datac[0]);
    m_buffer.append(datac[1]);
    m_buffer.append(datac[2]);
    m_buffer.append(datac[3]);
    m_buffer.append(f_buf);
    if(!m_socket->isOpen())
    {
        m_str = tr("socket断开连接");
        m_choice = 1;
        emit ReadyStr(m_choice,m_str);
        return;
    }
    qint64 num = m_socket->write(m_buffer);
    if(!m_socket->waitForBytesWritten(3000))
    {
        qDebug()<<m_socket->errorString()<<" error";
    }
    qDebug()<<num<<" num "<<socketDescriptor;
    qDebug()<<QDateTime::currentDateTime().toString("hh-mm-ss-zzz")<<" Send";
    GetEditText(f_type);
    m_str = tr("Out:\n%1发送的数据：<%2\n").arg(m_readdata).arg(m_buffer.data());
    LogMock(m_str);
    m_str = QString("//////////////////////////\n") + m_time.currentDateTime().toString("hh:mm:ss.zzz") +m_str;
    emit ReadyStr(m_choice,m_str);
}

void MsgReceiver::GetBuff(QByteArray &databy)
{
    m_buffer.clear();
    m_buffer = databy;
    qDebug()<<m_buffer.size()<<"  out::";
}

void MsgReceiver::GetEditText(int type)
{
    if(type == DEV_IO )
    {
        m_readdata= QString("IO:\n");
        m_choice = 2;
        return;
    }
    if(type == DEV_PRT )
    {
        m_readdata= QString("Printer:\n");
        m_choice = 3;
        return;
    }
    if(type == DEV_FD )
    {
        m_readdata= QString("FD:\n");
        m_choice = 4;
        return;
    }
    if(type == DEV_VPR )
    {
        m_readdata= QString("Vpr:\n");
        m_choice = 5;
        return;
    }
    if(type == DEV_WT )
    {
        m_readdata= QString("Wt:\n");
        m_choice = 6;
        return;
    }
}
