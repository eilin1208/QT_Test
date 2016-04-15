#include "freeregionclass.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qorgcodeparamfile.h"

// 查询区间顺序号
bool GetRegionSeqId(int *pnSeqId, const quint32 nOrgCode, const int nOrgType)
{
    QOrgCodeParamFile *pFile;
    if (!getParamFile(&pFile, cfOrgCode))
    {
        return false;
    }
    return pFile->getRegionSeqId(pnSeqId, nOrgCode, nOrgType);
}

// 设置鲁通卡免费区间
void FreeRegionClass::SetFreeRegion(int nFreeRegionType, const quint8 *pbFreeRegion, quint8 bFreeRegionBytes)
{
    m_nFreeRegionType=nFreeRegionType;
    if (frNone!=nFreeRegionType)
    {
        m_bGongWuKaType=gtNone;
        const size_t MAX_FREE_REGION_SIZE=sizeof m_FreeRegion;
        memcpy(m_FreeRegion, pbFreeRegion, qMin(MAX_FREE_REGION_SIZE, (size_t)bFreeRegionBytes));
        if (MAX_FREE_REGION_SIZE>bFreeRegionBytes)
        {
            memset(m_FreeRegion+bFreeRegionBytes, 0, MAX_FREE_REGION_SIZE-bFreeRegionBytes);
        }
    }
}

// 设置老公务卡免费区间
void FreeRegionClass::SetGongWuKaQuJian(quint8 bGongWuKaType, const quint32 *pdwYouXiaoQuJian, quint8 bYouXiaoQuJianNum)
{
    m_bGongWuKaType=bGongWuKaType;
    m_GongWuKaQuJian.clear();
    quint32 dwQuJian;
    if (gtNone!=bGongWuKaType)
    {
        memset(m_FreeRegion, 0, sizeof m_FreeRegion);
        if (gtQuanLuWang==bGongWuKaType)
        {
            m_nFreeRegionType=frNone;
        }
        else
        {
            // 因公务卡有效区间需要按鲁通卡格式发送至后台，此处将公务卡区间转换成鲁通卡格式，
            // 但不按照鲁通卡格式计费，并忽略转换过程中可能出现的错误
            // 注意转换过程可能需要花费较长时间
            m_nFreeRegionType=frRoadOwner;
            for (int i=0;i<bYouXiaoQuJianNum;i++)
            {
                dwQuJian=*(pdwYouXiaoQuJian+i);
                m_GongWuKaQuJian.push_back(dwQuJian);
                MarkGongWuKaFreeRegion(dwQuJian);
            }
        }
    }
}

// 测试是否为免费区间
bool FreeRegionClass::IsFreeRegion(bool *pbHasError, quint32 dwRoadOwner, quint32 dwAdmRegion, quint32 dwSta1, quint32 dwSta2) const
{
    if (gtNone!=m_bGongWuKaType)
    {
        // 按老公务卡免费区间判断
        *pbHasError=false;
        return IsGongwukaQujian(dwRoadOwner);
    }
    else if (frNone!=m_nFreeRegionType)
    {
        if (frRoadOwner==m_nFreeRegionType)
        {
            return IsFreeRegion_RoadOwner(pbHasError, dwRoadOwner);
        }
        else if (frAdmRegion==m_nFreeRegionType)
        {
            return IsFreeRegion_AdmRegion(pbHasError, dwAdmRegion);
        }
        else if (frNeighborSta==m_nFreeRegionType || frNeighborStaPlus==m_nFreeRegionType)
        {
            return IsFreeRegion_Sta(pbHasError, dwSta1, dwSta2);
        }
        else
        {
            *pbHasError=true;
            return false;
        }
    }
    else
    {
        // 两种免费区间都未定义，为通行卡或普通鲁通卡
        *pbHasError=false;
        return false;
    }
}

bool FreeRegionClass::IsGongwukaQujian(quint32 dwRoadOwner) const
{
    if (gtQuanLuWang==m_bGongWuKaType)
    {
        // 全路网免费
        return true;
    }
    else if (gtBuFenLuDuanGuaZhang==m_bGongWuKaType)
    {
        // 这种区间由后台处理，无论是否实际在区间内，皆返回真
        return true;
    }
    else if (gtBuFenLuDuanJiaoFei==m_bGongWuKaType)
    {
        if (dwRoadOwner<10000)
        {
            // NOTE: 收费单元字典表定义的路段可能形式如1802，在下面的判断中要求的形式如180200，故乘以100再进行判断
            dwRoadOwner*=100;
        }
        QList<quint32>::const_iterator it;
        for (it=m_GongWuKaQuJian.begin();it!=m_GongWuKaQuJian.end();it++)
        {
            // 注意公务卡区间定义可能是分中心和中心
            if (dwRoadOwner==*it)
            {
                return true;
            }
            if (0==*it%10000)
            {
                if (dwRoadOwner/10000*10000==*it)
                {
                    return true;
                }
            }
        }
    }
    // 未定义类型，或者未找到区间
    return false;
}

bool FreeRegionClass::IsFreeRegion_RoadOwner(bool *pbHasError, quint32 dwRoadOwner) const
{

    return true;
}

bool FreeRegionClass::IsFreeRegion_AdmRegion(bool *pbHasError, quint32 dwAdmRegion) const
{
    int nSeqId=0;
    if (!GetRegionSeqId(&nSeqId, dwAdmRegion, ORG_TYPE_ADM_REGION))
    {
        *pbHasError=true;
        return false;
    }
    *pbHasError=false;
    return IsFreeRegion(nSeqId);
}

bool FreeRegionClass::IsFreeRegion_Sta(bool *pbHasError, quint32 dwSta1, quint32 dwSta2) const
{
    bool bRlt;
    bRlt=IsFreeRegion_Sta(pbHasError, dwSta1);
    if (!bRlt)
    {
        return false;
    }
    return IsFreeRegion_Sta(pbHasError, dwSta2);
}

bool FreeRegionClass::IsFreeRegion_Sta(bool *pbHasError, quint32 dwSta) const
{
    int nSeqId=0;
    bool bRlt;
    bRlt=GetRegionSeqId(&nSeqId, dwSta, ORG_TYPE_STATION);
    if (!bRlt)
    {
        // 收费站可能是实际收费站，也可能是立交虚拟点
        bRlt=GetRegionSeqId(&nSeqId, dwSta, ORG_TYPE_VIRTUAL_STA);
    }
    if (!bRlt)
    {
        *pbHasError=true;
        return false;
    }
    *pbHasError=false;
    return IsFreeRegion(nSeqId);
}

bool FreeRegionClass::IsFreeRegion(int nSeqId) const
{
    // 假定nSeqId从1开始
    if (nSeqId<=0)
    {
        return false;
    }
    int nBytesIndex=(nSeqId-1)/8;
    int nBitIndex=(nSeqId-1)%8;
    if (nBytesIndex>=(int)sizeof(m_FreeRegion))
    {
        return false;
    }
    quint8 bByteToTest=m_FreeRegion[nBytesIndex];
    quint8 bBaseByte=0x80;
    bBaseByte=(bBaseByte>>nBitIndex);
    return (0!=(bBaseByte & bByteToTest));
}

void FreeRegionClass::MarkSeqIdAsFreeRegion(int nSeqId)
{
    // 假定nSeqId从1开始
    if (nSeqId<=0)
    {
        // 忽略错误
        return;
    }
    int nBytesIndex=(nSeqId-1)/8;
    int nBitIndex=(nSeqId-1)%8;
    if (nBytesIndex>=(int)sizeof(m_FreeRegion))
    {
        // 越界了，忽略该错误
        return;
    }
    quint8 bBaseByte=0x80;
    bBaseByte=(bBaseByte>>nBitIndex);
    m_FreeRegion[nBytesIndex]|=bBaseByte;
}

void FreeRegionClass::MarkGongWuKaFreeRegion(quint32 nQuJian)
{
}
