#ifndef TCPSENDER_H
#define TCPSENDER_H

#include <QObject>
class QTcpSocket;
class TcpSender : public QObject
{
    Q_OBJECT
public:
    explicit TcpSender(QObject *parent = 0);
    
signals:
    
public slots:
public:
    void start();
protected:
    void timerEvent(QTimerEvent *);
private:
    //读取参数文件路径
    void readIniInfo();
    //连接服务器
    void connectToServer();
    //发送
    void send();
private:
    QString m_sDir;     //卡信息文件路径
    QString m_sIp;      //IP
    quint32 m_uPort;    //端口号
    QTcpSocket* m_pSocket;  //发送数据套接字
};

#endif // TCPSENDER_H
