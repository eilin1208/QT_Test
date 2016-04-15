#ifndef QTCPCLIENT_H
#define QTCPCLIENT_H
#include <QTcpSocket>
#include <QObject>
#include <QTextCodec>
#include <QFile>
#include <QDateTime>
#include <QTime>
typedef void (*Call_Read)(QByteArray &data);
enum DEVTYPE
{
    DEVBEGIN,
    //这里写数据类型
    DEV_IO = 0x01,
    DEV_FD = 0x02,
    DEV_PRT = 0x03,
    DEV_VPR = 0x04,
    DEV_WT = 0x05,
    DEVEND
};
class QTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit QTcpClient(QObject *parent = 0);
    //连接socket
    bool ConnecttoServer(const QString& ip);

    void LogMsg(const QString& sLog);

    bool isConnected();

    //length代表data长度，非整个报文长度
    int SendMessage(int length,const char* data);

    int SendInit();

    //断开连接
    void Disconnected();

    //异或
    quint8 GetXOR(int nLen ,const char* Buff);

    void SetType(int type);

    void SetBack(Call_Read func);

    void SetFilename(QString name);
signals:
    void ReadStr(QByteArray& data);

public slots:

    void readMessage();
private:
    QTcpSocket *tcpsocket;
    QByteArray m_data;
    int m_type;
    Call_Read g_callRead;
    QFile g_logFile;
    QDateTime m_time;
};
void Dec2Char(int num,char data[4]);
int  Char2Dec(char* datac);
#endif // QTCPCLIENT_H
