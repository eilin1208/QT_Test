#include "qholidayparamfile.h"
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QHolidayParamFile::QHolidayParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QHolidayParamFile::InitParamFile()
{
    m_nFileID = 39;
    m_sFileName = getCfgFilePath() + "HolidayFree.cfg";
    m_sNewFileName = getCfgFilePath() + "HolidayFree.new";
    m_sDispFileName = "节假日免费字典";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = false;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SHolidayContainer);
}

bool QHolidayParamFile::loadParam_addRow(const char *dataBuffer)
{
    static int counter = 0;
    counter++;
    SHolidayContainer* container = (SHolidayContainer*)dataBuffer;
    int key = counter;
    SHolidayValue value;
    value.desc = GB2312toUtf8(container->szDesc);
    value.vc1 = qFromBigEndian(container->vc1);
    value.vc2 = qFromBigEndian(container->vc2);
    value.vc3 = qFromBigEndian(container->vc3);
    value.vc4 = qFromBigEndian(container->vc4);
    value.vc11 = qFromBigEndian(container->vc11);
    value.vc12 = qFromBigEndian(container->vc12);
    value.vc13 = qFromBigEndian(container->vc13);
    value.vc14 = qFromBigEndian(container->vc14);
    value.vc15 = qFromBigEndian(container->vc15);
    value.vc21 = qFromBigEndian(container->vc21);
    value.vc22 = qFromBigEndian(container->vc22);
    value.overRate = qFromBigEndian(container->overRate);
    value.freeBegin = QDateTime::fromString(container->szFreeBegin, "yyyyMMddhhmmss");
    value.freeEnd = QDateTime::fromString(container->szFreeEnd, "yyyyMMddhhmmss");
    value.lastVer = container->szLastVer;
    value.useTime = container->szUseTime;
    m_container.addItemToTemp(key, value);
    //qDebug() << value.desc << value.vc1 << value.vc2 << value.vc3 << value.vc4 << value.vc11
             //<< value.vc12 << value.vc13 <<value.vc14 <<value.vc15 << value.vc21 <<value.vc22
             //<< value.overRate << value.freeBegin << value.freeEnd << value.lastVer
             //<< value.useTime;
    return true;
}

bool QHolidayParamFile::getUsedValue(SHolidayValue &value)
{
    bool ret = false;
    //qDebug() << "holiday free size" << m_container.getMap().size();
    QMap<int, SHolidayValue>::const_iterator it = m_container.getMap().begin();
    for(it; it != m_container.getMap().end(); it++)
    {
        //qDebug() << it.value().freeBegin << QDateTime::currentDateTime() << it.value().freeEnd;
        if(it.value().freeBegin <= QDateTime::currentDateTime() &&
                it.value().freeEnd >= QDateTime::currentDateTime())
        {
            value = it.value();
            ret = true;
            break;
        }
    }
    return ret;
}

bool QHolidayParamFile::getVTFreeValue(int vehClass, int &rate)
{
    //qDebug() << "holiday free veh class" << vehClass;
    bool ret = false;
    SHolidayValue value;
    ret = getUsedValue(value);
    if(ret)
    {
        switch(vehClass)
        {
        case 1:
            rate = value.vc1;
            break;
        case 2:
            rate = value.vc2;
            break;
        case 3:
            rate = value.vc3;
            break;
        case 4:
            rate = value.vc4;
            break;
        case 11:
            rate = value.vc11;
            break;
        case 12:
            rate = value.vc12;
            break;
        case 13:
            rate = value.vc13;
            break;
        case 14:
            rate = value.vc14;
            break;
        case 15:
            rate = value.vc15;
            break;
        case 21:
            rate = value.vc21;
            break;
        case 22:
            rate = value.vc22;
            break;
        default:
            rate = 100;
            break;
        }
    }
    return ret;
}
