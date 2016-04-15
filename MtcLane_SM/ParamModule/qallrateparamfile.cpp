#include "qallrateparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QDebug>
QAllRateParamFile::QAllRateParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QAllRateParamFile::InitParamFile()
{
    m_nFileID = 11;
    m_sFileName = getCfgFilePath() + "AllRate.cfg";
    m_sNewFileName =  getCfgFilePath() + "AllRate.new";
    m_sDispFileName = "总收费费率字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SAllRateContainer);
}
bool QAllRateParamFile::loadParam_addRow(const char *dataBuffer)
{
    SAllRateContainer *tempContainer = (SAllRateContainer * )dataBuffer;
    quint32 dwSta1,dwSta2;
    SAllRateValue value;
    SAllRateKey Key;
    dwSta1 = ArCharToInt(tempContainer->szStationIn,sizeof(tempContainer->szStationIn));
    dwSta2 = ArCharToInt(tempContainer->szStationOut,sizeof(tempContainer->szStationOut));
    //将数据转化为key与value并转存在map(内存)中
    if (dwSta1 < dwSta2)
       {
           Key.nSta1=dwSta1;
           Key.nSta2=dwSta2;
       }
       else
       {
           Key.nSta1=dwSta2;
           Key.nSta2=dwSta1;
       }
    value.dwStationIn = atoi(tempContainer->szStationIn);
    value.dwStationOut = atoi(tempContainer->szStationOut);
    value.wStationNum = qFromBigEndian(tempContainer->wStationNum);
    value.sPathInfo = QString::fromLocal8Bit(tempContainer->szPathInfo);
    //添加解析经过收费站信息tPassStaAry
    if(!ParsePassStationInfo(value.sPathInfo,value.wStationNum,value.tPassStaAry))
    return false;
    value.wCellNum = qFromBigEndian(tempContainer->wCellNum);
    value.dwTollDistance = qFromBigEndian(tempContainer->dwTollDistance);
    value.dwRealDistance = qFromBigEndian(tempContainer->dwRealDistance);
    value.szWayFlag = QString::fromLocal8Bit(tempContainer->szWayFlag);
    value.sTollInfo = QString::fromLocal8Bit(tempContainer->szTollInfo);
    //添加价格解析tPriceAry
    if(!ParseVCPrice(value.sTollInfo,value.tPriceAry))
    return false;
    value.tmLastVer = QString::fromLocal8Bit(tempContainer->szLastVer);
    value.tmVerUsertime = QString::fromLocal8Bit(tempContainer->szVerUseTime);
    m_container.addItemToTemp(Key,value);
    return true;
}

bool operator <(const SAllRateKey &a, const SAllRateKey &b)
{
    if (a.nSta1<b.nSta1)
        return true;
    else if (a.nSta1>b.nSta1)
        return false;

    if (a.nSta2<b.nSta2)
        return true;
    else
        return false;
}
bool QAllRateParamFile::queryTollValue(SAllRateValue &tollValue, quint32 nSta1, quint32 nSta2)
{
    SAllRateKey key;
    if (nSta1 < nSta2)
    {
        key.nSta1 = nSta1;
        key.nSta2 = nSta2;
    }
    else
    {
        key.nSta1 = nSta2;
        key.nSta2 = nSta1;
    }

    return m_container.findValue(key, tollValue);
}

bool QAllRateParamFile::queryFarthestStaAndMinimumFare(quint32 *pFarthestSta,
    quint16 *pMinimumFareInYuan, QList<quint32> enStaList, quint32 nSta2)
{
    QList<quint32>::const_iterator it;
    SAllRateValue tollValue;
    quint32 nEnSta;
    quint32 nCandidateSta=0;
    quint16 nMaxPrice=0;
    quint16 nMinFare=15;
    int nErrorStaCount=0;
    for (it=enStaList.begin(); it!=enStaList.end(); it++)
    {
        nEnSta=*it;
        if (nEnSta==nSta2)
        {
            continue;
        }
        if (!queryTollValue(tollValue, nEnSta, nSta2))
        {
            // NOTE: 因机构编码表中经常出现已撤销收费站，但收费表中又无相关里程信息，
            //	故认为个别收费站查询不到收费里程为正常现象
            nErrorStaCount++;
            continue;
        }
        quint16 nPrice;
        // 取货5的通行费
        if (!tollValue.getPrice(nPrice, 15))
        {
            return false;
        }
        if (nPrice>nMaxPrice)
        {
            nMaxPrice=nPrice;
            nCandidateSta=nEnSta;
        }
        // 取客1的通行费
        if (!tollValue.getPrice(nPrice, 1))
        {
            return false;
        }
        if (nPrice<nMinFare)
        {
            nMinFare=nPrice;
        }
    }
    if (0==nCandidateSta)
    {
        return false;
    }
    if (nErrorStaCount>10)
    {
        return false;
    }
    *pFarthestSta=nCandidateSta;
    *pMinimumFareInYuan=nMinFare;
    return true;
}

bool QAllRateParamFile::ParsePassStationInfo(const QString& sPathInfo, quint16 nStaCount, QList<quint32> &aryStations)
{
    int i;
    QString subStr;
    bool ret = false;
    if(sPathInfo.length() < nStaCount * 6)
    return ret;
    for(i = 0;i<=nStaCount-1;i++)
    {
        subStr = sPathInfo.mid(i*6,6);
        //aryStations[i] = subStr.toInt();
        aryStations.append(subStr.toInt());
    }
    return true;
}

bool QAllRateParamFile::ParseVCPrice(const QString& sTollInfo, QMap<int, quint16> &aryToll)
{
    int VCCount = 9;
    int aryVC[] = {1,2,3,4,11,12,13,14,15};
    int i;
    QString subStr;
    bool ret = false;
    if(sTollInfo.length() < VCCount * 4)
        return ret;
    for(i = 0;i<= VCCount -1;i++)
    {
        subStr = sTollInfo.mid(i*4,4);
        if(aryToll.contains(aryVC[i]))
        {
            aryToll[aryVC[i]] = subStr.toInt();
        }
        else
        {
            aryToll.insert(aryVC[i],subStr.toInt());
        }
    }
    return true;
}
