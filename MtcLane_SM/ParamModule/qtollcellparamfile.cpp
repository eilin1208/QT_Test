
#include "qtollcellparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QtEndian>
#include <QDebug>
QTollCellParamFile::QTollCellParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QTollCellParamFile::InitParamFile()
{
    m_nFileID = 9;
    //m_nCfgFileType = cfOper;
    m_sFileName = getCfgFilePath() + "TollCell.cfg";
    //m_sFileName = "SysParaDic.cfg";
    m_sNewFileName = getCfgFilePath() + "TollCell.new";
    m_sDispFileName = "收费单元字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(STollCellRowContainer);
}
bool QTollCellParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    STollCellRowContainer * tempContainer = (STollCellRowContainer * )dataBuffer;
    STollCellKey Key;
    STollCellValue Value;
    Key.wCellCode = atoi(tempContainer->szCellCode);
    Key.dwStation1 = atol(tempContainer->szStation1);
    Key.dwStation2 = atol(tempContainer->szStation2);
    if (Key.dwStation1>Key.dwStation2)
    {
        // 保证dwStation1<dwStation2，查找时也使用该顺序
        quint32 temp = Key.dwStation1;
        Key.dwStation1 = Key.dwStation2;
        Key.dwStation2 = temp;
    }
    Value.wCellCode = Key.wCellCode;
    Value.dwStation1 = Key.dwStation1;
    Value.dwStation2 = Key.dwStation2;

    Value.cCellFlag = tempContainer->szCellFlag[0];
    Value.wStationSN=qFromBigEndian(tempContainer->wStationSN);
    Value.wBL_Road = atoi(tempContainer->szBL_Road) ;
    Value.wBL_Country = atoi(tempContainer->szBL_Country);
    Value.wOwner = atoi(tempContainer->szOwner);

    Value.sWayFlag=QString::fromLocal8Bit(tempContainer->szWayFlay);
    Value.sSpFreeFlag=QString::fromLocal8Bit(tempContainer->szSpFreeFlag);
    Value.sRateCode=QString::fromLocal8Bit(tempContainer->szRateCode);
    Value.dwRealDistance=qFromBigEndian(tempContainer->dwRealDistance);
    Value.dwTollDistance=qFromBigEndian(tempContainer->dwTollDistance);
    Value.wLimitSpeedPara=qFromBigEndian(tempContainer->wLimitSpeedPara);
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szUseTime, 14), "yyyyMMddhhmmss");
    //        //qDebug() << Key.wCellCode << Key.dwStation1 << Key.dwStation2 << ":" << Value.wCellCode
    //                    << Value.dwStation1 << Value.dwStation2 << Value.cCellFlag << Value.wStationSN
    //                       << Value.wBL_Road << Value.wBL_Country << Value.wOwner << Value.sWayFlag
    //                          << Value.sSpFreeFlag << Value.sRateCode << Value.dwRealDistance << Value.dwTollDistance
    //                             <<Value.wLimitSpeedPara << Value.sLastVer << Value.sUseTime;
    m_container.addItemToTemp(Key, Value);
    m_container_cellID.addItemToTemp(Key.wCellCode, Value);
    addRow_neighborSta(Key);
    return true;
}
void QTollCellParamFile::addRow_neighborSta(const STollCellKey &Key)
{
    SNeighborSta neighbor;
    neighbor.nSta1=Key.dwStation1;
    neighbor.nSta2=Key.dwStation2;
    m_container_neighborSta.addItemToTemp_multi(neighbor, Key.wCellCode);
}
bool operator <(const STollCellKey &a, const STollCellKey &b)
{
    if (a.wCellCode < b.wCellCode)
        return true;
    else if (a.wCellCode > b.wCellCode)
        return false;
    if (a.dwStation1 < b.dwStation1)
        return true;
    else if (a.dwStation1 > b.dwStation1)
        return false;
    if (a.dwStation2 < b.dwStation2)
        return true;
    else
        return false;
}

bool QTollCellParamFile::queryCellListByNeighborSta(QList<quint16> &cellList, quint32 nSta1, quint32 nSta2)
{
    SNeighborSta neighbor;
    if (nSta1 < nSta2)
    {
        neighbor.nSta1 = nSta1;
        neighbor.nSta2 = nSta2;
    }
    else
    {
        neighbor.nSta1 = nSta2;
        neighbor.nSta2 = nSta1;
    }
    cellList=m_container_neighborSta.getValues(neighbor);
    return cellList.count()>0;
}

bool QTollCellParamFile::queryTollCellValue(STollCellValue &TollCellValue, int nTollCell)
{
     return m_container_cellID.findValue(nTollCell, TollCellValue);
}
