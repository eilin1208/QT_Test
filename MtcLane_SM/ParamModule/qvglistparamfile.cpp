
#include "qvglistparamfile.h"
#include <QtEndian>
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QVGListParamFile::QVGListParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QVGListParamFile::InitParamFile()
{
    m_nFileID = 17;
    m_sFileName = getCfgFilePath() + "VGList.cfg";
    m_sNewFileName =  getCfgFilePath() + "VGList.new";
    m_sDispFileName = "车辆灰名单字典";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SVGListContainer);
}
bool QVGListParamFile::loadParam_addRow(const char *dataBuffer)
{
    SVGListContainer * tempContainer = (SVGListContainer *)dataBuffer;
    SVGListKey Key;
    SVGListValue Value;
    char szTmpVLP[13]="";
    //RemovePlateSpecChar(szTmpVLP,sizeof (szTmpVLP),tempContainer->szVLP);
    Key.sVLP=szTmpVLP;
    Key.wVLPColor=qFromBigEndian(tempContainer->wVLPColor);  
    Value.sVLP=Key.sVLP;
    Value.wVLPColor=Key.wVLPColor;
    Value.wVGType=qFromBigEndian(tempContainer->wVGType);
    Value.sVGTDes = tempContainer->VGTDes;
    Value.sOccurTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szOccurTime));
    Value.sExpireTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szExpireTime));
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    //Value.sVerUseTime=CharArrayToDateTime(tempContainer->szVerUseTime);
    //     //qDebug() << Key.sVLP << Key.wVLPColor << Value.sVLP << Value.wVLPColor<<Value.wVGType<<Value.sVGTDes
    //                 << Value.sOccurTime << Value.sExpireTime << Value.sLastVer
    //                 << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool operator <(const SVGListKey &a, const SVGListKey &b)
{
    if (a.sVLP<b.sVLP)
        return true;
    else
        return false;
    if (a.wVLPColor<b.wVLPColor)
        return true;
    else
        return false;
    //    if (a.sRecordNum<b.sRecordNum)
    //        return true;
    //    else
    //        return false;
}

bool QVGListParamFile::IsGrayVeh(const QString &sVlp, const quint16 wVlpColor, SVGListValue &value)
{
    if(m_container.getMap().isEmpty())
    {
        return false;
    }
    QMap<SVGListKey, SVGListValue>::const_iterator it = m_container.getMap().begin();
    for(;it != m_container.getMap().end(); it++)
    {
        if(sVlp == ((SVGListKey)it.key()).sVLP && wVlpColor == ((SVGListKey)it.key()).wVLPColor)
        {
            value = it.value();
            return true;
        }
    }
    return false;
}
bool QVGListParamFile::IsGrayVeh(const QString& sVlp, SVGListValue& value)
{
    if(m_container.getMap().isEmpty())
    {
        return false;
    }
    QMap<SVGListKey, SVGListValue>::const_iterator it = m_container.getMap().begin();
    for(;it != m_container.getMap().end(); it++)
    {
        if(sVlp == ((SVGListKey)it.key()).sVLP)
        {
            value = it.value();
            return true;
        }
    }
    return false;
}
