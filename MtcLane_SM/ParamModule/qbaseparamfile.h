#ifndef QBASEPARAMFILE_H
#define QBASEPARAMFILE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "parammapcontainer.h"

//参数文件头
#pragma pack(push,1)
struct SParamFileHeadRaw
{
    char szFileID[2];
    char szFileFmtVersion[2];
    char szVersion[14];
    quint32 dwTotalLen;
    char szUseTime[14];
    quint32 dwRecordCount;
    quint32 dwRecLength;
    char EncryptFlag;
    char Spare[8];
    char VerifyCode[32];
};

#pragma pack(pop)

//文件头结构体
class SParamFileHead
{
public:
    int nFileID;                //文件ID
    QDateTime lVersion;              //8位版本号，空字符结尾
    quint32 dwTotalLen;         //文件总长度
    quint32 dwRecordCount;      //文件记录数
    quint32 dwRecLength;        //文件记录长度
    QDateTime tUseTime;         //启用时间
    char szVerifyCode[32];      //MD5校验值
    char EncryptFlag;           //压缩标志 0，不压缩
public:
    SParamFileHead()
    {
        nFileID =0;
        dwTotalLen =0;
        dwRecordCount =0;
        dwRecLength =0;
        EncryptFlag =0;
    }
    void operator =(const SParamFileHead & cfgFileHead)
    {
        nFileID = cfgFileHead.nFileID;
        lVersion = cfgFileHead.lVersion;
        dwTotalLen = cfgFileHead.dwTotalLen;
        dwRecordCount = cfgFileHead.dwRecordCount;
        dwRecLength = cfgFileHead.dwRecLength;
        tUseTime = cfgFileHead.tUseTime;
        EncryptFlag = cfgFileHead.EncryptFlag;
    }

    bool GetValueByRaw(SParamFileHeadRaw & raw);
};

class QBaseParamFile : public QObject
{
    Q_OBJECT
public:
    explicit QBaseParamFile(QObject *parent = 0);

signals:
    void LoadSuccess(bool isNew);
public slots:

public:
    //检查并加载文件
    virtual bool CheckAndLoadParamFile();

    // 加载文件
    virtual bool LoadParamFile(QString sFileName, SParamFileHead &cfgFileHead, bool& isNew);

    // 初始化
    virtual void InitParamFile(){}
public:
    // set/get 启动必需
    void SetBootMust(bool bBootMust){m_bBootMust = bBootMust;}
    bool GetBootMust(){return m_bBootMust;}

    // set/get 参数文件类型
    void SetCfgFileType(int nCfgFileType){m_nCfgFileType = nCfgFileType;}
    int GetCfgFileType(){return m_nCfgFileType;}

    // set/get 文件ID
    void SetFileID(int nFileID){m_nFileID = nFileID;}
    int GetFileID(){return m_nFileID;}

    // set/get 文件名
    void SetFileName(const QString & sFileName){m_sFileName = sFileName;}
    QString GetFileName(){return m_sFileName;}

    // set/get 新版文件名
    void SetNewFileName(const QString & sNewFileName){m_sNewFileName = sNewFileName;}
    QString GetNewFileName(){return m_sNewFileName;}

    // set/get 文件显示名
    void SetDispFileName(const QString & sDispFileName){m_sDispFileName = sDispFileName;}
    QString GetDispFileName(){return m_sDispFileName;}

    // set/get 是否有新版文件
    void SetHasNewVersion(bool bHasNewVersion){m_bHasNewVersion = bHasNewVersion;}
    bool GetHasNewVersion(){return m_bHasNewVersion;}

    // set/get 是否允许为空
    void SetAllowNull(bool bAllowNull){m_bAllowNull = bAllowNull;}
    bool GetAllowNull(){return m_bAllowNull;}

    // set/get 路径名
    void SetFileDir(const QString & sFileDir){m_sFileDir = sFileDir;}
    QString GetFileDir(){return m_sFileDir;}

    QDateTime GetVersion(bool bNew);

    // set/get 启用时间
    QDateTime GetUseTime(){return spFileHead.tUseTime;}

    // 获取版本号
    QString GetVersionCode(){return spFileHead.lVersion.toString("yyyyMMddhhmmss");}

    // 取记录条数
    quint32 getRowCount(bool bNew = false)
    {
        if(bNew)
            return spNewFileHead.dwRecordCount;
        else
            return spFileHead.dwRecordCount;
    }
    // 更新参数文件头
    void SetCfgFileHead(const SParamFileHead &cfgFileHead,bool bNew=false);
    // 取参数文件头
    bool GetCfgFileHead(SParamFileHead &cfgFileHead,bool bNew=false);
    bool IsDownloadOnTime(){return m_bDownLoadOnTime;}
    bool RenameParamFile(const QString &sOldName, const QString &sNewName);
    // set/get isLoad
    bool GetFileIsLoad(){return m_bIsLoad;}
    void SetFileIsLoad(bool bIsLoad){m_bIsLoad = bIsLoad;}
protected:
    virtual bool loadParam_begin()
    {
        if (NULL!=m_pContainer)
        {
            return m_pContainer->load_begin();
        }
        return false;
    }

    virtual bool loadParam_cancel()
    {
        if (NULL!=m_pContainer)
        {
            return m_pContainer->load_cancel();
        }
        return false;
    }

    virtual bool loadParam_addRow(const char * dataBuffer)= 0;

    virtual bool loadParam_complete()
    {
        if (NULL!=m_pContainer)
        {
            return m_pContainer->load_complete();
        }
        return false;
    }

protected:
    AbstractParamContainer *m_pContainer;

protected:
    // 校验文件头
    virtual bool VerifyFileHead(const QString & sFileName, SParamFileHead& cfgFileHead);

    // 加载文件数据部分
    virtual bool LoadData(const QString & sFileName, bool bNew);
protected:
    // 启动必需
    bool m_bBootMust;
    // 参数文件类型（同枚举定义）
    int m_nCfgFileType;
    // 文件ID
    int m_nFileID;
    // 文件名
    QString m_sFileName;
    // 新版文件名
    QString m_sNewFileName;
    // 文件显示名
    QString m_sDispFileName;
    // 是否有新版文件
    bool m_bHasNewVersion;
    // 允许为空
    bool m_bAllowNull;
    // 路径名
    QString m_sFileDir;
    // 需要定时下载
    bool m_bDownLoadOnTime;
    // 文件头
    SParamFileHead spFileHead;
    // 新版文件头
    SParamFileHead spNewFileHead;
    // dataContainer长度
    int m_nDataContainerLength;
    // 文件是否被加载
    bool m_bIsLoad;
};
QString getCfgFilePath();
#endif // QBASEPARAMFILE_H
