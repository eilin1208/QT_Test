#ifndef REQPARAM_H
#define REQPARAM_H

#include <QObject>
#include <QMutex>
#include "CommonModule/qlanesocket.h"
#include "ParamModule/qparammgr.h"
struct TReqParam
{
    QString m_szServerIP;
    quint32 m_dwPort;
    QString m_sLastErr;
    QString m_sTempFileName;
    quint32 m_nFileId;
    quint8 m_sendBuffer[2049];
    quint32 m_dwSendLen;
    quint32 m_dwTimeOut;

};
struct FILENEED
{
    CCfgFile cfgFile;
    bool bNew;
public:
    bool operator ==(const FILENEED& fileNeed)
    {
        return ((cfgFile == fileNeed.cfgFile) && (bNew == fileNeed.bNew));
    }
};

class ReqParam : public QThread
{
    Q_OBJECT
public:
    explicit ReqParam(QObject *parent = 0);
    virtual ~ReqParam();
signals:
    void ParamFilesLoadSuccess();
    void ShowDownloadingFile(quint32 cfgFile);
    void ShowPreDownloadResult(quint32 cfgFile,bool bNew, bool bRet);
    void ShowDownloadedResult(quint32 cfgFile,bool bNew, bool bRet);
    //加载完成
    void LoadFinish(int cfgFile, bool bNew);

public slots:
    void FileLoadFail(int cfgFile);
    //开始下载参数
    void StartAskParamFiles();
    void AskParamFile();
    //下载参数完成
    void DownLoadFinish();
    //下载指定文件
    void AskFile(int cfgFile, bool bNew);
public:
    void SetHostInfo(const QString& sHost, int nPort);
    void ReportProgress(bool bReport);
    bool ReqParamFile(const CCfgFile cfgFile, quint16 nFileNum,
                      const QString& sFileInfo, bool bNew);
    void StopExcute();
    bool DownLoadParam(int cfgFile, bool bNew);
    void Term();
protected:
    bool AddTask(TReqParam* askReq);
    bool DownLoadParam(TReqParam *task);
    int GetTaskCount();
    void timerEvent(QTimerEvent *event);
protected:
    void run();
private:
    bool TryReadBuffer(char* buffer, const int nByteCount, const int nTimeOut);
    bool SaveParamFile();

    FILENEED getNeedParamFile();
private:
    //完成一次下载后的结果
    quint32 m_cfgFile;
    bool m_bNew;
    bool m_bRet;
    //
    quint32 m_FMaxBufSize;
    char* m_FDataBuffer;
    int m_FBufLength;
    QLaneSocket* m_client;
    QMutex m_mutex;
    QQueue<TReqParam*> m_listQuery;
    QString m_sIp;
    quint32 m_nPort;
    bool m_bTer;
    QQueue<FILENEED> m_listNeed;
    //加载参数定时器ID
    int m_nAskTimerId;
    //重新加载参数定时器ID
    int m_nReLoadTimerId;
    //下载不成功时是否需要等待
    bool m_bNeedWait;
};

ReqParam* getReqParam();


#endif // REQPARAM_H
