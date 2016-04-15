#ifndef QPARAMMGR_H
#define QPARAMMGR_H

#include <QObject>
#include <QMap>
#include "CommonModule/LaneType.h"
#include "qbaseparamfile.h"
#include <QThread>
class QParamMgr : public QObject
{
    Q_OBJECT
public:
    explicit QParamMgr(QObject *parent = 0);

signals:
    void FileLoadSuccess(int cfg);
    void FileLoadFail(int cfg);
    void AllFileLoadFinished();
    void AskFile(int cfgFile, bool bNew);
public slots:
public:
    //通过枚举值获取参数文
    QBaseParamFile* GetParamFile(CCfgFile cfgFile);
    //通过文件ID获取参数文件
    QBaseParamFile* GetParamFileById(quint32 nFileId);
    //初始化所有参数文件对象信
    void InitParamFile(int nLaneType);
    //通过文件ID获取枚举
    CCfgFile GetFileCfgById(quint32 nFileId);
    //获取文件信息(id和版本号)
    bool GetFileInfoStr(CCfgFile cfgFile, QString &sFileInfo, bool bNew);
    bool GetFileInfoStr(CCfgFile cfgFileBegin,
                                       CCfgFile cfgFileEnd,
                                       QString& sFileInfo,
                                       int& nFileNum,
                                       bool bNew);
    //加载所有参数文
    bool LoadAllParamFile();

    //根据时间判断是否有参数需要调入生效
    void LoadOutUseTimeParam();

    bool IsLoadPass()
    {
        return m_bLoadPass;
    }
protected:
    QBaseParamFile* CreateParamFile(CCfgFile cfgFile, int nLaneType);
private:
    QMap< CCfgFile, QBaseParamFile* > m_fileMap;
    bool m_bLoadPass;
};
QParamMgr* getParamMgr();

template<class T>
inline bool getParamFile(T **ppFile, CCfgFile cfgFile)
{
    QParamMgr *pMgr=getParamMgr();
    QBaseParamFile *pTmpFile=pMgr->GetParamFile(cfgFile);
    if (NULL==pTmpFile)
    {
        return false;
    }
    *ppFile=(T *)(pTmpFile);
    return true;
}
#endif // QPARAMMGR_H
