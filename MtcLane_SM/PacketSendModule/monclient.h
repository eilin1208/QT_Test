#ifndef MONCLIENT_H
#define MONCLIENT_H

#include <QObject>
#include "CommonModule/qpausablethread.h"
#include "CommonModule/qlanesocket.h"
#include "CommonModule/MsgDefine.h"
#include "msgsendfunc.h"
#include <QMutex>
#include <QQueue>
const int SEND_INTERVAL = 50;
const int WAIT_TIME = 2000;
const int CONNECT_TIME = 2000;

enum THKResult
{
    hrAccept,
    hrRefuse,
    hrTimeout,
    hrFail
};

struct THKResultInfo
{
    THKResult Response;
    QString AuthID;
    QString AuthName;
public:
    void clear()
    {
        AuthID = "";
        AuthName = "";
    }
};
#pragma pack(push,1)
struct TMonData
{
    double ATime;
    bool NeedHK;
    char * pBuf;
    quint32 ASize;

};

#pragma pack(pop)
Q_DECLARE_METATYPE(THKResultInfo)
class MonClient : public QPausableThread
{
    Q_OBJECT
public:
    explicit MonClient(QObject *parent = 0);
    virtual ~MonClient();
signals:
    void OnHKResponse(THKResultInfo info);
public slots:
public:
    //发送数据
    void SendData(TMsgToSend& AData);
    //取消发送
    void CancelHK();
    void SetLaneId(int nLaneId)
    {
        m_nLaneId = nLaneId;
    }
    int GetLaneId()
    {
        return m_nLaneId;
    }
    void SetStaId(int nStaId)
    {
        m_nStaId = nStaId;
    }
    int GetStaId()
    {
        return m_nStaId;
    }
    //设置监控服务器IP地址
    void WriteServerIP(const QString& Value);
    //设置监控服务器端口
    void WriteServerPort(const quint16 Value);

protected:
    void RunOnce();
private:
    bool PutData(char *ABuf, int ALen, bool NeedHK);
    //发送数据
    void BeginSendData(const TMonData& AData);
    //判断连接是否正常，如已断开需要重新连接
    bool KeepConnect();
    //取消数据发送
    TMonData* PopData();
    //等待监控应答
    void WaitHKResponse();
    //读取数据
    bool TryReadBuffer(void* ABuffer, const int AByteCount, const int ATimeout);
    //检查连接
    bool CheckConnection();
    //初始化监控应答信息
    void InitResponseInfo(THKResultInfo& ResponseInfo);
private:
    QLaneSocket* m_socket;
    //监控应答信息
    THKResultInfo m_responseInfo;
    //需要回控信息
    bool m_bCanceled;
    //车道代码
    int m_nLaneId;
    //收费站代码
    int m_nStaId;
    //数据锁
    QMutex m_mutex;
    //数据
    QQueue<TMonData*> m_data;
    //IP
    QString m_sIp;
    //Port
    quint16 m_nPort;
};
MonClient* getMon();
#endif // MONCLIENT_H
