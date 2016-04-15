#ifndef PORTCOMM_H
#define PORTCOMM_H
#include <QThread>
#include <QObject>

enum BStatus
{
    B0_frame = 0x01,
    B2_frame = 0x02,
    B3_frame = 0x03,
    B4_frame = 0x04,
    B5_frame = 0x05,
    B2_wait = 0x08
};
class PortComm : public QThread
{
    Q_OBJECT
public:
    explicit PortComm(QObject *parent = 0);

    void StartRsu();//打开RSU

    void CloseRsu();//关闭RSU

    void WriteData(const char* datac);//从INI文件读，并写到串口

    void SetStatus(BStatus status);

    BStatus GetStatus();

    QString ReadStr();

    QString m_str;

    BStatus m_status;

    void getFrame();//得到读到的数据

signals:

    void StatusChange();//状态改变时

    void ReadyString();//发送读取字符串的信号

public slots:
    void SendFrame();//发送帧

protected:
    void run();

private:
    QByteArray C1_data;
    QByteArray C2_data;
    QByteArray C3_data;
    QByteArray C0_data;
    QByteArray C6_data;    
    char* m_port;
    int m_dealnum;
    int m_b4deal;
    const char* m_Name;

};
void LogMsg(const QString& sLog);
#endif // PORTCOMM_H
