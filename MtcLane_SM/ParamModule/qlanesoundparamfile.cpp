#include "qlanesoundparamfile.h"

QLaneSoundParamFile::QLaneSoundParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QLaneSoundParamFile::InitParamFile()
{
    m_nFileID = 37;
    m_sFileName = getCfgFilePath() + "LaneSound.cfg";
    m_sNewFileName =  getCfgFilePath() + "LaneSound.new";
    //不知道名称
    m_sDispFileName = "语音文件";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SLaneSoundContainer);
}
bool QLaneSoundParamFile::loadParam_addRow(const char *dataBuffer)
{
    return true;
}

bool operator <(const SLaneSoundKey &a, const SLaneSoundKey &b)
{
    if (a.wSoundEvent<b.wSoundEvent)
        return true;
    else if (a.wSoundEvent > b.wSoundEvent)
        return false;

    if (a.nEnExFlag<b.nEnExFlag)
        return true;
    else if (a.nEnExFlag > b.nEnExFlag)
        return false;

    if (a.sVCFlag<b.sVCFlag)
        return true;
    else
        return false;
}
