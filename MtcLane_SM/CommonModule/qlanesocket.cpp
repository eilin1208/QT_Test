#include "qlanesocket.h"
#include <QHostAddress>

QLaneSocket::QLaneSocket():m_bConnected(false)
{
    //QDebug() << tr("socket构建一次");
}
QLaneSocket* getLaneSocket()
{
    static QLaneSocket laneSocket;
    return &laneSocket;
}
bool QLaneSocket::ConnectToserver(const QString &sHost,unsigned short nPort, int nTimeOut)
{
    QHostAddress hAddress(sHost);
    if(hAddress.isNull() || nPort <= 0||hAddress==QHostAddress::Broadcast)
    {
        //写日志
        LogMsg("net", tr("IP地址或者端口号传递错误，IP地址为：%1，端口号为%2")
               .arg(hAddress.toString()).arg(nPort));
        m_bConnected=false;
        return false;
    }

    //连接
    socket.connectToHost(hAddress, nPort,QIODevice::ReadWrite);

    //阻塞等待连接成功，最大延时30秒
    if(socket.waitForConnected(nTimeOut) == false)
    {
        //写日志
        LogMsg("net", tr("socket连接失败,错误信息为：%1").arg(socket.errorString()));
        Close();
        m_bConnected=false;
        return false;
    }
    m_bConnected=true;
    return true;
}

qint64 QLaneSocket::ReadData(char *buf, qint64 length,int timeout)
{

    if(isConnected() == false)
    {
        return false;
    }
    //读取
    if(socket.bytesAvailable() < length)
    {
        if(socket.waitForReadyRead(timeout) == false)
        {
            //写日志
            LogMsg("lane", tr("读取失败,错误信息为：%1").arg(socket.errorString()));
            //QDebug() << GetLastError();
            return -1;
        }
//        else
//        {
//            if(socket.bytesAvailable() < length)
//            {
//                return -1;
//            }
//        }
    }
    qint64 readLength = socket.read(buf, length);
    //Close();
    return readLength;
}

qint64 QLaneSocket::WriteData(char *buf, qint64 length)
{
    if(isConnected() == false)
    {
        //写日志
        return false;
    }

    qint64 writeLength = socket.write(buf, length);
    if(socket.waitForBytesWritten(10000) == false)
    {
        //写日志
        LogMsg("lane", tr("写入失败，错误原因为：%1").arg(socket.errorString()));
        return -1;
    }
    return writeLength;
}

void QLaneSocket::Close()
{
    socket.close();
    m_bConnected=false;
}

QString QLaneSocket::GetLastError()
{
    return socket.errorString();
}

qint64 QLaneSocket::ByteCanRead()
{
    return socket.bytesAvailable();
}
