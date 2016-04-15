#include "qdatasendmgr.h"
#include <CommonModule/laneutils_new.h>
#include <QMutexLocker>
#include <QList>
#include "CommonModule/MsgDefine.h"
#include "msgsendfunc.h"
QDataSendMgr::QDataSendMgr()
{
    m_pDataClient = NULL;
    m_pDBDataSaver = NULL;
    m_pSupDataSaver = NULL;
}
QDataSendMgr::~QDataSendMgr()
{
    ClearSupClients();
    if(m_pDataClient)
    {
        LogMsg("net", QObject::tr("开始释放业务报文发送线程..."));
        m_pDataClient->Stop();

        LogMsg("net", QObject::tr("报文发送线程释放完毕!"));
        m_pDataClient->deleteLater();
        m_pDataClient = NULL;
    }
    m_pDBDataSaver = NULL;
    m_pSupDataSaver = NULL;
}
QDataSendClient::QDataSendClient()
{
    m_pSendBuff=NULL;
    m_nBuffLen = 0;
    m_bMemoryDB =false;
    m_bSendImg = false;
    m_bDelImg = false;
    m_nPort =0;
    m_pDataSaver = NULL;
    m_llLastId=-1;
    m_nLastErrCode = 0;
    m_sLastErr="";
    m_szHost="";
    m_skClient=NULL;
    this->setObjectName("QDataSendClient");
}
QDataSendClient::~QDataSendClient()
{
    LogMsg("net", QObject::tr("release data send client thread..."));
    if (m_pSendBuff)
    {
        delete [] m_pSendBuff;
        LogMsg("net",tr("[QDataSendClient::~QDataSendClient]执行delete m_pSendVuff"));
    }
    m_pSendBuff = NULL;
    m_pDataSaver = NULL;
}

void QDataSendClient::RunOnce()
{
    if(m_skClient==NULL)
    {
        m_skClient=new QLaneSocket;
        LogMsg("net",tr("[QDataSendClient::RunOnce]执行new m_skClient指针"));
    }
    if (!CheckNetState())
    {
        //网络不通时清除即时发送数据
        ClearInstantData();
        ThrowErrorCode(Comm_ErrCode_ConnFailed);
        DealWithError(Comm_ErrCode_ConnFailed);
        return;
    }

    //发送即时数据
    SendInstantMsg();

    TDataToSave data;
    TDataToSave xmldata;
    memset(&data,0,sizeof(TDataToSave));
    memset(&xmldata,0,sizeof(TDataToSave));
    memset(m_SaveDataBuff,0,sizeof(m_SaveDataBuff));
    data.pMsg = m_SaveDataBuff;
    data.nMsgLen = sizeof(m_SaveDataBuff);
    bool bRet=GetNeedUpData(m_llLastId,data);
    if (!bRet)
    {
        ThrowErrorCode(Comm_ErrCode_NoData);
        DealWithError(Comm_ErrCode_NoData);
        return;
    }
    memcpy(&xmldata,&data,sizeof(TDataToSave));
    TMsgToSend *pMsgToSend = (TMsgToSend*)data.pMsg;
    char szMsgType[5]={'\0'};
    memcpy(szMsgType, pMsgToSend->BasicInfo.msgHead.cMsgID, 2);
    memcpy(szMsgType+2, pMsgToSend->BasicInfo.msgHead.cMsgType, 2);
    quint8 bWasteSN=pMsgToSend->BasicInfo.WasteID;
    LogMsg("net", QObject::tr("读取报文数据,[流水号:%1][报文代码:%2][交易流水号:%3][数据长度:%4][报文类型%5%6]")
           .arg(m_llLastId).arg(szMsgType).arg(bWasteSN).arg(data.nMsgLen).arg(data.MsgID).arg(data.MsgType));

    int nBuffLen=0;
    if (!PackSendData(&data,nBuffLen,m_llLastId))
    {
        ThrowErrorCode(Comm_ErrCode_DataError);
        DealWithError(Comm_ErrCode_DataError);
        return;
    }

    // 发送数据
    LogMsg("net", QObject::tr("[流水号:%1] 开始发送,总长度[%2]").arg(m_llLastId).arg(nBuffLen));
    int nErrCode = SendData(nBuffLen);
    if (Comm_ErrCode_Ok != nErrCode)
    {
        LogMsg("net", QObject::tr("[流水号:%1] 报文发送失败,关闭当前连接").arg(m_llLastId));
        m_skClient->Close();
        ThrowErrorCode(nErrCode);
        DealWithError(nErrCode);
        return;
    }
    LogMsg("net", QObject::tr("[流水号:%1] 发送完成,开始接收应答...").arg(m_llLastId));
    nErrCode=ReadAndCheckResponsMsg(m_llLastId, 20000);
    if(Comm_ErrCode_Ok != nErrCode )
    {
        LogMsg("net", QObject::tr("[流水号:%1] 读取应答失败,关闭当前连接").arg(m_llLastId));
        m_skClient->Close();
        ThrowErrorCode(nErrCode);
        DealWithError(nErrCode);
        return;
    }

    // 更新传输标志
    if (!m_bMemoryDB)
    {
        //QFileLogger::WriteLog("net", QObject::tr("[流水号:%1] 上传成功,开始更新上传标志...").arg(m_llLastId));
        LogMsg("net", QObject::tr("[流水号:%1] 上传成功,开始更新上传标志...").arg(m_llLastId));
        if (!MarkDataAsUpLoad(m_llLastId,xmldata))
        {
            //QFileLogger::WriteLog("net", QObject::tr("[流水号:%1]上传标志更新失败!!!").arg(m_llLastId));
            LogMsg("net", QObject::tr("[流水号:%1]上传标志更新失败!!!").arg(m_llLastId));
            ThrowErrorCode(Comm_ErrCode_UpDateFlagFailed);
            DealWithError(Comm_ErrCode_UpDateFlagFailed);
            return;
        }
    }

    // TODO:删除图片
    if (m_bSendImg && m_bDelImg)
    {

    }
    LogMsg("net", QObject::tr("[流水号:%1]发送成功").arg(m_llLastId));
    //qDebug()<<QObject::tr("[流水号:%1]发送成功").arg(m_llLastId);
    // 更新要上传的流水号，下次直接取下一条数据
    m_llLastId++;
    m_nLastErrCode=0;
}

bool QDataSendClient::CheckNetState()
{

    bool bRet=true;
    if (!m_skClient->isConnected())
    {
        m_skClient->Close();
        delete m_skClient;
        m_skClient = new QLaneSocket;
        bRet=m_skClient->ConnectToserver(m_szHost,m_nPort);
        if (bRet)
        {
            LogMsg("net", QObject::tr("连接服务器[%1:%2]成功").arg(m_szHost).arg(m_nPort));
        }
        else
        {
            LogMsg("net", QObject::tr("连接服务器[%1:%2]失败").arg(m_szHost).arg(m_nPort));
        }
    }
    return bRet;

}

void QDataSendClient::ClearInstantData()
{
    //QDebug() << "mutex" << "ClearInstantData";
    QMutexLocker locker(&mutex);
    if (m_InstantMsgLst.empty())
    {
        return;
    }
    QList<TDataToSave>::iterator it = m_InstantMsgLst.begin();
    for(;it!= m_InstantMsgLst.end();++it)
    {
        if (it->pMsg)
        {
            delete [] it->pMsg;
        }
    }

    m_InstantMsgLst.clear();
}

void QDataSendClient::ThrowErrorCode(int nErrCode)
{
    switch(nErrCode)
    {
    case 1:
        m_sLastErr = QObject::tr("网络连接失败");
        m_nLastErrCode=nErrCode;
        break;
    case 2:
        m_sLastErr = QObject::tr("取发送数据失败");
        m_nLastErrCode=nErrCode;
        break;
    case 3:
        m_sLastErr = QObject::tr("报文打包失败");
        m_nLastErrCode=nErrCode;
        break;
    case 4:
        m_sLastErr = QObject::tr("报文发送失败");
        m_nLastErrCode=nErrCode;
        break;
    case 5:
        m_sLastErr = QObject::tr("读回应报文失败");
        m_nLastErrCode=nErrCode;
        break;
    case 6:
        m_sLastErr =QObject::tr("回应报文处理异常");
        m_nLastErrCode=nErrCode;
        break;
    case 7:
        m_sLastErr = QObject::tr("收费站返回报文处理失败");
        m_nLastErrCode=nErrCode;
        break;
    case 8:
        m_sLastErr = QObject::tr("更新上传标志失败");
        m_nLastErrCode=nErrCode;
        break;
    default:
        m_sLastErr = QObject::tr("报文处理未知异常");
        m_nLastErrCode=nErrCode;
        break;
    }
}

bool QDataSendClient::SendInstantMsg()
{
    TDataToSave DataToSave;
    memset(&DataToSave,0,sizeof (TDataToSave));

    if (!ReadInstantData(DataToSave))
    {
        return FALSE;
    }

    int bRet=FALSE;
    if (DataToSave.nMsgLen>0 && NULL!=DataToSave.pMsg)
    {
        char szMsgInfo[6]="";
        memcpy(szMsgInfo,(char*)(DataToSave.pMsg)+4,4);
        LogMsg("net", QObject::tr("即时发送数据[%1],数据长度[%2]").arg(szMsgInfo).arg(DataToSave.nMsgLen));
        if (m_skClient->WriteData((char*)DataToSave.pMsg,DataToSave.nMsgLen))
        {
            LogMsg("net", QObject::tr("读取即时发送数据回应"));
            bRet=ReadAndCheckResponsMsg(0,10);
            if(bRet)
            {
                LogMsg("net", QObject::tr("读取即时发送数据回应成功"));
            }
            else
            {
                LogMsg("net", QObject::tr("读取即时发送数据回应失败"));
            }
        }
        delete [] DataToSave.pMsg;
    }

    return bRet;
}

void QDataSendClient::DealWithError(int Error)
{
    if(!m_bStop)
    {
        if(Error>=Comm_ErrCode_Ok||Error<=Comm_ErrCode_UpDateFlagFailed)
        {
            int nPauseTime=0;
            bool bResult = OnDataClientException(Error,nPauseTime);
            if(nPauseTime<0)
            {
                //在此调用线程的Pause。
                this->Pause();
                return;
            }

            CheckExit(nPauseTime);
        }
        else
        {
            //日志打印
            LogMsg("net", QObject::tr("未知异常,关闭当前连接,1分钟后重试"));
            m_skClient->Close();
            if (m_bMemoryDB)
            {
                LogMsg("net", QObject::tr("监控计算机[%1:%2]未知异常,停止处理").arg(m_szHost).arg(m_nPort));
                //调用线程的Pause
                this->Pause();
                return;
            }
            //调用线程的CheckExit
            CheckExit(30000);
        }
    }
}

bool QDataSendClient::OnDataClientException(int nErrCode, int &nPauseTime)
{
    //缺省等待时间为30秒
    nPauseTime = 30000;
    switch(nErrCode)
    {
    case Comm_ErrCode_NoData:
    {
        nPauseTime = 10000;
        break;
    }
    case Comm_ErrCode_ConnFailed:
    case Comm_ErrCode_DataError:
    case Comm_ErrCode_SendFailed:
    case Comm_ErrCode_ReadResponseFailed:
    case Comm_ErrCode_ResponseError:
    {
        break;
    }
    case Comm_ErrCode_ServerResponseFailed:
    {
        //服务端处理失败,3分钟后重试
        nPauseTime= 180000;
        break;
    }
    case Comm_ErrCode_UpDateFlagFailed:
    {
        nPauseTime = 10000;
        break;
    }
    }

    if(Comm_ErrCode_NoData != nErrCode)
    {
        if(nErrCode ==m_nLastErrCode)
        {
            nPauseTime *=2;
        }
        m_nLastErrCode = nErrCode;
        LogMsg("net", QObject::tr("%1,%2毫秒后继续处理。lastId:%3,IP:%4,Port:%5")
               .arg(m_sLastErr).arg(nPauseTime).arg(m_llLastId).arg(m_szHost).arg(m_nPort));
    }

    if(!m_skClient->isConnected())
    {
        if(m_bMemoryDB)
        {
            LogMsg("net", QObject::tr("监控计算机[%1:%2]通讯异常,停止处理").arg(m_szHost).arg(m_nPort));
            //暂停处理
            nPauseTime =-1;
            return false;
        }
    }
    return true;
}

void QDataSendMgr::ClearSupClients()
{
    //QDebug() << "mutex" << "ClearSupClients";
    QMutexLocker locker(&mutex);
    QList<QDataSendClient*>::iterator it = m_SupClients.begin();
    for(;it!=m_SupClients.end();++it)
    {
        QDataSendClient *pClient = *it;
        *it= NULL;
        if(pClient)
        {
            pClient->Stop();
            pClient->deleteLater();
        }
    }
    m_SupClients.clear();
}

bool QDataSendClient::InitClient(const QString &szHost, int nPort, quint64 dwSize)
{
    if (!m_pSendBuff)
    {
        try
        {
            quint64 dwLen = dwSize;
            m_pSendBuff = new char[dwLen];
            LogMsg("net",tr("[QDataSendClient::InitClient]执行new m_pSendVuff"));
            if (NULL!=m_pSendBuff)
            {
                m_nBuffLen =dwLen;
            }
            else
            {
                return FALSE;
            }
            memset(m_pSendBuff, 0, dwSize);
            m_szHost=szHost;
            m_nPort = nPort;
            return TRUE;
        }
        catch (...)
        {
            m_pSendBuff = NULL;
            m_nBuffLen =0;
        }
    }
    return FALSE;
}

bool QDataSendClient::ResizeBuff(int nSize)
{
    if (nSize > m_nBuffLen)
    {
        if (m_pSendBuff)
        {
            delete [] m_pSendBuff;
            LogMsg("net",tr("[QDataSendClient::ResizeBuff]执行delete m_pSendVuff"));
        }
        m_pSendBuff=NULL;
        m_nBuffLen=0;
    }

    if (NULL==m_pSendBuff)
    {
        try
        {
            m_nBuffLen=0;
            m_pSendBuff = new char[nSize];
            LogMsg("net",tr("[QDataSendClient::ResizeBuff]执行new m_pSendVuff"));
            if (m_pSendBuff)
            {
                m_nBuffLen = nSize;
                return true;
            }
        }
        catch(...)
        {
            m_nBuffLen=0;
            m_pSendBuff=NULL;
        }
    }
    return FALSE;
}

bool QDataSendClient::bSameHost(const QString &szHost, int nPort)
{
    if(m_szHost!=szHost)
    {
        if(m_nPort ==nPort)
            return TRUE;
    }
    return FALSE;
}

void QDataSendClient::SaveInstantData(const TDataToSave &DataToSave)
{
    QMutexLocker locker(&mutex);
    TDataToSave tmpDataToSave;
    tmpDataToSave.nMsgLen = DataToSave.nMsgLen;
    tmpDataToSave.MsgType = DataToSave.MsgType;
    tmpDataToSave.OccurTime = DataToSave.OccurTime;
    tmpDataToSave.pMsg= NULL;
    tmpDataToSave.pMsg = new char[DataToSave.nMsgLen];
    if(!tmpDataToSave.pMsg)
    {
        tmpDataToSave.nMsgLen=0;
        return;
    }
    memcpy(tmpDataToSave.pMsg,DataToSave.pMsg,DataToSave.nMsgLen);
    m_InstantMsgLst.push_back(tmpDataToSave);
    return;
}

bool QDataSendClient::PackSendData(const TDataToSave *pData, int &nActualLen, const quint64 iLastId)
{
    if(!pData)
        return FALSE;
    if((0==pData->nMsgLen) || !(pData->pMsg))
        return FALSE;

    TMsgToSend *pMsgToSend = (TMsgToSend*)pData->pMsg;

    if(pData->nMsgLen < sizeof(TMsgHeadInfo)+12)
    {
        return false;
    }
    pMsgToSend->BasicInfo.WasteID = iLastId;
    memset(m_pSendBuff,0,m_nBuffLen);
    int nSendLen=0;
    bool bRet = getMsgSendFun()->PackMsg(pMsgToSend, m_pSendBuff, nSendLen);
    if(!bRet)
    {
        LogMsg("net", QObject::tr("PackSendData 重新打包报文失败"));
    }
    nActualLen = nSendLen;
    return bRet;
}

bool QDataSendClient::GetNeedUpData(qint64 &pllLastId, TDataToSave &SendData)
{
    if (!m_pDataSaver)
    {
        return false;
    }
    //获取第一条未上传数据
    pllLastId = m_pDataSaver->GetUnDispatchID(dtSocket);
    return m_pDataSaver->GetFirstData(pllLastId, &SendData, dtSocket);
}

int QDataSendClient::SendData(quint64 dwDataLen)
{
    bool bRet =m_skClient->WriteData(m_pSendBuff,dwDataLen);
    if(!bRet)
    {
        LogMsg("net", tr("报文发送失败,长度:%1").arg(dwDataLen));
        return Comm_ErrCode_SendFailed;
    }
    TMsgToSend *data = (TMsgToSend*)m_pSendBuff;
    return Comm_ErrCode_Ok;
}

int QDataSendClient::ReadAndCheckResponsMsg(const quint64 llLastId, quint32 dwTimeOut)
{
    //读返回值
    TMsgHeadInfo MsgHead;
    memset(&MsgHead,0,sizeof(TMsgHeadInfo));
    LogMsg("lane", "data send client read data");
    if(!m_skClient->ReadData((char*)&MsgHead,sizeof(TMsgHeadInfo),dwTimeOut))
    {
        LogMsg("net", tr("读取回应报文头失败"));
        return Comm_ErrCode_ReadResponseFailed;
    }

    quint32 dwTotalLen= sizeof(TMsgResponseInfo);
    if(dwTotalLen < sizeof(TMsgHeadInfo))
    {
        LogMsg("net", tr("回应报文长度错误:%1").arg(dwTotalLen));
        return Comm_ErrCode_ResponseError;
    }

    char *pBuff=NULL;

    pBuff = new char[dwTotalLen];
    LogMsg("net",tr("[QDataSendClient::ReadAndCheckResponsMsg]执行new pBuffer"));
    if(!pBuff)
        return Comm_ErrCode_ReadResponseFailed;

    memset(pBuff,0,dwTotalLen);
    memcpy(pBuff,&MsgHead,sizeof(TMsgHeadInfo));
#ifdef DEBUG
    //QDebug() << "开始接收数据";
#endif
    qint64 dataLength = m_skClient->ReadData((char*)pBuff+sizeof(TMsgHeadInfo),dwTotalLen -sizeof(TMsgHeadInfo),dwTimeOut);
    if(dataLength == 0)
    {
        delete [] pBuff;
        LogMsg("net", tr("读取回应报文失败"));
        LogMsg("net",tr("[QDataSendClient::ReadAndCheckResponsMsg]执行delete pBuffer"));
        return Comm_ErrCode_ReadResponseFailed;
    }
#ifdef DEBUG
    //QDebug() << "程序接收到数据，数据长度为：" << dataLength;
#endif
    //TODO:这里缺少报文头的校验

    TMsgResponseInfo ResMsg;
    memset(&ResMsg,0,sizeof(TMsgResponseInfo));
    memcpy(&ResMsg,pBuff,sizeof(TMsgResponseInfo));
    delete [] pBuff;
    LogMsg("net",tr("[QDataSendClient::ReadAndCheckResponsMsg]执行delete pBuffer"));
    int nRet = ResMsg.RecResult;
    switch(nRet)
    {    
    case 1:
    {
        LogMsg("net", tr("服务端处理成功"));
        return Comm_ErrCode_Ok;
    }
    case 2:
    case 3:
    {
        /*---------------------日志----------------------*/
        //LogFmtMsg(_T("服务端处理失败!--错误代码:%d"),nRet);
        /*---------------------日志----------------------*/
        //QFileLogger::WriteLog("net",QObject::tr("服务端处理失败!--错误代码:%1").arg(nRet));
        LogMsg("net", tr("服务端处理失败,错误代码:%1").arg(nRet));
        return Comm_ErrCode_ServerResponseFailed;
    }
    default:
        return Comm_ErrCode_ResponseError;
    }
}

bool QDataSendClient::MarkDataAsUpLoad(const quint64 llLastId,TDataToSave &xmlData)
{
    if (!m_pDataSaver)
    {
        return FALSE;
    }

    //return m_pDataSaver->DoMarkDataState(llLastId, dtSocket);

    //liujian
    //更新xml文件，参数m_llLastId,xmldata
    //删除原xml数据
    if(m_pDataSaver->DoMarkDataState(llLastId, dtSocket))
    {
        LogMsg("lane",tr("开始备份已上传xml文件 ID:%1").arg(m_llLastId));
//        getXmlManager()->SaveLaneWasteData(llLastId,m_pDataSaver->getStaIdByID(llLastId),m_pDataSaver->getLaneIdByID(llLastId),&xmlData,true/*,fileName*/);
        LogMsg("lane",tr("备份已上传xml文件完成 ID:%1").arg(m_llLastId));
        return true;
    }
    else
        return false;
}


bool QDataSendClient::ReadInstantData(TDataToSave &DataToSave)
{
    //QDebug() << "mutex" << "ReadInstantData";
    QMutexLocker locker(&mutex);
    if (m_InstantMsgLst.empty())
    {
        return FALSE;
    }
    QDateTime sysTime=QDateTime::currentDateTime();
    quint32 dwTime=SystemTime2CardTime(sysTime);
    QList<TDataToSave>::iterator it = m_InstantMsgLst.begin();

    for(;it!= m_InstantMsgLst.end();)
    {
        if ((dwTime - it->OccurTime)> 300)
        {
            // 删除在队列中保存时间过长的数据
            it=m_InstantMsgLst.erase(it);
        }
        else
        {
            // 从队列中取出首条符合条件的数据，将原数据从队列中删除
            DataToSave.nMsgLen = it->nMsgLen;
            DataToSave.MsgType =it->MsgType;
            DataToSave.OccurTime =it->OccurTime;
            DataToSave.pMsg =it->pMsg;
            //memcpy(DataToSave.szAppFiles,it->szAppFiles,sizeof(it->szAppFiles));
            m_InstantMsgLst.erase(it);
            return true;
        }
    }
    // 未找到合适的数据，返回FALSE
    return FALSE;
}

bool QDataSendMgr::InitSendDataClient(const QString &szHost, int nPort, bool bDeleteImg)
{
    if(m_pDataClient)
    {
        m_pDataClient->Stop();
        delete m_pDataClient;
        m_pDataClient =NULL;
    }
    m_pDataClient = new QDataSendClient;
    m_pDataClient->SetDataSaver(m_pDBDataSaver);
    m_pDataClient->SetImgOption(TRUE,bDeleteImg);
    m_pDataClient->InitClient(szHost,nPort);

    m_pDataClient->start();
    return TRUE;
}

void QDataSendMgr::SetDataSaver(SqliteDataSaver *pDBDataSaver)
{
    m_pDBDataSaver = pDBDataSaver;
}

void QDataSendMgr::SaveInstantData(const TDataToSave &DataToSave)
{
    if(m_pDataClient)
        m_pDataClient->SaveInstantData(DataToSave);
    //QDebug() << "mutex" << "SaveInstantData";

    QMutexLocker Locker(&mutex);
    QList<QDataSendClient*>::iterator it = m_SupClients.begin();

    for(;it!= m_SupClients.end();++it)
    {
        QDataSendClient *pClient =*it;
        if(pClient)
        {
            if(pClient->isRunning())
            {
                pClient->SaveInstantData(DataToSave);
            }
        }
    }
}

void QDataSendMgr::ReleaseDataClient()
{
    ClearSupClients();
    if(m_pDataClient)
    {
        m_pDataClient->SetDataSaver(NULL);
        m_pDataClient->Stop();
        m_pDataClient->deleteLater();
        m_pDataClient = NULL;
    }
}

void QDataSendMgr::CheckAndReleaseClient(const QString &szHost, int nPort)
{
    QList<QDataSendClient*>::iterator it= m_SupClients.begin();
    for(;it!=m_SupClients.end();)
    {
        QDataSendClient *pClient =*it;
        if(pClient)
        {
            if((false==pClient->isRunning())|| pClient->bSameHost(szHost,nPort))
            {
                //日志打印
                pClient->Stop();
                it = m_SupClients.erase(it);
                delete pClient;
                continue;
            }
        }
        it++;
    }
}

QDataSendMgr* getDataSendMgr()
{
    static QDataSendMgr sendMgr;
    return &sendMgr;
}
