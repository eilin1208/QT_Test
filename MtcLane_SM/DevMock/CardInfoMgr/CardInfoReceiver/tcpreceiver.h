#ifndef TCPRECEIVER_H
#define TCPRECEIVER_H

#include <QObject>
class QTcpServer;
class QTcpSocket;
class TcpReceiver : public QObject
{
    Q_OBJECT
public:
    explicit TcpReceiver(QObject *parent = 0);
    
signals:
    
public slots:

public:
    void start();
private slots:
    void newConnect();
    void readNewData();
private:
    //读取参数文件路径
    void readIniInfo();
private:
    QString m_sDir;     //卡信息文件路径
    QString m_sIp;      //IP
    quint16 m_uPort;    //端口号
    QTcpServer* m_pServer;  //发送数据套接字
    QTcpSocket* m_pSocket;  //接收套接字
    quint64 m_nFileSize;
    QByteArray m_fileContent;
    QByteArray m_sMd5;
    quint64 m_uTmpCount;    //整个报文接收到多少byte
};

#endif // TCPRECEIVER_H
