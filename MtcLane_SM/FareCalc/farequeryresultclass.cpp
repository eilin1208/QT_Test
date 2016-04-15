#include <QList>
#include "farequeryresultclass.h"
#include "FareCalcFunc.h"

// 对分段计算结果进行汇总、取整
void FareQueryResultClass::SumSectMoney(const quint32 dwMinMoney)
{
    m_fCentDue=0;
    m_fCentFree=0;
    m_fCentOverweightDue=0;
    QList<CSectMoney>::const_iterator it;
    for (it=m_SplitInfo.begin();it!=m_SplitInfo.end();it++)
    {
        m_fCentDue+=it->m_fCentDue;
        m_fCentFree+=it->m_fCentFree;
        m_fCentOverweightDue+=it->m_fCentOverweightDue;
    }
    double fCentTotal=m_fCentDue+m_fCentFree;
    //wumax 修改为四舍五入     quint32 dwMoneyTotal=roundFareTo5Yuan(fCentTotal);
    quint32 dwMoneyTotal=round_int32(fCentTotal / 100.0) * 100;
    if (dwMoneyTotal<dwMinMoney && fCentTotal>0)
    {
        dwMoneyTotal=dwMinMoney;
    }
    //wumax 修改为四舍五入       m_dwMoneyDue=roundFareTo5Yuan(m_fCentDue);
    m_dwMoneyDue=round_int32(m_fCentDue / 100.0) * 100;
    if (m_dwMoneyDue<dwMinMoney && m_fCentDue>0)
    {
        m_dwMoneyDue=dwMinMoney;
    }
    m_dwMoneyFree=dwMoneyTotal-m_dwMoneyDue;
    if (m_fCentDue>0)
    {
        double fNormalMoney=m_fCentDue-m_fCentOverweightDue;
        //wumax 修改为四舍五入         quint32 dwNormalMoney=roundFareTo5Yuan(fNormalMoney);
        quint32 dwNormalMoney=round_int32(fNormalMoney / 100.0) * 100;
        if (dwNormalMoney<dwMinMoney)
        {
            dwNormalMoney=dwMinMoney;
        }
        m_dwMoneyOverweightDue=m_dwMoneyDue-dwNormalMoney;
    }
    else
    {
        m_dwMoneyOverweightDue=0;
    }
}

// 对计算结果（折扣后结果）进行拆分
void FareQueryResultClass::SplitMoney()
{
    if (0==m_SplitInfo.size())
    {
        // 对U型、无卡、换卡车等情况，使用特殊拆分，计算结果中不含拆分信息
        return;
    }
    double fMaxCentDue=-1, fMaxCentFree=-1;
    quint32 dwSumDue=0, dwSumFree=0;
    QList<CSectMoney>::iterator it;
    QList<CSectMoney>::iterator pMaxDue, pMaxFree;
    for (it=m_SplitInfo.begin(),pMaxDue=it,pMaxFree=it;it!=m_SplitInfo.end();it++)
    {
        if (it->m_fCentDue>fMaxCentDue)
        {
            fMaxCentDue=it->m_fCentDue;
            pMaxDue=it;
        }
        if (it->m_fCentFree>fMaxCentFree)
        {
            fMaxCentFree=it->m_fCentFree;
            pMaxFree=it;
        }
        it->m_dwMoneyDue=splitMoney(it->m_fCentDue, m_fCentDue, m_dwDiscountedDue);
        it->m_dwMoneyFree=splitMoney(it->m_fCentFree, m_fCentFree, m_dwDiscountedFree);
        dwSumDue+=it->m_dwMoneyDue;
        dwSumFree+=it->m_dwMoneyFree;
    }
    if (dwSumDue!=m_dwDiscountedDue)
    {
        pMaxDue->m_dwMoneyDue=pMaxDue->m_dwMoneyDue+m_dwDiscountedDue-dwSumDue;
    }
    if (dwSumFree!=m_dwDiscountedFree)
    {
        pMaxFree->m_dwMoneyFree=pMaxFree->m_dwMoneyFree+m_dwDiscountedFree-dwSumFree;
    }
}

// 修改折扣后金额（注意：拆分基于折扣后金额），并根据修改后的金额重新拆分，用于欠费、分段付款等情况
// 因欠费、分段付款不折扣，此函数将折扣率修改为0
void FareQueryResultClass::ModifyDiscountedMoney(quint32 dwRealDue, bool bClearFree)
{
    m_nDiscountRate=0;
    m_dwDiscountedDue=dwRealDue;
    if (bClearFree)
    {
        m_dwDiscountedFree=0;
    }
    SplitMoney();
}

// 进行折扣，折扣后重新拆分
void FareQueryResultClass::Discount(int nDiscountRate)
{
    m_nDiscountRate=nDiscountRate;
    m_dwDiscountedDue=discountedMoney(m_dwMoneyDue, nDiscountRate);
    m_dwDiscountedFree=discountedMoney(m_dwMoneyFree, nDiscountRate);
    if (m_dwMoneyOverweightDue>0)
    {
        quint32 dwDiscountedUnderweightDue=discountedMoney(m_dwMoneyDue-m_dwMoneyOverweightDue, nDiscountRate);
        m_dwDiscountedOverweightDue=m_dwDiscountedDue-dwDiscountedUnderweightDue;
    }
    else
    {
        m_dwDiscountedOverweightDue=0;
    }
    SplitMoney();
}

void FareQueryResultClass::Multiply(int nTimes)
{
    m_dwTollMeters*=nTimes;
    m_dwMoneyDue*=nTimes;
    m_dwMoneyFree*=nTimes;
    m_dwMoneyOverweightDue*=nTimes;
    m_dwDiscountedDue*=nTimes;
    m_dwDiscountedFree*=nTimes;
    m_dwDiscountedOverweightDue*=nTimes;
    m_dwMaxSeconds*=nTimes;
    m_dwMinSeconds*=nTimes;
}

// 返回折扣后金额（不更改其中的应收通行费）
quint32 FareQueryResultClass::GetDiscountedDutyMoney(int nDiscountRate) const
{
    return discountedMoney(m_dwMoneyDue, nDiscountRate);
}

// 获取字符串型的拆分结果，返回填充的拆分路段数
int FareQueryResultClass::GetSplitInfo(char *szSplitInfo, size_t nSizeOfSplitInfo) const
{
    if (m_SplitInfo.empty())
    {
        return 0;
    }
    // 每段拆分结果的大小，格式为"RRRRDDDDDDDFFFFFFF"，其中4字节RRRR为小业主代码，7字节DDDDDDD为应收拆分金额（单位为分），
    //	7字节FFFFFFF为免收拆分金额（单位为分）
    const int SECT_SIZE=18;
    size_t nSplitNum = m_SplitInfo.size();
    if (nSplitNum*SECT_SIZE > nSizeOfSplitInfo)
    {
        return 0;
    }
    QList<CSectMoney>::const_iterator it = m_SplitInfo.begin();
    int i=0;
    for (;it != m_SplitInfo.end();++it)
    {
        char szSecInfo[SECT_SIZE+65]={'\0'};
        sprintf(szSecInfo, "%.4u%.7u%.7u",
            it->m_dwOrgCode,
            it->m_dwMoneyDue,
            it->m_dwMoneyFree);
        memcpy(szSplitInfo+i*SECT_SIZE,szSecInfo,SECT_SIZE);
        i++;
    }
    return i;
}

// 获取6位站代码组成字符串，返回值为收费站数量
int FareQueryResultClass::GetStationInfo(char *szStationInfo, size_t nSizeOfStationInfo) const
{
    if (m_PassStaList.empty())
    {
        return 0;
    }
    // 每组数据的大小，每组数据只包含6位站代码
    const int SECT_SIZE=6;
    size_t nStaionNum = m_PassStaList.size();
    if (nStaionNum*SECT_SIZE > nSizeOfStationInfo)
    {
        return 0;
    }
    QList<quint32>::const_iterator it = m_PassStaList.begin();
    int i=0;
    for(;it!=m_PassStaList.end();++it)
    {
        char szSecInfo[SECT_SIZE+65]={'\0'};
        sprintf(szSecInfo, "%.6u", *it);
        memcpy(szStationInfo+i*SECT_SIZE, szSecInfo, SECT_SIZE);
        i++;
    }
    return i;
}

// 获取字符串收费单元和收费单元实际里程，返回值为收费单元数
// 8位收费单元组成字符串AAABBBBB，其中AAA为单元代码，BBBBB为实际里程
int FareQueryResultClass::GetTollCellInfo(char *szTollCellInfo, size_t nSizeOfTollCell) const
{
    if (m_PassedTollCellList.empty())
    {
        return 0;
    }
    // 每组数据的大小，格式为AAABBBBB，其中AAA为单元代码，BBBBB为实际里程
    const int SECT_SIZE=8;
    size_t nRealCount = m_PassedTollCellList.size();
    if (nRealCount*SECT_SIZE > nSizeOfTollCell)
    {
        return 0;
    }
    QList<STollCellValue>::const_iterator it = m_PassedTollCellList.begin();
    int i=0;
    for(;it!=m_PassedTollCellList.end();++it)
    {
        char szSecInfo[SECT_SIZE+65]={'\0'};
        sprintf(szSecInfo, "%.3u%.5u",
                it->wCellCode, it->dwRealDistance);
        memcpy(szTollCellInfo+i*SECT_SIZE, szSecInfo, SECT_SIZE);
        i++;
    }
    return i;
}

int FareQueryResultClass::GetFreeInfo(char *szFreeInfo,size_t nSizeOfFreeInfo) const
{
    if (m_FreeInfoList.empty())
    {
        return 0;
    }
    //每段免费区间共15位:路段3+起站6+止站6
    const int SECT_SIZE=15;
    size_t nFreeNum = m_FreeInfoList.size();
    if (nFreeNum*SECT_SIZE > nSizeOfFreeInfo)
    {
        return 0;
    }
    QList<TFreeInfo>::const_iterator it = m_FreeInfoList.begin();
    int i=0;
    for (;it != m_FreeInfoList.end();++it)
    {
        char szSecInfo[SECT_SIZE+65]={'\0'};
        sprintf(szSecInfo, "%.3u%.6u%.6u",
            it->wRoadID%1000,
            it->wBeginSta%1000000,
            it->wEndSta%1000000);
        memcpy(szFreeInfo+i*SECT_SIZE,szSecInfo,SECT_SIZE);
        i++;
    }
    return i;

}
bool FareQueryResultClass::TranFreeInfo()
{
    return true;
}
