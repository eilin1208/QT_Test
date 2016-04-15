#include "tcpreceiver.h"
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <QCryptographicHash>
#include <QHostAddress>
#include <QDebug>
TcpReceiver::TcpReceiver(QObject *parent) :
    QObject(parent), m_uPort(0), m_pServer(new QTcpServer), m_pSocket(NULL), m_nFileSize(0)
{
}

void TcpReceiver::start()
{
    readIniInfo();
    connect(m_pServer, SIGNAL(newConnection()), this, SLOT(newConnect()));
    m_pServer->listen(QHostAddress::Any, m_uPort);
    qDebug() << "read ini info" << m_sDir << m_sIp << m_uPort;

}

void TcpReceiver::newConnect()
{
    QTcpSocket* tmpSocket = m_pServer->nextPendingConnection();
    //当接收到新的套接字连接时，删除原来的
    if(m_pSocket != tmpSocket)
    {
        if(m_pSocket != NULL)
        {
            disconnect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readNewData()));
            m_pSocket->close();
            delete m_pSocket;
        }
        m_pSocket = tmpSocket;
        connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readNewData()));
    }
}

void TcpReceiver::readNewData()
{
    QFile file;
    qDebug() << "has read new data...";
    int i = 0;
    while(m_pSocket->bytesAvailable() > 0)
    {
        qDebug() << "begin to handle the" << i + 1 << "data";
        if(m_pSocket->bytesAvailable() >= 8)
        {
            //读取长度
            if(m_nFileSize == 0)
            {
                m_pSocket->read((char*)&m_nFileSize, 8);
                qDebug() << "file size is" << m_nFileSize;
            }
            //读取文件内容
            if(m_fileContent.count() < m_nFileSize)
            {
                if(m_nFileSize - m_fileContent.count() > m_pSocket->bytesAvailable())
                {
                    m_fileContent.append(m_pSocket->readAll());
                }
                else
                {
                    m_fileContent.append(m_pSocket->read(m_nFileSize - m_fileContent.count()));
                }
                qDebug() << "file content has receive" << m_fileContent.count();
            }

            //判断读取内容是否完成
            if(m_fileContent.count() < m_nFileSize)
            {
                continue;
            }

            //当内容加载完成时，加载md5
            if(m_sMd5.count() < 16)
            {
                m_sMd5.append(m_pSocket->read(16 - m_sMd5.count()));
            }

            if(m_sMd5.count() == 16)
            {
                qDebug() <<"receive md5 is" << m_sMd5.toHex();
                //报文全部读取,开始转存
                QByteArray tmpMd5;
                tmpMd5.append((const char*)&m_nFileSize, 8);
                tmpMd5.append(m_fileContent);
                qDebug() << QCryptographicHash::hash(tmpMd5, QCryptographicHash::Md5 ).toHex();
                if(m_sMd5 == QCryptographicHash::hash(tmpMd5, QCryptographicHash::Md5 ))
                {
                    file.setFileName(m_sDir + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
                    if(file.open(QIODevice::WriteOnly))
                    {
                        file.write(m_fileContent);
                    }
                    file.close();
                }
                m_nFileSize = 0;
                m_fileContent.clear();
                m_sMd5.clear();
            }
        }
    }
}

void TcpReceiver::readIniInfo()
{
    QSettings settings("./Card.ini", QSettings::IniFormat);
    m_sDir = settings.value("CardInfo/Dir").toString();
    m_sIp = settings.value("CardInfo/IP").toString();
    m_uPort = settings.value("CardInfo/Port").toUInt();
}
