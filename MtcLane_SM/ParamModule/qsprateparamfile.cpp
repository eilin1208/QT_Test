
#include "qsprateparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QtEndian>
#include <QDebug>
QSpRateParamFile::QSpRateParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QSpRateParamFile::InitParamFile()
{
    m_nFileID = 10;
    m_sFileName = getCfgFilePath() + "SpRate.cfg";
    m_sNewFileName =  getCfgFilePath() + "SpRate.new";
    m_sDispFileName = "例外费率";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SSpRateRowContainer);
}
bool QSpRateParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SSpRateRowContainer * tempContainer = (SSpRateRowContainer * )dataBuffer;
    SSpRateKey Key;
    SSpRateValue Value;
    quint32 dwStation1 = atol(tempContainer->szSpStation1);
    quint32 dwStation2 = atol(tempContainer->szSpStation2);
    if(dwStation1 < dwStation2)
    {
        Key.dwSpStation1=dwStation1;
        Key.dwSpStation2=dwStation2;
    }
    else
    {
        Key.dwSpStation1=dwStation2;
        Key.dwSpStation2=dwStation1;
    }
    Key.wVC=qFromBigEndian(tempContainer->wVC);

    Value.dwSpStation1=Key.dwSpStation1;
    Value.dwSpStation2=Key.dwSpStation2;
    Value.wVC=Key.wVC;
    Value.sRateDesc=GB2312toUtf8(tempContainer->szRateDesc);
    Value.wSpVCRate=qFromBigEndian(tempContainer->wSpVCRate);
    Value.wSpTrunkMinRate=qFromBigEndian(tempContainer->wSpTrunkMinRate);
    Value.sSpVCRateFlag=QString::fromLocal8Bit(tempContainer->szSpVCRateFlag);
    Value.wSpFlagStation=qFromBigEndian(tempContainer->wSpFlagStation);
    Value.sLastVer=tempContainer->szLastVer;
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime,14), "yyyyMMddhhmmss");
    //    //qDebug() << Key.dwSpStation1 << Key.dwSpStation2 << Key.wVC << ":" << Value.dwSpStation1 << Value.dwSpStation2
    //                 << Value.wVC << Value.sRateDesc << Value.wSpTrunkMinRate << Value.sSpVCRateFlag << Value.wSpFlagStation
    //                 << Value.sLastVer << Value.sVerUseTime<<"load seccess";
    m_container.addItemToTemp(Key, Value);
    return true;
}
bool operator <(const SSpRateKey &a, const SSpRateKey &b)
{
    if (a.dwSpStation1<b.dwSpStation1)
        return true;
    else if (a.dwSpStation1 > b.dwSpStation1)
        return false;

    if (a.dwSpStation2<b.dwSpStation2)
        return true;
    else if (a.dwSpStation2 > b.dwSpStation2)
        return false;

    if (a.wVC<b.wVC)
        return true;
    else
        return false;
}

bool QSpRateParamFile::querySpecialTollInfo(SSpRateValue &specialRate, quint32 nSta1, quint32 nSta2, int nVehClass)
{
    SSpRateKey key;
    if (nSta1 < nSta2)
    {
        key.dwSpStation1 = nSta1;
        key.dwSpStation2 = nSta2;
    }
    else
    {
        key.dwSpStation1 = nSta2;
        key.dwSpStation2 = nSta1;
    }
    key.wVC = nVehClass;
    return m_container.findValue(key, specialRate);
}
