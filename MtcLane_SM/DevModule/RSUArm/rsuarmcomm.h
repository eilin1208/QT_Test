/*****************************************
*setportsetting  open  close
*****************************************/

#ifndef RSUARMCOMM_H
#define RSUARMCOMM_H

#include <QObject>
#include <QTimer>
#include <QThread>

#include "QtSerialPort/QSerialPort"

using namespace TNX;

class RsuArmComm : public QObject
{
    Q_OBJECT
public:
    explicit RsuArmComm(const QString & portName, QObject *parent = 0);
    RsuArmComm(QObject *parent = 0);
    ~RsuArmComm();
    //打开串口
    bool openComm();
    bool isOpenComm();
    //关闭串口
    void closeComm();
    //设置串口参数
    bool setPortSettings(const QPortSettings &portSettings);
    bool setPortName(const QString & portName);
    //获取接收到的帧数据
    QByteArray getRecivedFrameData();
    //帧的完整性校验(异或值校验)
    bool FrameBCCPass(QByteArray data);
public slots:
    //读取串口数据
    void readCommData();
    //发送串口数据
    qint64 sendFrameData(QByteArray &data);
    qint64 sendFrameData(char *data,int size);
    void printCommData();
    //设置是否显示调试信息
    void SetDebugflag(bool flag);
private:
    //初始化
    void initParm();
    //set time interval of data recived
    void setDataRecivedTimeInterval_ms(int ms =1);
    //报文截取重组
    bool readFrameData();
    //显示调试信息
    void DebugMessage(QString message);
    //输入转换
    void InputTrans(QByteArray &message);
    //输出转换
    void OutputTrans(QByteArray &message);
    //开始结束标记位的截取
    void InterceptFrameData(QByteArray &message);
    //发送数据前对帧封装
    void PackageFrameData(QByteArray &message);
    //计算异或
    quint8 GetXOR(const QString &str);
    quint8 GetXOR(const char *Buff,int nLen);
public:
    QSerialPort *pSerialPort;
    //串口设置接口
    QPortSettings portSettings;
private:
    QTimer *reciveDataTimer;
    QByteArray m_RecivedRawData;
    QByteArray m_RecivedFrameData;
    bool bDebugFlag;
signals:
    void readyReadFrameData(QByteArray frameData);
    //测试用信号
    void sendingFrameData(QByteArray frameData);
};
//获取程序唯一实例
RsuArmComm *GetRsuArmCommInstance();
#endif // RSUARMCOMM_H
