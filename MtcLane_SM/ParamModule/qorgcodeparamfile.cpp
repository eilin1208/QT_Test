#include "qorgcodeparamfile.h"
#include <QDebug>
QOrgCodeParamFile::QOrgCodeParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer=&m_container;
}

void QOrgCodeParamFile::InitParamFile()
{
    m_nFileID = 1;
    m_sFileName = getCfgFilePath() + "OrgCode.cfg";
    m_sNewFileName =  getCfgFilePath() + "OrgCode.new";
    m_sDispFileName = "机构编码";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SOrgCodeDataContainer);
}
bool QOrgCodeParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将Key与Value拆分到map中
    SOrgCodeDataContainer * tempContainer = (SOrgCodeDataContainer * )dataBuffer;
    SOrgCodeKey Key;
    SOrgCodeValue Value;
    Key.sOrgCode=tempContainer->szOrgCode;
    Key.wOrgType=qFromBigEndian(tempContainer->wOrgType);
    Value.sOldOrgCode = QString::fromLocal8Bit(tempContainer->szOldOrgCode);
    Value.sOrgCode=Key.sOrgCode;
    Value.wOrgType=Key.wOrgType;
    Value.sLane=QString::fromLocal8Bit(tempContainer->szLane);
    Value.sOrgSName =GB2312toUtf8(tempContainer->szOrgSName);
    Value.sOrgFName =GB2312toUtf8(tempContainer->szOrgFName);
    //    Value.sOrgSName = QString::fromLocal8Bit(tempContainer->szOrgSName);
    //    Value.sOrgFName = QString::fromLocal8Bit(tempContainer->szOrgFName);
    Value.sBL_Station=QString::fromLocal8Bit(tempContainer->szBL_Station);
    Value.sBL_SubCenter=QString::fromLocal8Bit(tempContainer->szBL_SubCenter);
    Value.sBL_Road=QString::fromLocal8Bit(tempContainer->szBL_Road);
    Value.sBL_Area=QString::fromLocal8Bit(tempContainer->szBL_Area);
    Value.sBL_Owner=QString::fromLocal8Bit(tempContainer->szBL_Owner);
    Value.sBL_ProvID=QString::fromLocal8Bit(tempContainer->szBL_ProvID);
    Value.sBL_NetID = QString::fromLocal8Bit(tempContainer->szBL_NetID);
    Value.wRegionSN=qFromBigEndian(tempContainer->wRegionSN);
    Value.sIP=QString::fromLocal8Bit(tempContainer->szIP);
    Value.sUseTime= QDateTime::fromString(
                QString::fromLocal8Bit(tempContainer->szUseTime),
                "yyyyMMddhhmmss");
    Value.sExpireTime= QDateTime::fromString(
                QString::fromLocal8Bit(tempContainer->szExpireTime),
                "yyyyMMddhhmmss");
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sVerUseTime= QDateTime::fromString(
                QString::fromLocal8Bit(tempContainer->szVerUseTime),
                "yyyyMMddhhmmss");
    //        //qDebug() << Key.sOrgCode << Key.wOrgType << ":" << Value.sOrgCode << Value.wOrgType << Value.sLane
    //                 << Value.sOrgSName << Value.sOrgFName << Value.sBL_Station << Value.sBL_SubCenter << Value.sBL_Center
    //                 << Value.sBL_Road << Value.sBL_Area << Value.sBL_Owner << Value.sBL_Corp << Value.wRegionSN << Value.sIP
    //                 << Value.sUseTime << Value.sExpireTime << Value.sLastVer << Value.sVerUseTime<<Value.sOldOrgCode;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool operator <(const SOrgCodeKey &lp, const SOrgCodeKey &rp)
{
    if (lp.wOrgType <rp.wOrgType)
        return true;
    else if (lp.wOrgType > rp.wOrgType)
        return false;
    if (lp.sOrgCode < rp.sOrgCode)
        return true;
    else
        return false ;
}
bool QOrgCodeParamFile::getStaList(QList<quint32> &staList)
{
    staList.clear();
    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap<SOrgCodeKey, SOrgCodeValue>::const_iterator it=tempMap.begin();
    for(; it!=tempMap.end(); ++it)
    {
        if (ORG_TYPE_STATION==(int)it->wOrgType)
        {
            QString sStaCode=it->sOrgCode;
            quint32 nStaCode=sStaCode.toULong();
            if (0==nStaCode)
            {
                // 因下发的机构代码有"######"，此时视为正常
                continue;
            }
            staList.push_back(nStaCode);
        }
    }
    return staList.size()>0;
}
QString transOrgCode(quint32 nOrgCode, int nOrgType)
{
    switch (nOrgType)
    {

    case ORG_TYPE_ADM_REGION:
        // NOTE: 在机构字典表中要求的形式如37AABB
        if (nOrgCode<100)
        {
            nOrgCode=370000+nOrgCode*100;
        }
        else if (nOrgCode<10000)
        {
            nOrgCode+=370000;
        }
    }
    char buf[64];
    sprintf(buf, "%.6d", nOrgCode);
    return buf;
}
bool QOrgCodeParamFile::getRegionSeqId(int *pnSeqId, const quint32 nOrgCode, const int nOrgType)
{
    *pnSeqId=0;
    SOrgCodeKey key;
    key.sOrgCode=transOrgCode(nOrgCode, nOrgType);
    key.wOrgType=nOrgType;
    SOrgCodeValue value;
    if (!m_container.findValue(key, value))
    {
        return false;
    }
    *pnSeqId=value.wRegionSN;
    return true;
}

int QOrgCodeParamFile::getSubOrgList(QVector<SOrgCodeValue> &orgList, int nOrgType, const QString &sOrgCode)
{
    orgList.clear();

    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap<SOrgCodeKey, SOrgCodeValue>::const_iterator it = tempMap.begin();

    QString superOrgCode;

    return orgList.size();
}

bool QOrgCodeParamFile::queryOrgCodeValue(SOrgCodeValue &orgCodeValue, quint32 nOrgCode, const int nOrgType)
{
    SOrgCodeKey key;
    key.sOrgCode = tr("%1").arg(nOrgCode,6,10,QLatin1Char('0'));
    key.wOrgType=nOrgType;
    return m_container.findValue(key, orgCodeValue);
}

bool QOrgCodeParamFile::getCenterOrgList(QVector<SOrgCodeValue> &orgList)
{
    orgList.clear();

    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap<SOrgCodeKey, SOrgCodeValue>::const_iterator it = tempMap.begin();
    for (;it != tempMap.end();++it)
    {
        SOrgCodeValue codeValue=it.value();
        if(ORG_TYPE_CENTER == codeValue.wOrgType)
        {
            orgList.append(codeValue);
        }
    }
    return true;
}

bool QOrgCodeParamFile::GetOrgName(char *szStaId, QString &sOrgName, quint16 wOrgType)
{
    SOrgCodeKey tempKey;
    SOrgCodeValue tempValue;
    tempKey.sOrgCode = QString::fromLocal8Bit(szStaId);
    tempKey.wOrgType = wOrgType;
    bool ok = m_container.findValue(tempKey, tempValue);
    if(ok)
    {
        sOrgName = tempValue.sOrgSName;
        return true;
    }
    return false;

}

bool QOrgCodeParamFile::getOrgList(quint16 wOrgType, const quint32 dwOrgCode, QVector<SOrgCodeValue> &orgList)
{
    orgList.clear();
    SOrgCodeKey tempKey;
    SOrgCodeValue tempValue;
    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap< SOrgCodeKey , SOrgCodeValue>::const_iterator it = tempMap.begin();
    for(; it != tempMap.end(); it++)
    {
        tempKey = it.key();
        tempValue = it.value();
        if(tempKey.wOrgType != wOrgType)
        {
            continue;
        }

        if(wOrgType == ORG_TYPE_STATION)
        {
            if(tempValue.sBL_Road.toUInt() != dwOrgCode && dwOrgCode != 0)
            {
                continue;
            }
        }
        else if(wOrgType == ORG_TYPE_ROAD)
        {
            if(tempKey.wOrgType != ORG_TYPE_ROAD)
            {
                continue;
            }
        }
        else
        {
            continue;
        }
        orgList.append(tempValue);
    }
    return true;
}

bool QOrgCodeParamFile::GetNewStaCode(quint32& dwNew, quint32 dwOld)
{
    SOrgCodeValue pOrg;
    bool Result = false;
    QMutex mutex;
    mutex.lock();
    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap< SOrgCodeKey , SOrgCodeValue>::const_iterator it = tempMap.begin();
    for(;it != tempMap.end();it++)
    {
        pOrg = it.value();
        if(dwOld == pOrg.sOldOrgCode.toInt() && ORG_TYPE_STATION == pOrg.wOrgType)
        {
            dwNew = pOrg.sOrgCode.toInt();
            Result = true;
            break;
        }
    }
    mutex.unlock();
    return Result;
}

bool QOrgCodeParamFile::GetOldStaCode(quint32& dwOld, quint32 dwNew)
{
    SOrgCodeValue pOrg;
    bool Result = false;
    QMutex mutex;
    mutex.lock();
    QMap<SOrgCodeKey, SOrgCodeValue> tempMap=m_container.getMap();
    QMap< SOrgCodeKey , SOrgCodeValue>::const_iterator it = tempMap.begin();
    for(;it != tempMap.end();it++)
    {
        pOrg = it.value();
        if(dwNew == pOrg.sOrgCode.toInt() && ORG_TYPE_STATION == pOrg.wOrgType)
        {
            dwOld = pOrg.sOldOrgCode.toInt();
            Result = true;
            break;
        }
    }
    mutex.unlock();
    return Result;
}
