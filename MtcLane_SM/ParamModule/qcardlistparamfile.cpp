#include "qcardlistparamfile.h"
#include <QtEndian>
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QCardListParamFile::QCardListParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

bool operator <(const SCardBListKey &a, const SCardBListKey &b)
{
    if (a.sCardNet<b.sCardNet)
        return true;
    else if (a.sCardNet >b.sCardNet)
        return false;
    if (a.sCard<b.sCard)
        return true;
    else
        return false;
}
void QCardListParamFile::InitParamFile()
{
    m_nFileID = 14;
    m_sFileName = getCfgFilePath() + "CardBList.cfg";
    m_sNewFileName =  getCfgFilePath() + "CardBList.new";
    m_sDispFileName = "卡黑名单字典";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SCardBListContainer);
}
bool QCardListParamFile::loadParam_addRow(const char *dataBuffer)
{
    //qDebug() << "begin load black list param file...";
    // 将数据转化为key与value，存放在map中
    SCardBListContainer * tempContainer = (SCardBListContainer * )dataBuffer;
    SCardBListKey Key;
    SCardBListValue Value;
    Key.sCard=QString::fromLocal8Bit(tempContainer->szCard);
    //Key.wCardType=qFromBigEndian(wCardType);
    Key.sCardNet=QString::fromLocal8Bit(tempContainer->szCardNet);
    Value.sCard=Key.sCard;
    Value.wCardType=qFromBigEndian(tempContainer->wCardType);
    Value.sCardNet=Key.sCardNet;
    Value.wBlistType=qFromBigEndian(tempContainer->wBlistType);
    Value.sBListDesc=GB2312toUtf8(tempContainer->szBListDesc);
    Value.sOccurTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szOccurTime),"yyyyMMddhhmmss");
    Value.sExpireTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szExpireTime),"yyyyMMddhhmmss");

    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime),"yyyyMMddhhmmss");

    //qDebug() << Key.sCard << Key.sCardNet << ":" << Value.sCard << Value.wCardType << Value.sCardNet
    //         << Value.wBlistType <<Value.sBListDesc<< Value.sOccurTime << Value.sExpireTime << Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool QCardListParamFile::IsBlackCard(const QString &sCard, const QString &sCardNet, quint8 &wCardType,quint16 &wBListType,QString &sBListDesc)
{
    SCardBListKey key;
    key.sCard = sCard;
    key.sCardNet = sCardNet;
    key.wCardType = wCardType;
    if(m_container.getMap().find(key) != m_container.getMap().end())
    {
        //wCardType = m_container.getValue(key).wCardType;
        wBListType = m_container.getValue(key).wBlistType;
        sBListDesc = m_container.getValue(key).sBListDesc;
        return true;
    }
    return false;
}
