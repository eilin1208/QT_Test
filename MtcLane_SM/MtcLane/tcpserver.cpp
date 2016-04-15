#include "tcpserver.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/ilogmsg.h"
#include "ParamModule/qparammgr.h"
#include "laneinfo.h"
#include "PacketSendModule/monclient.h"
#include "reqparam.h"
QString GetFileNameFromID(const quint32 dwImgId, const QString &sPicPath, const QString &sFileExt)
{
    QString sFileName = QObject::tr("%1%2")
            .arg(dwImgId >> 12 & 0x00ff, 2, 16, QLatin1Char('0'))
            .arg(dwImgId & 0x00000fff, 3, 16, QLatin1Char('0'));
    return QObject::tr("%1/%2/%3/%4/%5/%6")
            .arg(sPicPath)
            .arg(dwImgId >> 24, 2,10, QLatin1Char('0'))
            .arg(dwImgId >> 20 & 0x0f, 0, 16)
            .arg(dwImgId >> 12 & 0x00ff, 2, 16, QLatin1Char('0'))
            .arg(sFileName).arg(sFileExt);
}

TcpServer::TcpServer(QObject *parent) :
    QObject(parent),m_tcpSocket(NULL),m_pMsgRev(NULL)
{
}

void TcpServer::init(quint32 nPort)
{
    if(!m_tcpSvr.isListening())
    {
        connect(&m_tcpSvr, SIGNAL(newConnection()), this, SLOT(newConnection()));
        m_tcpSvr.listen(QHostAddress::Any, nPort);
    }
}

void TcpServer::DoMsg()
{
    TMsgHeadInfo msgHead;
    int msgId = 0;
    int msgType = 0;
    bool bDoResult = false;
    qMemSet(&msgHead, 0, sizeof msgHead);
    if(m_pMsgRev == NULL)
    {
        SendBasicResponseMsg(2);
    }

    memcpy(& msgHead, m_pMsgRev, sizeof msgHead);
    msgId = ArCharToInt(msgHead.cMsgID, 2);
    msgType = ArCharToInt(msgHead.cMsgType, 2);
    LogMsg("net", tr("接收到报文类型:%1, 报文ID:%2").arg(msgType).arg(msgId));
    if(msgId != 11)
    {
        LogMsg("net", tr("错误的报文类型，不处理!"));
        SendBasicResponseMsg(2);
        return;
    }
    switch(msgType)
    {
    case 3:
        bDoResult = DoRecvParamFile(msgHead);
        break;
    case 5:
        bDoResult = DoReceiveCommand(msgHead);
        break;
    case 6:
        bDoResult = DoReceiveMessage(msgHead);
        break;
    case 7:
        bDoResult = DoReceiveFile(msgHead);
        break;
    case 8:
        bDoResult = DoAdjustTime(msgHead);
        break;
    case 9:
        bDoResult = DoUpdateMonIP(msgHead);
        break;
    default:
        break;
    }
    if(bDoResult)
    {
        LogMsg("net", tr("参数接收成功"));
        SendBasicResponseMsg(1);
    }
    else
    {
        SendBasicResponseMsg(2);
    }
}

void TcpServer::SendBasicResponseMsg(quint8 nRes)
{
    TMsgResponseInfo responseInfo;
    int nPackLen = 0;
    int nTotalLen = 0;
    qMemSet(&responseInfo, 0, sizeof responseInfo);
    responseInfo.RecResult = nRes;
    FillMsgHead(99, nRes, getLaneInfo()->getOrgInfo().sOrgCode.toInt(),
                getLaneInfo()->getLaneCode(),
                getLaneInfo()->getOrgInfo().sOrgCode.toInt(),
                responseInfo.MsgHead);
    nTotalLen = sizeof(TMsgResponseInfo);
    nPackLen = sizeof(TMsgResponseInfo) - sizeof(TMsgHeadInfo);
    responseInfo.MsgHead.TotalLength = qFromBigEndian(nTotalLen);
    QString sTmp = GetMD5ForBuffer((char*)& responseInfo.RecResult, nPackLen);
    memcpy(responseInfo.MsgHead.VerifyCode, sTmp.toLocal8Bit().data(), sTmp.length());
    if(m_tcpSocket != NULL && m_tcpSocket->isOpen())
    {
        m_tcpSocket->write((char*)&responseInfo, sizeof(TMsgResponseInfo));
    }
}

bool TcpServer::DoRecvParamFile(const TMsgHeadInfo &msgHead)
{
    quint16 wFileNum = 0;
    char * pC;
    int nFileId = 0;
    bool bNew = false;
    CCfgFile cfg;
    if(m_pMsgRev == NULL)
    {
        return false;
    }
    pC = m_pMsgRev;
    pC += sizeof(TMsgHeadInfo);
    wFileNum = ArCharToInt(pC,2);
    LogMsg("net", tr("下发参数文件数量:%1").arg(wFileNum));
    if(wFileNum > 50 || wFileNum < 1)
    {
        LogMsg("lane", tr("下发参数文件数量错误:%1").arg(wFileNum));
        return false;
    }
    pC += 2;
    QString sTemp;

    for(int i = 0; i < wFileNum; i++)
    {
        nFileId = ArCharToInt(pC,2);
        pC += 2;
        sTemp = QString::fromLocal8Bit(pC, 14);
        bNew = (sTemp.compare("00000000000000") != 0);
        cfg = getParamMgr()->GetFileCfgById(nFileId);
        if(cfg < cfSysParaDic || cfg >= cfEnd)
        {
            continue;
        }
        getReqParam()->AskFile(cfg, bNew);
        pC += 14;
    }
    getReqParam()->AskFile(cfEnd, false);
    return true;
}

bool TcpServer::DoAdjustTime(const TMsgHeadInfo &msgHead)
{
    char tmptime[14];
    QDateTime tmpDateTime;
    QDateTime tmpSpan;
    QDateTime tmpOldTime;
    memset(tmptime, 0, 14);
    memcpy(tmptime, m_pMsgRev + sizeof(TMsgHeadInfo), 14);
    if(!ConvertChar14ToSystemTime(&tmpDateTime, tmptime))
    {
        LogMsg("net", tr("强制校时,时间装换错误!"));
        return false;
    }
    LogMsg("net", tr("强制校时,收费站时间:%1").arg(tmpDateTime.toString("yyyy-MM-dd hh:mm:ss")));
    tmpOldTime = QDateTime::currentDateTime();
    int timeSpan = qAbs(tmpOldTime.toTime_t() - tmpDateTime.toTime_t());
    if(timeSpan > 5 * 24 * 60* 60)
    {
        LogMsg("net", tr("强制校时时间差异:%1,校时失败").arg(timeSpan / 3600 / 24));
        return false;
    }
    //设置系统时间
    if(!setSystemDateTime(tmpDateTime))
    {
        LogMsg("net", tr("强制校时失败!"));
        return false;
    }
    else
    {
        LogMsg("net", tr("强制校时成功,原时间:%1 校时后时间:%2").arg(tmpOldTime.toString("yyyyMMddhhmmss"))
               .arg(tmpDateTime.toString("yyyyMMddhhmmss")));
    }
    return true;
}

bool TcpServer::DoReceiveFile(const TMsgHeadInfo &msgHead)
{
    int nHeadLen = 0;
    TRecvFileInfo recvFileInfo;
    QString sTmpDir;
    QString sTmpFile;
    qint64 nWriten = 0;
    nHeadLen = sizeof(TMsgHeadInfo);
    memset(&recvFileInfo, 0, sizeof(recvFileInfo));
    memcpy(&recvFileInfo, m_pMsgRev + nHeadLen, 132);
    if(recvFileInfo.FileLen <= 0)
    {
        LogMsg("net", tr("接收到的文件长度为0,不予处理!"));
        return false;
    }

    LogMsg("net", tr("接收到文件:%1").arg(GB2312toUtf8(recvFileInfo.FileName)));
    sTmpDir = "./update";
    sTmpFile = GB2312toUtf8(recvFileInfo.FileName);
    QDir dir;
    if(!dir.exists(sTmpDir))
    {
        if(!ForceDirectories(sTmpDir.toLocal8Bit().data()))
        {
            LogMsg("net", tr("创建备份文件目录失败!"));
            return false;
        }
    }
    sTmpDir = sTmpDir + "/" + sTmpFile;
    QFile tmpFile(sTmpDir);
    if(!tmpFile.open(QIODevice::ReadWrite))
    {
        LogMsg("net", tr("创建文件:%1失败").arg(sTmpDir));
        return false;
    }

    recvFileInfo.FileContent = m_pMsgRev + nHeadLen + 132;
    nWriten = tmpFile.write(recvFileInfo.FileContent, recvFileInfo.FileLen);
    tmpFile.close();
#ifndef Q_OS_WIN
    QProcess::execute("sync");
#endif
    if(nWriten == -1 || nWriten != recvFileInfo.FileLen)
    {
        LogMsg("net", tr("保存文件:%1 失败").arg(sTmpDir));
        return false;
    }
    return true;
}

bool TcpServer::DoReceiveCommand(const TMsgHeadInfo &msgHead)
{
    int nHeadLen = 0;
    char command;
    nHeadLen = sizeof(msgHead);
    command = m_pMsgRev[nHeadLen];
    TWorkToDoAfterClose tmpCommand;
    switch(command)
    {
    case '1':
        LogMsg("net",tr("重新启动计算机"));
        tmpCommand = wcReboot;
        break;
    case '2':
        LogMsg("net", tr("关闭计算机"));
        tmpCommand = wcShutDown;
        break;
    case '3':
        LogMsg("net", tr("重新启动程序"));
        tmpCommand = wcRestart;
        break;
    case '4':
        LogMsg("net", tr("关闭程序"));
        tmpCommand = wcNone;
        break;
    default:
        LogMsg("net", tr("未知的远程指令"));
        return false;
    }
    return true;
}

bool TcpServer::DoReceiveMessage(const TMsgHeadInfo &msgHead)
{
    TJKMsgInfo msgInfo;
    QString sSaveFile;
    memset(&msgInfo, 0, sizeof(TJKMsgInfo));
    memcpy(&msgInfo, m_pMsgRev, sizeof(msgInfo));
    QString MSGSAVEPATH = "./msg";
    QDir dir;
    if(!dir.exists(MSGSAVEPATH.toLocal8Bit().data()))
    {
        if(!ForceDirectories(MSGSAVEPATH.toLocal8Bit().data()))
        {
            LogMsg("net", tr("无法创建公告信息目录%1!").arg(MSGSAVEPATH));
            return false;
        }

    }
    sSaveFile = tr("%1/save.msg").arg(MSGSAVEPATH);
    if(QFile::exists(sSaveFile))
    {
        QFile::remove(sSaveFile);
    }
    QFile iTmpFile(sSaveFile);
    if(!iTmpFile.open(QIODevice::ReadWrite))
    {
        return false;
    }
    qint64 nWriten = iTmpFile.write((char*)&msgInfo, sizeof(msgInfo));
    if(nWriten == -1 || nWriten != sizeof(msgInfo))
    {
        return false;
    }
    return true;
}

bool TcpServer::DoUpdateMonIP(const TMsgHeadInfo &msgHead)
{
    QSettings settings("./Lane.bak", QSettings::IniFormat);
    settings.setValue("StationInfo/MonSrvIP", m_sPeerIp);
    //TODO:更改monip
    getMon()->WriteServerIP(m_sPeerIp);
    return true;
}


void TcpServer::newConnection()
{
    //如果现在已经有一个连接了，则不会再次接受
    if(m_tcpSocket != NULL)
    {
        socketClose();
    }
    if(m_tcpSocket == NULL)
    {
        m_tcpSocket = m_tcpSvr.nextPendingConnection();
        m_sPeerIp = m_tcpSocket->peerAddress().toString();
        connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(byteWritten(qint64)));
        connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(socketClose()));
    }
}

void TcpServer::readyRead()
{
    const int nMsgHeadLen = sizeof(TMsgHeadInfo);
    TMsgHeadInfo MsgHeadInfo;
    quint32 nMsgLen;
    char * Buffer  =NULL;
    QString VerifyCode;
    QString strError;
    qMemSet(&MsgHeadInfo,0, sizeof(MsgHeadInfo));
    int nRet = m_tcpSocket->read((char*)&MsgHeadInfo, nMsgHeadLen);
    if(nRet == -1)
    {
        return;
    }
    nMsgLen = MsgHeadInfo.TotalLength;
    if(nMsgLen > 16 * 1024* 1024)
    {
        LogMsg("net", tr("报文长度=%1,无法处理").arg(nMsgLen));
        return;
    }

    Buffer = new char[nMsgLen];

    memcpy(Buffer, &MsgHeadInfo, nMsgHeadLen);
    nRet = m_tcpSocket->read(Buffer + nMsgHeadLen, nMsgLen - nMsgHeadLen);
    if(nRet == -1)
    {
        delete [] Buffer;
        return;
    }
    VerifyCode = GetMD5ForBuffer(Buffer + nMsgHeadLen, nMsgLen - nMsgHeadLen);
    nRet =  VerifyCode.compare(QString::fromLocal8Bit(MsgHeadInfo.VerifyCode, 32), Qt::CaseInsensitive);
    if(nRet != 0)
    {
        LogMsg("net", tr("站级监控服务主动下发报文校验失败"));
        delete [] Buffer;

        return;
    }
    m_pMsgRev = Buffer;
    this->DoMsg();
    delete [] Buffer;
}

void TcpServer::byteWritten(qint64 nLen)
{
}

void TcpServer::socketClose()
{
    disconnect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    disconnect(m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(byteWritten(qint64)));
    disconnect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(socketClose()));
    m_tcpSocket = NULL;
}

TcpServer* getLaneMsgServer()
{
    static TcpServer server;
    return &server;
}
