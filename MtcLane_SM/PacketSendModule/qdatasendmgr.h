#ifndef QDATASENDMGR_H
#define QDATASENDMGR_H
#include <CommonModule/qlanesocket.h>
#include <QMutex>
#include <CommonModule/qpausablethread.h>
#include "sqlitedatasaver.h"
//数据发送客户端，发送数据
class QDataSendClient:public QPausableThread
{
public:
    enum CCommErrorCode
    {
        Comm_ErrCode_Ok,
        Comm_ErrCode_ConnFailed,
        Comm_ErrCode_NoData,
        Comm_ErrCode_DataError,
        Comm_ErrCode_SendFailed,
        Comm_ErrCode_ReadResponseFailed,                //读回应报文失败
        Comm_ErrCode_ResponseError,			//回应报文错误
        Comm_ErrCode_ServerResponseFailed,              //服务端回应失败
        Comm_ErrCode_UpDateFlagFailed
    };

    QDataSendClient();
    virtual ~QDataSendClient();
    void SetDataSaver(SqliteDataSaver *pDataSaver,bool bMemoryDB=FALSE)
    {
        m_pDataSaver = pDataSaver;
        m_bMemoryDB = bMemoryDB;
    }
    void SetImgOption(bool bSendImg,bool bDelImg)
    {
        m_bSendImg = bSendImg;
        m_bDelImg = bDelImg;
    }

    virtual void RunOnce(void);
    bool InitClient(const QString &szHost,int nPort,quint64 dwSize=1024*1024);
    bool ResizeBuff(int nSize);
    bool bSameHost(const QString &szHost,int nPort);
    void SaveInstantData(const TDataToSave &DataToSave);

protected:
    //对应源程序的ThrowException对异常的处理
    void ThrowErrorCode(int nErrCode);
    //异常处理，用于配合ThrowException来进行异常处理
    void DealWithError(int Error);
    bool PackSendData(const TDataToSave *pData,int &nActualLen,const quint64 iLastId);
    bool CheckNetState();
    bool GetNeedUpData(qint64 &pllLastId,TDataToSave &SendData);
    int  SendData(quint64 dwDataLen);
    int ReadAndCheckResponsMsg(const quint64 llLastId,quint32 dwTimeOut=1);
    bool MarkDataAsUpLoad(const quint64 llLastId,TDataToSave &xmlData);
    bool ReadInstantData(TDataToSave &DataToSave);
    bool SendInstantMsg();
    void ClearInstantData();
    bool OnDataClientException(int nErrCode,int &nPauseTime);

private:
    SqliteDataSaver *m_pDataSaver;
    QLaneSocket *m_skClient;
    //是否内存数据库
    bool m_bMemoryDB;
    //是否上传图片
    bool m_bSendImg;
    //上传图片后是否删除
    bool m_bDelImg;
    //m_pSendBuff 长度
    int m_nBuffLen;
    //用于保存从库内取出的临时数据
    char m_SaveDataBuff[10240];
    //存放带有图片信息的报文
    char *m_pSendBuff;
    QString m_szHost;
    int m_nPort;
    //保存即时发送数据,最多保存5分钟以内的数据,主要存放网络状态报文
    QList <TDataToSave> m_InstantMsgLst;
    QMutex mutex;
    QString m_sLastErr;
    int m_nLastErrCode;
    qint64 m_llLastId;
};

class QDataSendMgr
{
public:
    QDataSendMgr();
    virtual ~QDataSendMgr();
    //初始化业务报文客户端，启动发送线程
    bool InitSendDataClient(const QString &szHost,int nPort,bool bDeleteImg=FALSE);
    void SetDataSaver(SqliteDataSaver *pDBDataSaver);
    void SaveInstantData(const TDataToSave &DataToSave);
    void ReleaseDataClient();
private:
    void CheckAndReleaseClient(const QString &szHost,int nPort);
    void ClearSupClients();
private:
    SqliteDataSaver *m_pDBDataSaver;
    BaseSaver *m_pSupDataSaver;

    QList<QDataSendClient*> m_SupClients;
    QDataSendClient *m_pDataClient;

    QMutex mutex;
};

QDataSendMgr* getDataSendMgr();
#endif // QDATASENDMGR_H
