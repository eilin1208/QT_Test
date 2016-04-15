#include "reqtime.h"
#include "CommonModule/MsgDefine.h"
#include "laneinfo.h"
const int MAX_BUFFER_SIZE = 512;

const int TIME_SVR_PORT = 13336;
ReqTime::ReqTime(QObject *parent) :
    QPausableThread(parent)
{
    m_bFirstRun = true;
    m_nMaxBufSize = MAX_BUFFER_SIZE;
    m_pDataBuffer = new char[MAX_BUFFER_SIZE];
    m_nBufLength = 0;
    m_iClient = NULL;
    setObjectName("ReqTimeThread");
}

ReqTime::~ReqTime()
{
    delete [] m_pDataBuffer;
}

bool ReqTime::ReqNewTime(const QString &sIp, bool bRunThread)
{
    //判断是否使用线程启动
    if(bRunThread)
    {
        m_sIp = sIp;

        if(this->isRunning() == false)
        {
            start();
            return true;
        }
        else
        {
            Resume();
            return true;
        }
    }
    else
    {
        m_sIp = sIp;
        RunOnce();
    }
    return true;
}

void ReqTime::RunOnce()
{
    TMsgHeadInfo msgHead;
    TMsgResponseInfo msgResp;
    memset(&msgHead, 0, sizeof(msgHead));
    FillMsgHead(58,1, getLaneInfo()->getOrgInfo().sOrgCode.toInt(),
                getLaneInfo()->getLaneCode(), getLaneInfo()->getOrgInfo().sOrgCode.toInt(),msgHead);
    memset(&msgResp, 0, sizeof(msgResp));
    if(m_iClient == NULL)
    {
        m_iClient = new QLaneSocket();
    }
    if(!m_iClient->isConnected())
    {
        if(!m_iClient->ConnectToserver(m_sIp, TIME_SVR_PORT, 2000))
        {
            this->Pause();
            return;
        }
    }
    m_iClient->WriteData((char*)&msgHead, sizeof(msgHead));
    memset(&msgHead, 0, sizeof(msgHead));
    int nHeadLen = sizeof(TMsgHeadInfo);
    if(!TryReadBuffer((char*)&msgHead, nHeadLen, 1000))
    {
        m_iClient->Close();
        this->Pause();
        return;
    }
    int nMsgId = (msgHead.cMsgID[0] - '0') * 10 + msgHead.cMsgID[1] - '0';
    int nMsgType = (msgHead.cMsgType[0] - '0') * 10 + msgHead.cMsgType[1] - '0';
    int nRcvLen = qFromBigEndian(msgHead.TotalLength);
    int nToRead = nRcvLen - nHeadLen;
    if(nMsgId != 58 && nMsgType != 2)
    {
        LogMsg("net", tr("校时错误，收到的报文类型错误:%1%2").arg(nMsgId).arg(nMsgType));
        this->Pause();
        return;
    }
    if(m_nMaxBufSize < nRcvLen)
    {
        delete [] m_pDataBuffer;
        m_nMaxBufSize = nRcvLen;
        m_pDataBuffer = new char[m_nMaxBufSize];
    }
    memcpy(m_pDataBuffer, &msgHead, sizeof(msgHead));
    m_nBufLength = sizeof(msgHead);
    char *pMsg = m_pDataBuffer;
    pMsg += sizeof(msgHead);
    LogMsg("net", tr("开始读返回报文，报文长度:%1").arg(nToRead));
    if(!TryReadBuffer(pMsg, nToRead, 1000))
    {
        LogMsg("net", tr("读取报文内容超时!"));
        this->Pause();
        return;
    }
    m_nBufLength = nRcvLen;
    pMsg = m_pDataBuffer;
    pMsg += sizeof(msgHead);
    QString sTemp = GetMD5ForBuffer(pMsg, nToRead);
    if(sTemp.toUpper().compare(QString::fromLocal8Bit(msgHead.VerifyCode, 32)) != 0)
    {
        LogMsg("net", tr("校时错误，校时报文校验码错误！"));
        this->Pause();
        return;
    }
    else
    {
        char szTime[14];
        memcpy(szTime, pMsg, 14);
        QDateTime newTime;
        if(!ConvertChar14ToSystemTime(&newTime, szTime))
        {
            this->Pause();
            return;
        }
        else
        {
            AdjustTime(newTime);
        }
    }
    //执行一次后暂停
    this->Pause();
}

bool ReqTime::TryReadBuffer(char *buffer, const int nByteLen, const int nTimeOut)
{
    int nReadCount = 0;
    int nRet = 0;
    if(nByteLen > 0 && buffer != NULL)
    {
        //QDebug() << tr("要读%1").arg(nByteLen);
        nRet = m_iClient->ReadData(buffer, nByteLen - nReadCount, nTimeOut);
        if(nRet != nByteLen)
        {
            return false;
        }
    }
    return true;
}

bool ReqTime::AdjustTime(const QDateTime &newTime)
{
    //QDebug() << "new time" << newTime;
    LogMsg("net", tr("系统校时，收费站返回时间：%1").arg(newTime.toString("yyyy-MM-dd hh:mm:ss")));
    QDateTime curTime = QDateTime::currentDateTime();

    if(!m_bFirstRun)
    {
        quint32 nSecond= qAbs((qint64)newTime.toTime_t() - (qint64)curTime.toTime_t());
        if(nSecond / 60/60 > 1)
        {
            LogMsg("net", tr("系统校时时间差异:%1小时,校时失败").arg(nSecond /60/60));
            return false;
        }

    }
    m_bFirstRun = false;
    if(!setSystemDateTime(newTime))
    {
        return false;
    }
    else
    {
        LogMsg("net", tr("校时成功，原时间:%1,校时后时间%2").arg(curTime.toString("yyyy-MM-dd hh:mm:ss"))
               .arg(newTime.toString("yyyy-MM-dd hh:mm:ss")));
    }
    return true;
}

ReqTime* getReqTime()
{
    static ReqTime req;
    return &req;
}
