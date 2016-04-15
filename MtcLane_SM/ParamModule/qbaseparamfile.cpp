#include <QtEndian>
#include <QFile>
#include "qbaseparamfile.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include "CommonModule/laneutils_new.h"
#include "LogModule/qfilelogger.h"
bool SParamFileHead::GetValueByRaw(SParamFileHeadRaw &raw)
{
    nFileID = QString::fromLocal8Bit(raw.szFileID, 2).toInt();
    dwTotalLen = qFromBigEndian(raw.dwTotalLen);
    dwRecordCount = qFromBigEndian(raw.dwRecordCount);
    dwRecLength = qFromBigEndian(raw.dwRecLength);
    lVersion = QDateTime::fromString((char*)&raw.szVersion,"yyyyMMddhhmmss");
    EncryptFlag = raw.EncryptFlag;
    tUseTime = QDateTime::fromString((char*)&raw.szUseTime,"yyyyMMddhhmmss");
    qMemCopy(szVerifyCode, raw.VerifyCode, sizeof raw.VerifyCode);
    return tUseTime.isValid();
}

QBaseParamFile::QBaseParamFile(QObject *parent) :
    QObject(parent), m_bBootMust(true), m_bDownLoadOnTime(false), m_bHasNewVersion(false)
{
    m_pContainer=NULL;
}

bool QBaseParamFile::VerifyFileHead(const QString & sFileName, SParamFileHead& cfgFileHead)
{
    // 打开文件
    QFile paramFile(sFileName);
    if(paramFile.open(QFile::ReadOnly) == false)
    {
        if(!sFileName.contains("new"))
        {
            LogMsg("lane", tr("校验文件：%1,文件打开失败").arg(sFileName));
        }
        return false;
    }

    // 校验文件大小
    qint64 nActualLength = paramFile.size();
    if((nActualLength < sizeof(SParamFileHeadRaw)) || (nActualLength > 0x2000000))
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,文件大小%2错误").arg(sFileName).arg(nActualLength),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1,文件大小%2错误").arg(sFileName).arg(nActualLength));
        return false;
    }

    // 读取文件头
    SParamFileHeadRaw tempFileHeadRow;
    quint32 nFileHeadLength = sizeof(SParamFileHeadRaw);
    quint64 nReadLength = paramFile.read((char *)&tempFileHeadRow, nFileHeadLength);

    if(nReadLength != nFileHeadLength)
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,读取文件错误，应读%2，实读%3").arg(sFileName).arg(nFileHeadLength)
//                                                           .arg(nReadLength),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane",tr("校验文件：%1,读取文件错误，应读%2，实读%3").arg(sFileName).arg(nFileHeadLength)
                                                                          .arg(nReadLength) );
        return false;
    }

    // 校验文件头
    if(cfgFileHead.GetValueByRaw(tempFileHeadRow) == false)
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1:文件头转换出错").arg(sFileName),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1:文件头转换出错").arg(sFileName));
        return false;
    }

    // 检验版本号
    if(!cfgFileHead.lVersion.isValid())
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,版本号错误，读取到版本号为：%2").arg(sFileName).arg(cfgFileHead.lVersion),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1,版本号错误，读取到版本号为：%2").arg(sFileName).arg(cfgFileHead.lVersion.toString("yyyyMMddhhmmss")));
        return false;
    }

    // 检验文件ID
    if(m_nFileID != cfgFileHead.nFileID)
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,文件ID错误，应为%2，实为%3").arg(m_nFileID)
//                                                          .arg(cfgFileHead.nFileID),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1,文件ID错误，应为%2，实为%3").arg(GetFileName()).arg(m_nFileID).arg(cfgFileHead.nFileID));
        return false;
    }

    // 检验文件长度
    if(nActualLength != cfgFileHead.dwTotalLen)
    {
        paramFile.close();
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,文件长度错误，应为%2，实为%3").arg(sFileName).arg(cfgFileHead.dwTotalLen)
//                                                          .arg(nActualLength),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1,文件长度错误，应为%2，实为%3").arg(sFileName).arg(cfgFileHead.dwTotalLen).arg(nActualLength));
        return false;
    }

    // 读取文件数据部分，用于MD5校验
    int nDataLength = nActualLength - nFileHeadLength;
    char * dataBuffer = new char[nDataLength];
    int nActualDataLength = paramFile.read(dataBuffer, nDataLength);
    if(nActualDataLength != nDataLength)
    {
        paramFile.close();
        delete [] dataBuffer;
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,文件数据读取长度错误，应为%2，实为%3").arg(sFileName).arg(nDataLength)
//                                                          .arg(nActualDataLength),
//                              QFileLogger::LOG_ERROR);
        LogMsg("lane", tr("校验文件：%1,文件数据读取长度错误，应为%2，实为%3").arg(sFileName).arg(nDataLength).arg(nActualDataLength));
        return false;
    }
    paramFile.close();
//2012年7月1日20:47:57-----由于云南程序中MD5校验值为空，因此不进行MD5校验
//Modify by 祥凯
//    // MD5校验
//    QString md5 = QMD5Checksum::GetMD5ForBuffer(dataBuffer, nDataLength);
//    delete [] dataBuffer;
//    QString readMd5 = QString::fromLocal8Bit(tempFileHeadRow.VerifyCode, 32);
//    if(md5.compare(readMd5, Qt::CaseInsensitive) != 0)
//    {
//        QFileLogger::WriteLog("lane",
//                              tr("校验文件：%1,文件校验错误，应为%2，实为%3").arg(sFileName).arg(readMd5).arg(md5),
//                              QFileLogger::LOG_ERROR);
//        return false;
//    }
    //liujian delete
    delete [] dataBuffer;
    return true;
}

bool QBaseParamFile::LoadParamFile(QString sFileName, SParamFileHead &cfgFileHead, bool& isNew)
{
    SParamFileHead fileHead;
    // 校验文件头
    if(VerifyFileHead(sFileName, fileHead) == false)
    {
//        QFileLogger::WriteLog("lane",
//                              tr("加载文件%1,校验文件失败").arg(sFileName),
//                              QFileLogger::LOG_ERROR);
        return false;
    }

    ////qDebug()<<tr("LoadParamFile FileHead Success %1").arg(sFileName);

    // 判断时间,确定为加载或者预加载
    QDateTime curTime = QDateTime::currentDateTime();
    //qDebug() << sFileName <<"uss time: " << fileHead.tUseTime << "cur time" << curTime;
    if(fileHead.tUseTime < curTime)
    {
        isNew = false;
        if(sFileName.compare(m_sNewFileName) == 0)
        {
            RenameParamFile(m_sNewFileName, m_sFileName);
            sFileName = m_sFileName;
        }
    }
    else
    {
        isNew = true;

    }
    //根据是否为预加载文件，更新所对应保存的文件头信息
    SetCfgFileHead(fileHead, isNew);
    return LoadData(sFileName, isNew);
}

bool QBaseParamFile::LoadData(const QString &sFileName, bool bNew)
{
    // 检查每一条记录的长度
    SParamFileHead realHead = bNew ? spNewFileHead : spFileHead;
    //qDebug() << realHead.dwRecLength << m_nDataContainerLength;
    if((int)realHead.dwRecLength != m_nDataContainerLength)
    {
        return false;
    }
    // 检查文件数据总长度
    if(realHead.dwTotalLen < realHead.dwRecordCount * m_nDataContainerLength
            +  sizeof(SParamFileHeadRaw))
    {
        return false;
    }

    // 打开文件跳过文件头
    QFile paramFile(sFileName);
    if(paramFile.open(QFile::ReadOnly) == false)
    {
        return false;
    }

    paramFile.seek(sizeof(SParamFileHeadRaw));

    char * dataBuffer = new char[m_nDataContainerLength];

    loadParam_begin();
    // 循环加载文件
    //dataBuffer赋值?
    for(int i = 0; i < (int)realHead.dwRecordCount; i++)
    {
        memset(dataBuffer, 0, m_nDataContainerLength);
        qint64 nRlt=paramFile.read(dataBuffer, m_nDataContainerLength);
        if (nRlt!=(qint64)m_nDataContainerLength)
        {
            loadParam_cancel();
            delete [] dataBuffer;
            return false;
        }
        loadParam_addRow(dataBuffer);
    }
    //如果是未启用文件则进行测试加载,并不执行loadParam_complete
    if(bNew == true)
    {
        delete [] dataBuffer;
        return true;
    }
    //qDebug() << sFileName << bNew;
    loadParam_complete();

    delete [] dataBuffer;

    return true;
}

QDateTime QBaseParamFile::GetVersion(bool bNew)
{
    if(bNew)
    {
        return spNewFileHead.lVersion;
    }
    else
    {
        return spFileHead.lVersion;
    }
}

void QBaseParamFile::SetCfgFileHead(const SParamFileHead &cfgFileHead, bool bNew)
{
    if(bNew)
    {
        spNewFileHead = cfgFileHead;
        m_bHasNewVersion = true;
    }
    else
    {
        spFileHead = cfgFileHead;
    }
    // update display;
}

bool QBaseParamFile::GetCfgFileHead(SParamFileHead &cfgFileHead, bool bNew)
{
    if(bNew)
    {
        if(spNewFileHead.lVersion.isValid())
        {
            cfgFileHead = spNewFileHead;
            return true;
        }
    }
    else
    {
        if(spFileHead.lVersion.isValid())
        {
            cfgFileHead = spFileHead;
            return true;
        }
    }
    return false;
}

bool QBaseParamFile::RenameParamFile(const QString &sOldName, const QString &sNewName)
{
    //这里没有重置文件的属性！
    return MoveFileReplaceExisting(sOldName, sNewName);
    return true;
}
QString getCfgFilePath()
{
    if(!QFile::exists("./Lane.ini"))
    {
        return "";
    }
    else
    {
        QSettings settings("./Lane.ini", QSettings::IniFormat);
        return settings.value("LaneInfo/ParamPath").toString();
    }
}

bool QBaseParamFile::CheckAndLoadParamFile()
{
    SParamFileHead fileHead;
    bool isNew;
    bool ret = LoadParamFile(GetNewFileName(), fileHead, isNew);
    if(ret == true && isNew == true)
    {
        emit LoadSuccess(true);
    }
    //当新版参数文件未加载成功，或者加载成功但是未启用时，加载当前版本cfg文件
    if(ret == false || (ret == true && isNew == true))
    {
        ret = LoadParamFile(GetFileName(), fileHead, isNew);
    }
    if(ret)
    {
        emit LoadSuccess(false);
    }
    return ret;
}
