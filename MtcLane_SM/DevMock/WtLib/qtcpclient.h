#ifndef QTCPCLIENT_H
#define QTCPCLIENT_H
#include <QTcpSocket>
#include <QObject>
#include <QTextCodec>
#include<QBuffer>
#include<QDataStream>
#include <QFile>
#include <QDateTime>

class QTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit QTcpClient(QObject *parent = 0);
    void ConnecttoServer();
    bool isConnected();
    void SendMessage();    
    QString IPaddress;
signals:
    void HasData();
public slots:
    void DisConnected();
    void readMessage();
private:
    QTcpSocket *tcpsocket;
    DataFrame tcpframe;
};
void LogMsg(const QString& sLog);
extern QByteArray tcpindata;
#endif // QTCPCLIENT_H
