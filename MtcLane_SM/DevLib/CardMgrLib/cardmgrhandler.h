#ifndef CARDMGRLISTENER_H
#define CARDMGRLISTENER_H

#include <QThread>
#include "QtSerialPort/QSerialPort"
#include "datadef.h"
#include <QMutex>
using namespace TNX;
class CardMgrHandler : public QThread
{
    Q_OBJECT
public:
    explicit CardMgrHandler(QObject *parent = 0);
    bool initImpl(const char* sCom, int nBps, QObject* pFunc);
    bool upCardImpl(int nMgrIndex);
    bool badCardImpl(int nMgrIndex);
    bool setCardCountImpl(int nMgrIndex, int nCount);
    void clearAllBadCardImpl();
    bool getBoxInfoImpl(int nBoxCode, int& nBoxId, int& nCount);
    bool getChannelInfoImpl(int nBoxCode,
                            int& nBoxId,
                            int& nCount,
                            bool& bHasCardOn,
                            char& curStatus);
signals:
    
public slots:
protected:
    void run();
private:
    //处理数据
    void dealData();
    //通知上层
    void notify(CMFRAMETYPE dataType, int nChannelIndex);
    //应答
    void response(int rsctl, CMFRAMETYPE dataType);

    //更新rsctl
    void updateRsctl();
    //发送数据
    bool sendFrame(SENDTYPE sendType, int nMgrIndex, int nCount);
private:
    //串口操作类
    QSerialPort *m_port;
    //线程停止标志
    volatile bool m_bTerm;
    //回调函数
    QObject* m_showObject;
    //频道信息
    ChannelInfo m_channels[CHANNELCOUNT];
    //当前获取到的rsctl
    char m_cRsctl;
    //串口接到的数据临时存放
    QByteArray m_tmpData;
    //互斥锁
    QMutex m_mutex;
    //上次发送的信息帧类型
    SENDTYPE m_sendType;
};

#endif // CARDMGRLISTENER_H
