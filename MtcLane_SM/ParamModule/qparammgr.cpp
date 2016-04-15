#include "qparammgr.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "ParamModule/qvccodeparamfile.h"
#include "ParamModule/qdissectionparamfile.h"
#include "ParamModule/qbaserateparamfile.h"
#include "ParamModule/qtollcellparamfile.h"
#include "ParamModule/qsprateparamfile.h"
#include "ParamModule/qallrateparamfile.h"
#include "ParamModule/qareacodeparamfile.h"
#include "ParamModule/qcardlistparamfile.h"
#include "ParamModule/qvblistparamfile.h"
#include "ParamModule/qpsamwhiteparamfile.h"
#include "ParamModule/qvglistparamfile.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "ParamModule/qlaneeventcodeparamfile.h"
#include "ParamModule/qoperparamfile.h"
#include "ParamModule/qshiftparamfile.h"
#include "ParamModule/qcardrangeparamfile.h"
#include "ParamModule/qlanesoundparamfile.h"
#include "ParamModule/qholidayparamfile.h"
QParamMgr::QParamMgr(QObject *parent) :
    QObject(parent)
{
    m_bLoadPass = false;
}

void QParamMgr::InitParamFile(int nLaneType)
{
    for (int i=0;i<int(cfEnd);i++)
    {
        QBaseParamFile *pFile=m_fileMap[CCfgFile(i)];
        if (NULL!=pFile)
        {
            delete pFile;
        }
        m_fileMap[CCfgFile(i)] = CreateParamFile(CCfgFile(i), nLaneType);
    }
}

QBaseParamFile * QParamMgr::CreateParamFile(CCfgFile cfgFile, int nLaneType)
{
    QBaseParamFile *pFile=NULL;
    switch(cfgFile)
    {
    case cfOrgCode:
        pFile = new QOrgCodeParamFile();
        break;
    case cfVCCode:
        pFile = new QVCCodeParamFile();
        break;
    case cfVCTrans:
        //pFile = new
        break;
    case cfDisSection:
        //        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        //        {
        //            pFile = new QDisSectionParamFile();
        //        }
        break;

    case cfBaseRate:
        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        {
            pFile = new QBaseRateParamFile();
        }
        break;
    case cfTollCell:
        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        {
            pFile = new QTollCellParamFile();
        }
        break;
    case cfSpRate:
        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        {
            pFile = new QSpRateParamFile();
        }
        break;
    case cfAllRate:
        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        {
            pFile = new QAllRateParamFile();
        }
        break;
    case cfAreaCode:
        pFile = new QAreaCodeParamFile();
        break;
    case cfCardBList:
        pFile = new QCardListParamFile();
        break;
    case cfVBList:
        pFile = new QVBlistParamFile();
        break;
    case cfPSamWhite:
        //pFile = new QPSamWhiteParamFile(this);
        break;
    case cfVGList:
        pFile = new QVGListParamFile();
        break;
    case cfSysParaDic:
        pFile = new QSysParaDicParamFile();
        break;
    case cfLaneEvenCode:
        //pFile = new QLaneEventCodeParamFile(this);
        break;
    case cfSpWeighPara:
        break;
    case cfOper:
        pFile = new QOperParamFile();
    case cfPublish:
        break;
    case cfShift:
        pFile = new QShiftParamFile();
        break;
        //包交卡区间
    case cfCardRange:
        if(LaneType_MtcOut == nLaneType)
        {
            pFile = new QCardRangeParamFile();
        }
        break;
    case cfLaneSound:
        //pFile = new QLaneSoundParamFile(this);
        break;
    case cfLaneKey:
        break;
    case cfHolidayFree:
        if(LaneType_MtcOut == nLaneType || LaneType_EtcOut == nLaneType)
        {
            pFile = new QHolidayParamFile();
        }
    case cfEnd:
        break;
    default:
        break;
    }
    if(pFile != NULL)
    {
        pFile->SetBootMust(true);
        pFile->InitParamFile();
    }
    return pFile;
}

QBaseParamFile * QParamMgr::GetParamFile(CCfgFile cfgFile)
{
    return m_fileMap.value(cfgFile, NULL);
}

QBaseParamFile * QParamMgr::GetParamFileById(quint32 nFileId)
{
    QMap<CCfgFile, QBaseParamFile* >::const_iterator it = m_fileMap.begin();
    QBaseParamFile *pTmpFile = NULL;
    for(; it != m_fileMap.end(); it++)
    {
        pTmpFile = it.value();
        if(pTmpFile != NULL)
        {
            if(pTmpFile->GetFileID() == nFileId)
            {
                return pTmpFile;
            }
        }
    }
    return NULL;
}

CCfgFile QParamMgr::GetFileCfgById(quint32 nFileId)
{
    QMap<CCfgFile, QBaseParamFile* >::const_iterator it = m_fileMap.begin();
    QBaseParamFile *pTmpFile = NULL;
    for(; it != m_fileMap.end(); it++)
    {
        pTmpFile = it.value();
        if(pTmpFile != NULL)
        {
            if(pTmpFile->GetFileID() == nFileId)
            {
                return it.key();
            }
        }
    }
    return cfEnd;
}

bool QParamMgr::GetFileInfoStr(CCfgFile cfgFile, QString &sFileInfo, bool bNew)
{
    QBaseParamFile * pFile = m_fileMap.value(cfgFile, NULL);

    if(pFile != NULL)
    {
        int nFileId = pFile->GetFileID();
        QDateTime dtFileVer = pFile->GetVersion(bNew);
        QString sVersion;
        if(!dtFileVer.isValid())
        {
            sVersion = "00000000000000";
        }
        else
        {
            sVersion = dtFileVer.toString("yyyyMMddhhmmss");
        }
        sFileInfo = tr("%1%2").arg(nFileId, 2, 10, QLatin1Char('0')).arg(sVersion);
        return true;
    }
    return false;
}
bool QParamMgr::GetFileInfoStr(CCfgFile cfgFileBegin,
                               CCfgFile cfgFileEnd,
                               QString& sFileInfo,
                               int& nFileNum,
                               bool bNew)
{
    if(cfgFileBegin > cfgFileEnd)
    {
        return false;
    }
    sFileInfo.clear();
    nFileNum = 0;
    for(int i = int(cfgFileBegin); i < int(cfgFileEnd); i++)
    {
        QString sSingleFileInfo;
        if(GetFileInfoStr((CCfgFile)i, sSingleFileInfo, bNew))
        {
            sFileInfo.append(sSingleFileInfo);
            nFileNum++;
        }
    }
    return nFileNum > 0;
}

bool QParamMgr::LoadAllParamFile()
{
    QMap< CCfgFile, QBaseParamFile*>::const_iterator it;
    //先请求下载所有参数文件的新版本和正在使用版本
    for(it = m_fileMap.begin(); it != m_fileMap.end(); it++)
    {
        qDebug() << "开始下载参数" << it.key();
        if(it.value() != NULL)
        {
            emit AskFile(it.key(), true);
            emit AskFile(it.key(), false);
        }
    }
    //添加下载结束符
    emit AskFile(cfEnd, false);
    return true;
}

void QParamMgr::LoadOutUseTimeParam()
{
    QBaseParamFile* pFile = NULL;
    SParamFileHead tmpHead;
    QDateTime curTime = QDateTime::currentDateTime();
    for(int i = 0; i < cfEnd; i++)
    {
        pFile = GetParamFile((CCfgFile)i);
        if(pFile != NULL)
        {
            if(pFile->GetHasNewVersion())
            {
                if(pFile->GetCfgFileHead(tmpHead, true))
                {
                    if(tmpHead.tUseTime < curTime)
                    {
                        pFile->CheckAndLoadParamFile();
                    }
                }
            }
        }
    }
}

QParamMgr* getParamMgr()
{
    static QParamMgr mgr;
    return &mgr;
}

