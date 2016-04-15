#include "qvblistparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QDebug>
QVBlistParamFile::QVBlistParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QVBlistParamFile::InitParamFile()
{
    m_nFileID = 15;
    m_sFileName = getCfgFilePath() + "VBList.cfg";
    m_sNewFileName =  getCfgFilePath() + "VBList.new";
    m_sDispFileName = "车辆黑名单字典";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SVBListContainer);
}
bool QVBlistParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SVBListContainer * tempContainer = (SVBListContainer * )dataBuffer;
    SVBListKey Key;
    SVBListValue Value;

    char szTmpVLP[13]="";
    //添加处理
    //RemovePlateSpecChar(szTmpVLP,sizeof (szTmpVLP),tempContainer->szVLP);
    Key.sVLP=GB2312toUtf8(tempContainer->szVLP);
    Key.wVLPColor=qFromBigEndian(tempContainer->wVLPColor);

    Value.sVLP=Key.sVLP;
    Value.wVLPColor=Key.wVLPColor;
    Value.wVBType=qFromBigEndian(tempContainer->wVBType);
    Value.sVBTDes =GB2312toUtf8(tempContainer->szVBTDes);
    Value.sOccurTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szOccurTime),"yyyyMMddhhmmss");
    Value.sExpireTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szExpireTime),"yyyyMMddhhmmss");
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime),"yyyyMMddhhmmss");

    //--DEBUG
//    qDebug() << Key.sVLP << Key.wVLPColor << ":" << Value.sVLP << Value.wVLPColor  << Value.wVBType
//             << Value.sOccurTime<<Value.sVBTDes
//             << Value.sExpireTime << Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool operator <(const SVBListKey &a, const SVBListKey &b)
{
    if (a.sVLP<b.sVLP)
        return true;
    else if (a.sVLP >b.sVLP)
        return false;

    if (a.wVLPColor<b.wVLPColor)
        return true;
    else
        return false;
}

bool QVBlistParamFile::IsBlackVeh(const QString &sVLP, quint16 wVLPColor, quint16 &wVBType)
{
    if(m_container.getMap().isEmpty())
        return false;
    QMap<SVBListKey, SVBListValue>::const_iterator it = m_container.getMap().begin();
    for(;it != m_container.getMap().end(); it++)
    {
        if(sVLP == ((SVBListKey)it.key()).sVLP && wVLPColor == ((SVBListKey)it.key()).wVLPColor)
        {
            wVBType = ((SVBListValue)it.value()).wVBType;
            return true;
        }
    }
    return false;
}
bool QVBlistParamFile::IsBlackVeh(const QString &sVLP,quint16 &wVBType)
{
    if(m_container.getMap().isEmpty())
        return false;
    QMap<SVBListKey, SVBListValue>::const_iterator it = m_container.getMap().begin();
    for(;it != m_container.getMap().end(); it++)
    {
        if(sVLP == ((SVBListKey)it.key()).sVLP)
        {
            wVBType = ((SVBListValue)it.value()).wVBType;
            return true;
        }
    }
    return false;
}
