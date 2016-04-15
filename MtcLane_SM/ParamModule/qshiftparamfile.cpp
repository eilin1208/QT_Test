#include "qshiftparamfile.h"
#include "parammapcontainer.h"
#include "CommonModule/laneutils_new.h"
#include <QtEndian>
#include <QDebug>
QShiftParamFile::QShiftParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QShiftParamFile::InitParamFile()
{
    m_nFileID = 25;
    m_sFileName = getCfgFilePath() + "Shift.cfg";
    m_sNewFileName =  getCfgFilePath() + "Shift.new";
    m_sDispFileName = "工班参数字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SShiftRowContainer);
}
bool QShiftParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SShiftRowContainer * tempContainer = (SShiftRowContainer * )dataBuffer;
    quint16 Key;
    SShiftValue Value;
    Key=qFromBigEndian(tempContainer->wShift);
    Value.sStation=QString::fromLocal8Bit(tempContainer->szStation);
    Value.sBLNetID = QString::fromLocal8Bit(tempContainer->szBLNetID);
    Value.szBLRoadID = QString::fromLocal8Bit(tempContainer->szBLRoadID);
    Value.wShift = Key;
    Value.sShiftName=GB2312toUtf8(tempContainer->szShiftName);
    Value.stVerStartTime=QTime::fromString(QString::fromLocal8Bit(tempContainer->szVerStartTime),"hh:mm:ss");
    Value.stVerEndTime=QTime::fromString(QString::fromLocal8Bit(tempContainer->szVerEndTime),"hh:mm:ss");
    Value.wTimeDiff = qFromBigEndian(tempContainer->wTimeDiff);
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    //Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime));
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime),"yyyyMMddhhmmss");
    //--DEBUG
//        //qDebug() << Key << ":" << Value.sStation <<  Value.sBLNetID << Value.szBLRoadID << Value.wShift
//                 << Value.sShiftName << Value.stVerStartTime<< Value.stVerEndTime<< Value.wTimeDiff <<
//                    Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

QDate QShiftParamFile::getWorkDate(const QDateTime &time, quint16 wShiftNum)
{
    SShiftValue firstValue = getFirstShift();
    if(firstValue.wShift != wShiftNum && time.time() <= firstValue.stVerStartTime)
    {
        return time.date().addDays(-1);
    }
    else
    {
        return time.date();
    }
}

void QShiftParamFile::getShiftInfoByTime(const QDateTime &time, QList<SShiftValue> &usableShift)
{
    QTime startTime;
    QTime endTime;
    int index = 0;
    SShiftValue tmpValue;
    QMap< quint16, SShiftValue >::const_iterator it = m_container.getMap().begin();
    for(; it != m_container.getMap().end(); it++)
    {
        tmpValue = it.value();
        if(tmpValue.wTimeDiff >= 60)
        {
            tmpValue.wTimeDiff = (tmpValue.wTimeDiff / 60 % 12) * 60 ;
        }
        startTime = tmpValue.stVerStartTime;
        endTime = tmpValue.stVerEndTime;
        if(startTime < endTime)
        {
            if((startTime <= time.time() && time.time() < endTime) ||
                    (startTime <= time.addSecs(tmpValue.wTimeDiff * 60).time() &&
                     time.addSecs(tmpValue.wTimeDiff * 60).time() < endTime))
            {
                index++;
                //计算工作日
                tmpValue.iWorkDate = getWorkDate(time, tmpValue.wShift);
                usableShift.append(tmpValue);
            }
        }
        if(startTime > endTime)
        {
            if((startTime <= time.time() || time.time() < endTime) ||
                    (startTime <= time.addSecs(tmpValue.wTimeDiff * 60).time() ||
                     time.addSecs(tmpValue.wTimeDiff * 60).time() < endTime))
            {
                index++;
                tmpValue.iWorkDate = getWorkDate(time, tmpValue.wShift);
                usableShift.append(tmpValue);
            }
        }
    }
}

bool QShiftParamFile::getFirstUsableShift(SShiftValue &shiftParam, bool bDelay)
{
    QDateTime curTime = QDateTime::currentDateTime();
    QList<SShiftValue> usableShift;
    getShiftInfoByTime(curTime, usableShift);
    if(usableShift.isEmpty())
    {
        return false;
    }
    shiftParam = usableShift.first();
    return true;
}

SShiftValue QShiftParamFile::getShiftInfoByNo(quint16 wShiftNo)
{
    SShiftValue tmpValue = m_container.getValue(wShiftNo);
    tmpValue.iWorkDate = getWorkDate(QDateTime::currentDateTime(), tmpValue.wShift);
    return tmpValue;
}

SShiftValue QShiftParamFile::getFirstShift()
{
    return m_container.getMap().values().first();
}
