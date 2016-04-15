#ifndef MSGRECEIVER_H
#define MSGRECEIVER_H
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDateTime>
class MsgReceiver : public QObject
{
    Q_OBJECT
public:
    explicit MsgReceiver(QObject *parent = 0);
    
signals:
    void ReadyShow();

    void ReadyStr(int &choice , QString &string);

public slots:
    void beginRead();

    void beginSend();

public:
    void setSocket(QTcpSocket* socket);

    quint8 GetXOR(const char *Buff,int nLen);

    void GetBuff(QByteArray &databy);

    QByteArray getResult();
    //选择在哪个TextEdit中显示数据
    void GetEditText(int type);

private:
    QTcpSocket* m_socket;
    QByteArray m_buffer;
    int f_type;
    int f_length;
    quint8 f_crc;
    QByteArray f_buf;
    QString m_str;
    QString m_readdata;
    int m_choice;
    QDateTime m_time;
    int socketDescriptor;
};
#endif // MSGRECEIVER_H
