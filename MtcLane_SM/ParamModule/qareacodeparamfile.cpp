
#include "qareacodeparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QDebug>
QAreaCodeParamFile::QAreaCodeParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

bool operator <(const SAreaCodeKey &a, const SAreaCodeKey &b)
{
    if (a.sAreaCode<b.sAreaCode)
        return true;
    else if(a.sAreaCode>b.sAreaCode)
        return false;

    if (a.sRoad<b.sRoad)
        return true;
    else
        return false;
}

void QAreaCodeParamFile::InitParamFile()
{
    m_nFileID = 13;
    m_sFileName = getCfgFilePath() + "AreaCode.cfg";
    m_sNewFileName =  getCfgFilePath() + "AreaCode.new";
    m_sDispFileName = "区域编码";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SAreaCodeContainer);
}
bool QAreaCodeParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SAreaCodeContainer * tempContainer = (SAreaCodeContainer * )dataBuffer;
    SAreaCodeKey Key;
    SAreaCodeValue Value;
    Key.sAreaCode=QString::fromLocal8Bit(tempContainer->szAreaCode);
    Key.sRoad=QString::fromLocal8Bit(tempContainer->szRoad);
    Value.sAreaCode=Key.sAreaCode;
    Value.sRoad=Key.sRoad;
    Value.sAreaname=GB2312toUtf8(tempContainer->szAreaname);
    Value.wStoreCardAgio=qFromBigEndian(tempContainer->wStoreCardAgio);
    Value.wTallyCardAgio=qFromBigEndian(tempContainer->wTallyCardAgio);
    Value.sSpVTFlag=QString::fromLocal8Bit(tempContainer->szSpVTFlag);
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    //Value.sVerUseTime=CharArrayToDateTime(tempContainer->szVerUseTime);
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime),"yyyyMMddhhmmss");
    //--DEBUG
    //    //qDebug() << Key.sAreaCode << Key.sRoad << ":" << Value.sAreaCode << Value.sRoad <<
    //                Value.sAreaname<< Value.wStoreCardAgio << Value.wTallyCardAgio <<
    //                Value.sSpVTFlag << Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool QAreaCodeParamFile::QAreaCodeParamFile::QryAreaCode(const QString &sAreaCode, SAreaCodeValue *pValue)
{
    if(m_container.getMap().isEmpty())
        return false;
    QMap<SAreaCodeKey, SAreaCodeValue>::const_iterator it = m_container.getMap().begin();
    for(;it != m_container.getMap().end(); it++)
    {
        if(sAreaCode == ((SAreaCodeKey)it.key()).sAreaCode)
        {
            *pValue = it.value();
            return true;
        }
    }
    return false;
}


