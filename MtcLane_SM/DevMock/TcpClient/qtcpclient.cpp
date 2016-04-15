#include "qtcpclient.h"
#include <QByteArray>

//转换编码方式
QString UTF8toGB2312(QString str, int length = -1)
{
    QByteArray temp,temp2;
    temp = str.toLocal8Bit();
    QString message;
    temp2 = QTextCodec::codecForName("GB2312")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(temp));
    message = QString(temp2);
    return message;
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
QTcpClient::QTcpClient(QObject *parent) :
    QObject(parent),tcpsocket(NULL),g_callRead(NULL)
{

}
bool QTcpClient::ConnecttoServer(const QString& ip)
{
    tcpsocket = new QTcpSocket(this);
    tcpsocket->connectToHost(ip,19999);
    if(tcpsocket->waitForConnected(1000) == false)
    {
        QString string = tcpsocket->errorString();
        LogMsg(tr("连接IP:%1失败，原因：%2。").arg(ip).arg(string));
        delete tcpsocket;
        tcpsocket = NULL;
        return false;
    }
    if(tcpsocket == NULL)
    {
        qDebug()<<"NULL  FENG";
    }
    LogMsg(tr("连接IP:%1成功").arg(ip));
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(readMessage()));
    return true;
}
void QTcpClient::readMessage()
{
    qDebug()<<"////////////////";
    qint64 numc = 0;
    qint64 numb = tcpsocket->bytesAvailable();
    qDebug()<<numb<<"  numb";
    while(numc < numb)
    {
        int type = 0;
        int length = 0;
        QByteArray databy;
        databy = tcpsocket->read(1);
        numc++;
        type = databy.toInt();
        if(type != m_type)
        {
            databy = tcpsocket->readAll();
            databy.clear();
            continue;
        }
        databy.clear();
        databy = tcpsocket->read(4);
        numc = numc + 4;
        length = Char2Dec(databy.data());
        if(length == 0)
        {
            continue;
        }
        databy.clear();
        databy = tcpsocket->read(length);
        numc = numc +length;
        qDebug()<<m_time.currentDateTime().toString("hh:mm:ss.zzz")<<" Lib";
        (*g_callRead)(databy);
    }
}

int QTcpClient::SendMessage(int length,const char* data)
{
//    m_data.append((char*)&type,4);
//    m_data.append((char*)&length,4);
    QByteArray temp;
    temp = QByteArray::number(m_type);
    m_data.append(temp);
    temp.clear();
    char datac[4];
    Dec2Char(length,datac);
    m_data.append(datac[0]);
    m_data.append(datac[1]);
    m_data.append(datac[2]);
    m_data.append(datac[3]);
    if(length > 0)
    {
      m_data.append(data,length);
    }
   //异或校验
//    m_data.append((char)GetXOR(length,data));
    //发送
    if(tcpsocket == NULL)
    {
        m_data.clear();
        return -1;
    }
    if(tcpsocket->isOpen())
    {
        tcpsocket->write(m_data);
        qDebug()<<m_data<<" ionum";
        if(tcpsocket->waitForBytesWritten(30000))
        {
            qDebug()<<"写数据成功";
        }
        else
        {
            qDebug()<<"写数据失败";
        }
        m_data.clear();
        return 0;
    }
    else
    {
        LogMsg(tcpsocket->errorString());
        delete tcpsocket;
        tcpsocket =NULL;
        return -1;
    }
}

quint8 QTcpClient::GetXOR(int nLen , const char *Buff )
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

void QTcpClient::SetType(int type)
{
    m_type = type;
}

void QTcpClient::Disconnected()
{
    if(tcpsocket != NULL)
    {
        const char* content;
        content = "END";
        SendMessage(strlen(content),content);
        tcpsocket->disconnectFromHost();
        tcpsocket->close();
        delete tcpsocket;
        tcpsocket = NULL;
    }
}

bool QTcpClient::isConnected()
{
    if(tcpsocket == NULL)
    {
        return false;
    }
    return true;
}

void QTcpClient::SetBack(Call_Read func)
{
    g_callRead = func;
}

int QTcpClient::SendInit()
{
    QByteArray temp;
    int length = 0;
    temp = QByteArray::number(m_type);
    m_data.append(temp);
    temp.clear();
    qDebug()<<"1: "<<m_data;
    char datac[4];
    Dec2Char(length,datac);
    m_data.append(datac[0]);
    m_data.append(datac[1]);
    m_data.append(datac[2]);
    m_data.append(datac[3]);
    qDebug()<<"2: "<<m_data.right(4);
    //发送
    if(tcpsocket == NULL)
    {
        m_data.clear();
        return -1;
    }
    if(tcpsocket->isOpen())
    {
        tcpsocket->write(m_data);
        m_data.clear();
        qDebug()<<"/////////////////";
        return 0;
    }
    else
    {
        LogMsg(tcpsocket->errorString());
        delete tcpsocket;
        tcpsocket =NULL;
        return -1;
    }
}

void QTcpClient::LogMsg(const QString &sLog)
{
    if(!g_logFile.isOpen())
    {
        g_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }

    QString slogger = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(sLog);
    QString sFinal = UTF8toGB2312(slogger);
    g_logFile.write(sFinal.toLocal8Bit());
    g_logFile.flush();
}

void QTcpClient::SetFilename(QString name)
{
    g_logFile.setFileName(name);
}

