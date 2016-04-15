#include "reqparam.h"
#include "CommonModule/MsgDefine.h"
#include "CommonModule/laneutils_new.h"
#include "laneinfo.h"
const int MAX_BUFFER_SIZE = 1024*1024*5;
ReqParam::ReqParam(QObject *parent) :
    QThread(parent)
{
    m_FMaxBufSize = MAX_BUFFER_SIZE;
    m_FDataBuffer = new char[m_FMaxBufSize];
    m_FBufLength = 0;
    m_bTer = true;
    m_client = NULL;
    m_nAskTimerId = -1;
    m_nReLoadTimerId = -1;
    m_bNew = false;
    m_bRet = false;
    m_cfgFile = 0;

    //    connect(this, SIGNAL(ShowDownloadedResult(quint32,bool,bool)),
    //            this, SLOT(DownLoadFinish(quint32,bool,bool)));
    connect(this,SIGNAL(finished()), this, SLOT(DownLoadFinish()));
    //下载连接
}
ReqParam::~ReqParam()
{
    ////qDebug() << "release thread req param...";
    //删除请求数据
    while(this->isRunning())
    {
        m_bTer = false;
    }
    m_mutex.lock();
    while(m_listQuery.count() != 0)
    {
        delete m_listQuery.dequeue();
    }
    if(m_FDataBuffer != NULL)
    {
        delete m_FDataBuffer;
        m_FDataBuffer = NULL;
    }
    m_mutex.unlock();
}

void ReqParam::SetHostInfo(const QString &sHost, int nPort)
{
    m_sIp = sHost;
    m_nPort = nPort;
}

bool ReqParam::AddTask(TReqParam *askReq)
{
    ////qDebug() << "mutex" << "addtask";
    QMutexLocker locker(&m_mutex);
    m_listQuery.append(askReq);
    return true;
}

bool ReqParam::DownLoadParam(int cfgFile, bool bNew)
{
    //如果有下载新版文件，说明属于全部下载，不需要等待

    if(bNew)
    {
        m_bNeedWait = false;
    }
    QString sFileInfo;
    int nFileNum = 1;
    if(!getParamMgr()->GetFileInfoStr((CCfgFile)cfgFile, sFileInfo, bNew))
    {
        return false;
    }

    return ReqParamFile((CCfgFile)cfgFile, nFileNum, sFileInfo, bNew);
}

void ReqParam::Term()
{
    //中断线程
    m_bTer = false;
    this->wait();
    //停止定时器
    if(m_nAskTimerId != -1)
    {
        killTimer(m_nAskTimerId);
        qDebug("83 kill ask timer %x", m_nAskTimerId);

        m_nAskTimerId = -1;
    }
    if(m_nReLoadTimerId != -1)
    {
        killTimer(m_nReLoadTimerId);
        qDebug("90 kill timer %x", m_nReLoadTimerId);

        m_nReLoadTimerId = -1;
    }
    //清空队列
    m_listQuery.clear();
    m_listNeed.clear();
    //重新启动下载线程
    m_bTer = true;
    this->start();
}

bool ReqParam::DownLoadParam( TReqParam *task)
{
    TMsgHeadInfo msgHead;
    TMsgResponseInfo msgResp;
    quint32 nRcvLen = 0;
    quint32 nToRead = 0;
    quint32 nHeadLen = 0;
    quint8* pMsg;
    int nRet = 0;
    int nMsgID = 0;
    QString sMd5;
    bool bOk = false;
    memset(&msgResp, 0, sizeof(msgResp));
    if(m_client == NULL)
    {
        m_client = new QLaneSocket();
        bOk = m_client->ConnectToserver(m_sIp, m_nPort, 100);
    }
    bOk = m_client->isConnected();
    if(!bOk)
    {
        m_client->ConnectToserver(m_sIp, m_nPort, 100);
    }
    bool bTmpNew = false;
    if(!m_listNeed.isEmpty())
    {
        bTmpNew = m_listNeed.first().bNew;
    }
    if(bOk == false)
    {
        LogMsg("net", tr("参数下载连接%1:%2失败,原因:%3").arg(m_sIp).arg(m_nPort).arg(m_client->GetLastError()));
        emit ShowDownloadedResult(task->m_nFileId, bTmpNew, false);
        m_cfgFile = task->m_nFileId;
        m_bNew = bTmpNew;
        m_bRet = false;
        return false;
    }
    LogMsg("net", tr("参数下载连接%1:%2成功!").arg(m_sIp).arg(m_nPort));
    m_client->WriteData((char*)task->m_sendBuffer, task->m_dwSendLen );
    memset(&msgHead, 0, sizeof(TMsgHeadInfo));
    nHeadLen = sizeof(TMsgHeadInfo);
    LogMsg("net", tr("开始读参数服务返回报文头"));
    //nRet = m_client.ReadData((char* )&msgHead, nHeadLen);
    if(!TryReadBuffer((char*)&msgHead, nHeadLen, task->m_dwTimeOut))
    {
        LogMsg("net", tr("读取报文头超时!"));
        emit ShowDownloadedResult(task->m_nFileId, bTmpNew, false);
        m_cfgFile = task->m_nFileId;
        m_bNew = bTmpNew;
        m_bRet = false;
        return false;
    }
    nMsgID = (msgHead.cMsgID[0] - '0') * 10 + msgHead.cMsgID[1] - '0';
    nRcvLen = qFromBigEndian(msgHead.TotalLength);
    nToRead = nRcvLen - nHeadLen;
    LogMsg("net", tr("查询返回%1，长度%2").arg(nMsgID).arg(nRcvLen));
    if(m_FMaxBufSize < nRcvLen)
    {
        delete [] m_FDataBuffer;
        m_FMaxBufSize = nRcvLen;
        m_FDataBuffer = new char[m_FMaxBufSize];
    }
    memcpy(m_FDataBuffer, &msgHead, sizeof(msgHead));
    m_FBufLength = sizeof(msgHead);
    pMsg = (quint8*)m_FDataBuffer;
    pMsg += sizeof(msgHead);
    LogMsg("net", tr("开始读返回报文，报文长度:%1").arg(nToRead));
    //nRet = m_client.ReadData(pMsg, nToRead, task->m_dwTimeOut);
    if(!TryReadBuffer((char*)pMsg, nToRead, task->m_dwTimeOut))
    {
        LogMsg("net", tr("读取报文内容超时"));
        emit ShowDownloadedResult(task->m_nFileId, bTmpNew, false);
        m_cfgFile = task->m_nFileId;
        m_bNew = bTmpNew;
        m_bRet = false;
        return false;
    }

    m_FBufLength = nRcvLen;
    pMsg = (quint8*)m_FDataBuffer;
    pMsg += sizeof(msgHead);
    sMd5 = GetMD5ForBuffer((char*)pMsg, nToRead);
    bOk = false;
    //由于卡黑名单下载时存在md5校验不通过问题，因此先暂时注释掉
//    if(sMd5.toUpper().compare(QString::fromLocal8Bit(msgHead.VerifyCode, 32)) != 0)
//    {
//        LogMsg("net", tr("接收到下发参数指令，报文校验不通过"));
//        msgResp.RecResult = 2;
//    }
//    else
//    {
        if(SaveParamFile())
        {
            msgResp.RecResult = 1;
            bOk = true;
        }
        else
        {
            msgResp.RecResult = 2;
        }
//    }
    FillMsgHead(99, msgResp.RecResult, getLaneInfo()->getOrgInfo().sOrgCode.toInt(),
                getLaneInfo()->getLaneCode(),getLaneInfo()->getOrgInfo().sOrgCode.toInt(), msgResp.MsgHead);
    msgResp.WasteID = 0;
    msgResp.MsgHead.TotalLength = sizeof(msgResp);
    sMd5 = GetMD5ForBuffer((char*)&msgResp.WasteID, sizeof(msgResp) - sizeof(msgResp.MsgHead));
    memcpy(msgResp.MsgHead.VerifyCode, sMd5.toLocal8Bit().data(), 32);
    nRet = m_client->WriteData((char*)&msgResp, sizeof(msgResp));
    //下载完成，开启定时器再次下载数据
    //emit ShowDownloadedResult(task->m_nFileId, bTmpNew,bOk);
    m_cfgFile = task->m_nFileId;
    m_bNew = bTmpNew;
    m_bRet = bOk;
    if(nRet == -1)
    {
        LogMsg("net", tr("报文发送失败"));
        return false;
    }
    return bOk;
}

int ReqParam::GetTaskCount()
{
    ////qDebug() << "mutex" << "GetTaskCount";
    QMutexLocker locker(&m_mutex);
    return m_listQuery.count();
}

void ReqParam::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nAskTimerId)
    {
        AskParamFile();
    }
    if(event->timerId() == m_nReLoadTimerId)
    {
        ////qDebug() << "re load timer id";
        killTimer(m_nReLoadTimerId);
        m_nReLoadTimerId = -1;
        qDebug("226 kill timer %x", m_nReLoadTimerId);
        AskParamFile();
    }
}

void ReqParam::run()
{
    //while(!)
    int nTaskCount = 0;
    TReqParam* task = NULL;

    m_mutex.lock();
    nTaskCount = m_listQuery.count();
    if(nTaskCount > 0)
    {
        task = m_listQuery.first();
        m_listQuery.dequeue();
    }

    m_mutex.unlock();

    //将下载是否成功的信息发送出去
    //emit DownLoadFinish(task->m_nFileId, DownLoadParam(task));
    //qDebug() << "req run once " << nTaskCount;
    if(task != NULL)
    {
        DownLoadParam(task);
        delete task;
    }
}

bool ReqParam::TryReadBuffer(char *buffer, const int nByteCount, const int nTimeOut)
{
    int nWaitCount = 0;
    int nRet = 0;
    if(nByteCount > 0 && buffer != NULL)
    {
        ////qDebug()<< tr("要读%1").arg(nByteCount);
        while(nWaitCount < nByteCount)
        {
            nRet = m_client->ReadData(buffer, nByteCount, nTimeOut);
            if(nRet == -1)
            {
                LogMsg("net", tr("读取数据失败"));
                return false;
            }
            buffer += nRet;
            nWaitCount += nRet;
        }
        return nWaitCount == nByteCount;
    }
    return false;
}

bool ReqParam::SaveParamFile()
{
    TMsgHeadInfo* pMsgHead = 0;
    int nMsgId = 0;
    int nHeadLen = 0;
    bool bNewFile = false;
    bool isRealNew = false;
    quint16 wFileNum = 0;
    quint8* pB = NULL;
    int nLeft = 0;
    quint32 nFileLen = 0;
    int nFileId = 0;
    SParamFileHeadRaw fileHeadRow;
    SParamFileHead cfgFileHead;
    QBaseParamFile *baseParamFile;
    QString sFileName;
    QString sTmpFileName;
    pMsgHead = (TMsgHeadInfo*)m_FDataBuffer;
    nHeadLen = sizeof(TMsgHeadInfo);
    nMsgId = ArCharToInt(pMsgHead->cMsgID, 2);
    if(nMsgId == 99)
    {
        if(pMsgHead->cMsgType[1] == '1')
        {
            LogMsg("net", tr("参数下载返回9901"));
            return true;
        }
        else if(pMsgHead->cMsgType[1] == '2')
        {
            LogMsg("net", tr("参数返回9902"));
            return false;
        }
    }

    if((nMsgId != 68) && (nMsgId != 67))
    {
        return false;
    }
    bNewFile = nMsgId == 68;
    if(m_FBufLength < nHeadLen + 4)
    {
        return false;
    }

    pB = (quint8*)m_FDataBuffer;
    pB += nHeadLen;
    wFileNum = 0;
    memcpy(&wFileNum, pB, 2);
    wFileNum = qFromBigEndian(wFileNum);
    if(wFileNum == 0)
    {
        return false;
    }
    nLeft = m_FBufLength - nHeadLen - 4;
    pB += 4;
    for(int i = 0; i < wFileNum; i++)
    {
        memcpy(&fileHeadRow, pB, sizeof(SParamFileHeadRaw));
        nFileLen = qFromBigEndian(fileHeadRow.dwTotalLen);
        if(nFileLen > nLeft)
        {
            return false;
        }
        nFileId = ArCharToInt(fileHeadRow.szFileID, 2);
        baseParamFile = getParamMgr()->GetParamFileById(nFileId);
        if(baseParamFile == NULL)
        {
            return false;
        }
        if(bNewFile)
        {
            sFileName = baseParamFile->GetNewFileName();
        }
        else
        {
            sFileName = baseParamFile->GetFileName();
        }
        sTmpFileName = sFileName + ".tmp";
        if(QFile::exists(sTmpFileName))
        {
            QFile::remove(sTmpFileName);
        }
        QFile tmpFile(sTmpFileName);
        if(!tmpFile.open(QIODevice::ReadWrite))
        {
            return false;
        }
        int nRet = tmpFile.write((char*)pB, nFileLen);
        tmpFile.close();
#ifndef Q_OS_WIN
        QProcess::execute("sync");
#endif
        if(nRet == -1)
        {
            return false;
        }
        isRealNew = false;
        if(!baseParamFile->LoadParamFile(sTmpFileName, cfgFileHead, isRealNew))
        {
            QFile::remove(sTmpFileName);
            return false;
        }
        baseParamFile->SetCfgFileHead(cfgFileHead, isRealNew);
        if(!isRealNew)
        {
            baseParamFile->RenameParamFile(sTmpFileName, baseParamFile->GetFileName());
        }
        else
        {
            baseParamFile->RenameParamFile(sTmpFileName, baseParamFile->GetNewFileName());
        }
        nLeft = nLeft - nFileLen;
        if(nLeft < sizeof(fileHeadRow) && (i < wFileNum - 1))
        {
            return false;
        }
        pB += nFileLen;

    }
    return true;

}

FILENEED ReqParam::getNeedParamFile()
{
    if(m_listNeed.isEmpty())
    {
        FILENEED tmpFile;
        tmpFile.cfgFile = cfEnd;
        return tmpFile;
    }
    return m_listNeed.first();
}

void ReqParam::ReportProgress(bool bReport)
{
}

bool ReqParam::ReqParamFile(const CCfgFile cfgFile, quint16 nFileNum, const QString &sFileInfo, bool bNew)
{
    TMsgReqParam msg;
    memset(&msg, 0, sizeof(msg));
    int msgLen = 0;
    quint32 nTotalLen;
    TReqParam* task;
    quint16 msgId = 0;
    if(GetTaskCount() > 1)
    {
        return true;
    }
    task = new TReqParam;
    LogMsg("net", tr("开始下载参数:FileID:%1, FileNum:%2").arg(cfgFile).arg(nFileNum));
    msg.FileCnt = qFromBigEndian(nFileNum);
    memcpy(msg.FileInfo, sFileInfo.toLocal8Bit().data(), sFileInfo.length());
    msgLen = sFileInfo.length();
    if(msgLen != nFileNum * 16)
    {
        return false;
    }
    nTotalLen = sizeof(TMsgHeadInfo) + sizeof(msg.FileCnt) + msgLen + 2;
    if(bNew)
    {
        msgId = MSG_TYPE_REQNEWPARAM;
    }
    else
    {
        msgId = MSG_TYPE_REQPARAM;
    }
    memset(&msg.MsgHead, 0, sizeof(msg.MsgHead));
    msg.MsgHead.TotalLength = qFromBigEndian(nTotalLen);
    IntToArChar(msg.MsgHead.cMsgID, sizeof(msg.MsgHead.cMsgID), msgId);
    IntToArChar(msg.MsgHead.cMsgType, sizeof(msg.MsgHead.cMsgType), 0);
    msg.MsgHead.ZipType = '0';
    DateTimeToChar14(QDateTime::currentDateTime(), msg.MsgHead.SendTime);
    QString sTmp = GetMD5ForBuffer((char*)&msg.FileCnt, nTotalLen - sizeof(TMsgHeadInfo));
    memcpy(msg.MsgHead.VerifyCode, sTmp.toUpper().toLocal8Bit().data(), sTmp.length());
    memcpy(task->m_sendBuffer, &msg , nTotalLen);
    task->m_dwSendLen = nTotalLen;
    task->m_dwTimeOut = 30000;
    task->m_nFileId = cfgFile;
    return AddTask(task);
}

void ReqParam::StopExcute()
{
    m_bTer = false;
    if(m_nAskTimerId != -1)
    {
        killTimer(m_nAskTimerId);
        qDebug("471 kill ask timer %x", m_nAskTimerId);
        m_nAskTimerId = -1;
    }
    if(m_nReLoadTimerId != -1)
    {
        killTimer(m_nReLoadTimerId);
        qDebug("477 kill timer %x", m_nReLoadTimerId);
        m_nReLoadTimerId = -1;
    }
}

void ReqParam::StartAskParamFiles()
{
    m_nAskTimerId = startTimer(1000);
    qDebug("480 m_nAskTimerId %x",m_nAskTimerId);
}

ReqParam* getReqParam()
{
    static ReqParam req;
    return &req;
}

void ReqParam::FileLoadFail(int cfgFile)
{
    //m_listNeed.append((CCfgFile)cfgFile);
}

void ReqParam::AskParamFile()
{
    FILENEED tmpFile = getNeedParamFile();
    //qDebug() << "get need file" << tmpFile.cfgFile;
    if(cfEnd == tmpFile.cfgFile)
    {
        //参数全部下载并加载完成，发送参数加载完毕信号
        ////qDebug() << "ask timer id";
        if(m_nAskTimerId != -1)
        {
            killTimer(m_nAskTimerId);
            m_nAskTimerId = -1;
        }
        qDebug("507 kill ask timer %x", m_nAskTimerId);
        emit ParamFilesLoadSuccess();
        m_listNeed.clear();
        return;
    }
    if(getParamMgr()->GetParamFile(tmpFile.cfgFile) == NULL)
    {
        return;
    }
    ////qDebug() << "kill ask timer id";
    if(m_nAskTimerId != -1)
    {
        killTimer(m_nAskTimerId);
        m_nAskTimerId = -1;
        qDebug("520 kill ask timer %x", m_nAskTimerId);

    }
    ////qDebug() << "ask param file:" << cfgFile;

    //显示正在下载的信息
    emit ShowDownloadingFile(tmpFile.cfgFile);
    if(!DownLoadParam(tmpFile.cfgFile, tmpFile.bNew))
    {
        //显示下载失败
        //emit ShowPreDownloadResult(tmpFile.cfgFile,tmpFile.bNew ,false);
        m_cfgFile = tmpFile.cfgFile;
        m_bNew = tmpFile.bNew;
        m_bRet = false;

        //如果启动程序必须加载该文件，否则进行重新下载
        if(getParamMgr()->GetParamFile(tmpFile.cfgFile)->GetBootMust())
        {
            m_nReLoadTimerId = startTimer(60000);
            qDebug("573 m_nReLoadTimerId %x",m_nReLoadTimerId);

            return;
        }
        ////qDebug() << "预处理下载失败";
    }
    else
    {
        //emit ShowPreDownloadResult(tmpFile.cfgFile, tmpFile.bNew ,true);
    }
    start();
    //DownLoadFinish();
}

void ReqParam::DownLoadFinish()
{
    //qDebug() << getParamMgr()->GetParamFile((CCfgFile)cfgFile)->GetFileName() << bRet;

    qDebug() << "下载结果" << m_cfgFile << "new" << m_bNew << "ret" << m_bRet;
    //如果加载的是旧版文件，则无论是否下载成功，都要进行加载
    if(!m_bNew)
    {
        m_bRet = getParamMgr()->GetParamFile((CCfgFile)m_cfgFile)->CheckAndLoadParamFile();
        //如果加载成功，则移除队列项，下载下一个文件
        if(m_bRet)
        {
            if(!m_listNeed.isEmpty())
            {
                m_listNeed.dequeue();
                m_nAskTimerId = startTimer(50);
                qDebug("565 m_nAskTimerId %x",m_nAskTimerId);

            }
            emit LoadFinish(m_cfgFile, false);
        }
        else if(getParamMgr()->GetParamFile((CCfgFile)m_cfgFile)->GetBootMust())
        {
            m_nAskTimerId = startTimer(60000);
            qDebug("573 m_nAskTimerId %x",m_nAskTimerId);

        }
        //qDebug() << "load success" << bRet;
    }
    else
    {
        //新版文件无论成功失败，都移除项
        if(!m_listNeed.isEmpty())
        {
            m_listNeed.dequeue();
            m_nAskTimerId = startTimer(50);
            qDebug("585 m_nAskTimerId %x",m_nAskTimerId);

        }
    }
}

void ReqParam::AskFile(int cfgFile, bool bNew)
{
    FILENEED file;
    file.cfgFile = (CCfgFile)cfgFile;
    //qDebug() << "cfgFile" << file.cfgFile;
    file.bNew = bNew;
    m_listNeed.append(file);
    if(cfgFile == cfEnd)
    {
        AskParamFile();
    }
}

