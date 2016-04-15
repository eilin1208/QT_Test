#include "qcardrangeparamfile.h"
#include <QtEndian>
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QCardRangeParamFile::QCardRangeParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QCardRangeParamFile::InitParamFile()
{
    m_nFileID = 37;
    m_sFileName = getCfgFilePath() + "CardRange.cfg";
    m_sNewFileName =  getCfgFilePath() + "CardRange.new";
    //不知道名称
    m_sDispFileName = "包交卡字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SCardRangeContainer);
}
bool QCardRangeParamFile::loadParam_addRow(const char *dataBuffer)
{
    SCardRangeContainer * tempContainer = (SCardRangeContainer *)dataBuffer;
    SCardRangeValue Value;
    quint32 key;
    key = atoi(tempContainer->szCardID);
    Value.sCardID = QString::fromLocal8Bit(tempContainer->szCardID);
    Value.sOccurTime = QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szOccurTime),"yyyyMMddhhmmss");
    Value.sExpireTime = QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szExpireTime),"yyyyMMddhhmmss");
    Value.wFreeStationCnt = qFromBigEndian(tempContainer->wFreeStationCnt);
    //Value.sFreeInfo = QString::fromLocal8Bit(tempContainer->szFreeInfo);
    //Value.sFreeInfo = GB2312toUtf8(tempContainer->szFreeInfo);
    qMemCopy(Value.sFreeInfo,tempContainer->szFreeInfo,sizeof(tempContainer->szFreeInfo));
    Value.szLastVer = QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.szVerUseTime = QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime),"yyyyMMddhhmmss");
    ////qDebug()<<Value.sCardID<<Value.sOccurTime<<Value.sExpireTime<<Value.wFreeStationCnt<<
              //Value.sFreeInfo<<Value.szLastVer<<Value.szVerUseTime;
    m_container.addItemToTemp(key,Value);
    return true;
}

bool QCardRangeParamFile::QueryCardRange(quint32 cardID,SCardRangeValue& value)
{
   return m_container.findValue(cardID,value);
}
