#include "qdissectionparamfile.h"

QDisSectionParamFile::QDisSectionParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QDisSectionParamFile::InitParamFile()
{
    m_nFileID = 14;
    m_sFileName = getCfgFilePath() + "CardBList.cfg";
    m_sNewFileName =  getCfgFilePath() + "CardBList.new";
    m_sDispFileName = "卡黑名单字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SDisSectionContainer);
}
bool QDisSectionParamFile::loadParam_addRow(const char *dataBuffer)
{
    return true;
}

bool operator <(const SDisSectionKey &a, const SDisSectionKey &b)
{

    if(a.sSectionID<b.sSectionID)
        return true;
    else if(a.sSectionID>b.sSectionID)
        return false;
    if(a.sRoadID<b.sRoadID)
        return true;
    else
        return false;
}
