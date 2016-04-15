#include "tcpsender.h"
#include <QSettings>
#include <QTcpSocket>
#include <QDir>
#include <QFileInfoList>
#include <QCryptographicHash>
#include <QDebug>
TcpSender::TcpSender(QObject *parent) :
    QObject(parent),m_uPort(0),m_pSocket(new QTcpSocket)
{
}

void TcpSender::start()
{
    startTimer(1000);
}

void TcpSender::readIniInfo()
{
    QSettings settings("./Card.ini", QSettings::IniFormat);
    m_sDir = settings.value("CardInfo/Dir").toString();
    m_sIp = settings.value("CardInfo/IP").toString();
    m_uPort = settings.value("CardInfo/Port").toUInt();
}

void TcpSender::connectToServer()
{
    //连接服务器
    bool bRet = false;
    while(m_pSocket->state() != QTcpSocket::ConnectedState)
    {
        //重新加载INI文件
        readIniInfo();
        qDebug() << "read ini info" << m_sDir << m_sIp << m_uPort;

        //连接服务器
        m_pSocket->connectToHost(m_sIp, m_uPort);
        //10秒重新连接一次
        bRet = m_pSocket->waitForConnected(10000);
        qDebug() << "connect server..." << bRet;

        if(!bRet)
        {
            delete m_pSocket;
            m_pSocket = new QTcpSocket();
        }
    }
}

void TcpSender::send()
{
    readIniInfo();
    //获得目录下的文件数
    QDir dir(m_sDir);
    QFileInfoList fileList;
    QFile file;
    quint64 nFileSize = 0;
    QByteArray fileContent;
    bool bRet = false;

    fileList = dir.entryInfoList(QDir::Files, QDir::Time);
    qDebug() << "file count " << fileList.count();
    for(int i  = 0; i < fileList.count(); i++)
    {
        qDebug() << fileList.at(i).fileName();
    }
    if(fileList.count() >= 2)
    {
        //先检查网络状态，如无未连接，则尝试连接
        connectToServer();
        file.setFileName(m_sDir + fileList.first().fileName());
        if(file.open(QIODevice::ReadOnly))
        {
            nFileSize = file.size();
            //附加长度
            fileContent.append((const char*)&nFileSize, 8);
            //附加内容
            fileContent.append(file.readAll());
            //附加校验
            fileContent.append(QCryptographicHash::hash(fileContent, QCryptographicHash::Md5 ));
            //发送数据
            m_pSocket->write(fileContent);
            bRet = m_pSocket->waitForBytesWritten(3000);
            qDebug() << "write result" << bRet;
            if(bRet)
            {
                //删除文件
                file.close();
                file.remove();
            }
            fileContent.clear();
        }
        else
        {
            qDebug() << "open file fail";
        }
    }
}

void TcpSender::timerEvent(QTimerEvent *event)
{
    send();
}
