#ifndef QLANESOCKET_H
#define QLANESOCKET_H
#include <QtGui>
#include <QHostAddress>
#include <QTcpSocket>
#include "ilogmsg.h"
class QLaneSocket : public QObject
{
    Q_OBJECT
public:
    QLaneSocket();
    //建立连接
    bool ConnectToserver(const QString &sHost,unsigned short nPort, int nTimeOut = 1000);
    //读取数据
    qint64 ReadData(char * buf, qint64 length,int timeout);
    //发送数据
    qint64 WriteData(char * buf, qint64 length);
    //关闭
    void Close();
    //是否已连接
    bool isConnected(){return socket.isOpen() && socket.state() == QAbstractSocket::ConnectedState;}
    //获取最后一条错误
    QString GetLastError();
    //多少字节可读
    qint64 ByteCanRead();
protected:
    QTcpSocket socket;
    bool m_bConnected;
           //超时时间
};
QLaneSocket* getLaneSocket();
#endif // QLANESOCKET_H
