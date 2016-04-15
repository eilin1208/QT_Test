#include "axisinfo.h"
#include <QMutex>
#include <FareCalc/FareCalcFunc.h>
#include <QDebug>
CLaneAxisInfo CLaneAxisInfo::m_LaneAxisInfo;
AxisInfo::AxisInfo(QObject *parent) :
    QObject(parent)
{
}
const double g_fWeightDevErroRatio = 0.98;
CAxisGroupData::CAxisGroupData(int nAxisGroupType, double fSpeed, const long AxisWeightArry[], int nMaxAxisWeightArryLen)
{
    m_nAxisGroupType = nAxisGroupType;
    m_fSpeed = fSpeed;
    m_nAxisGroupWeightRaw = 0;
    m_nAxisGroupweight = 0;
    m_nAxisNum = 0;
    m_AxisList.clear();
    for(int i = 0;i<nMaxAxisWeightArryLen;i++)
    {
        if(AxisWeightArry[i] == 0)
            break;
        CSingleAxisData SingleAxisData;
        SingleAxisData.m_nWeightRaw = AxisWeightArry[i];
        SingleAxisData.m_nWeight = qRound(SingleAxisData.m_nWeightRaw*g_fWeightDevErroRatio);
        m_AxisList.push_back(SingleAxisData);
        m_nAxisGroupWeightRaw+=SingleAxisData.m_nWeightRaw;
        m_nAxisGroupweight+=SingleAxisData.m_nWeight;
        m_nAxisNum++;
    }
}
bool CAxisGroupData::operator ==(const CAxisGroupData &AxisData)
{
    return
            (this->m_nAxisGroupType==AxisData.m_nAxisGroupType) &&
            (this->m_nAxisGroupWeightRaw==AxisData.m_nAxisGroupWeightRaw) &&
            (this->m_nAxisGroupweight==AxisData.m_nAxisGroupweight) &&
            (this->m_fSpeed==AxisData.m_fSpeed) &&
            (this->m_nAxisNum==AxisData.m_nAxisNum) &&
            (this->m_AxisList==AxisData.m_AxisList);
}
bool CAxisGroupData::operator !=(const CAxisGroupData &AxisData)
{
    return
            (this->m_nAxisGroupType!=AxisData.m_nAxisGroupType) ||
            (this->m_nAxisGroupWeightRaw!=AxisData.m_nAxisGroupWeightRaw) ||
            (this->m_nAxisGroupweight!=AxisData.m_nAxisGroupweight) ||
            (this->m_fSpeed!=AxisData.m_fSpeed) ||
            (this->m_nAxisNum!=AxisData.m_nAxisNum) ||
            (this->m_AxisList!=AxisData.m_AxisList);
}
void CAxisGroupData::operator =(const CAxisGroupData &AxisData)
{
    this->m_nAxisGroupType=AxisData.m_nAxisGroupType;
    this->m_nAxisGroupWeightRaw=AxisData.m_nAxisGroupWeightRaw;
    this->m_nAxisGroupweight=AxisData.m_nAxisGroupweight;
    this->m_fSpeed=AxisData.m_fSpeed;
    this->m_nAxisNum=AxisData.m_nAxisNum;
    this->m_AxisList=AxisData.m_AxisList;
}
bool operator ==(const CAxisGroupData &AxisData1, const CAxisGroupData &AxisData2)
{
    return
            (AxisData1.m_nAxisGroupType==AxisData2.m_nAxisGroupType) &&
            (AxisData1.m_nAxisGroupWeightRaw==AxisData2.m_nAxisGroupWeightRaw) &&
            (AxisData1.m_nAxisGroupweight==AxisData2.m_nAxisGroupweight) &&
            (AxisData1.m_fSpeed==AxisData2.m_fSpeed) &&
            (AxisData1.m_nAxisNum==AxisData2.m_nAxisNum) &&
            (AxisData1.m_AxisList==AxisData2.m_AxisList);
}
void CVehAxisInfo::operator =(const CVehAxisInfo &VehAxisInfo)
{
    this->WeightModifyStatus=VehAxisInfo.WeightModifyStatus;
    this->WeightStandardStatus=VehAxisInfo.WeightStandardStatus;
    this->WeightCut=VehAxisInfo.WeightCut;
    this->m_fVehSpeed=VehAxisInfo.m_fVehSpeed;
    this->m_nRawTotalWeight=VehAxisInfo.m_nRawTotalWeight;
    this->m_dwTotalWeight=VehAxisInfo.m_dwTotalWeight;
    this->m_dwInputedWeightLimit=VehAxisInfo.m_dwInputedWeightLimit;
    this->m_RawAxisList=VehAxisInfo.m_RawAxisList;
    this->m_AutoAxisList=VehAxisInfo.m_AutoAxisList;
}
quint32 CVehAxisInfo::GetAutoTotalWeight() const
{
    return GetTotalWeightOf(m_AutoAxisList);
}
quint32 CVehAxisInfo::GetWeightStandard() const
{
    if (0!=m_dwInputedWeightLimit)
    {
        return m_dwInputedWeightLimit;
    }
    else
    {
        return getWeightLimit(GetAxisCount());
    }
}
quint32 CVehAxisInfo::GetOverWeightPer() const
{
    int nOverweightRate;
    CalcWeightLimitAndOverweightRate(nOverweightRate,GetAxisCount(),m_dwTotalWeight,m_dwInputedWeightLimit);
    return nOverweightRate;
}
quint32 CVehAxisInfo::GetMinAxisWeight() const
{
    quint32 dwMinWeight=0x0FFFFFFF;
    QList<CAxisGroupData>::const_iterator it_axis_group;
    for (it_axis_group=m_RawAxisList.begin();it_axis_group!=m_RawAxisList.end();++it_axis_group)
    {
        QList<CSingleAxisData>::const_iterator it_axis;

        for (it_axis=it_axis_group->m_AxisList.begin();it_axis!=it_axis_group->m_AxisList.end();++it_axis)
        {
            if (it_axis->m_nWeight<dwMinWeight)
            {
                dwMinWeight=it_axis->m_nWeight;
            }
        }
    }
    return dwMinWeight;
}
quint32 CVehAxisInfo::GetAxisGroupOf(const QList<CAxisGroupData> &AxisList)
{
    // 娉ㄦ剰4瀛楄妭鏃犵鍚堟暣鍨嬫渶澶у彧鑳借〃绀294967295锛屼负閬垮厤婧㈠嚭锛
    // 鍦ㄨ酱缁勭粍鎴愯秴鍑浣嶆椂锛屽彧杩斿洖鍓浣
    quint32 dwAxisGroup=0;
    int i=0;
    QList<CAxisGroupData>::const_iterator it;
    for (it=AxisList.begin();it!=AxisList.end();++it)
    {
        dwAxisGroup=dwAxisGroup*10+(it->m_nAxisGroupType%10);
        i++;
        if (i>=9)
        {
            break;
        }
    }
    return dwAxisGroup;
}
quint32 CVehAxisInfo::GetTotalWeightOf(const QList<CAxisGroupData> &AxisList)
{
    quint32 nWeight=0;
    QList<CAxisGroupData>::const_iterator it;
    for (it=AxisList.begin();it!=AxisList.end();++it)
    {
        nWeight+=it->m_nAxisGroupweight;
    }
    return nWeight;
}
void CVehAxisInfo::GetAxisInfoStr(char *szAxisInfo,size_t nSizeOfAxisInfo,const QList<CAxisGroupData> &AxisList)
{
    QList<CAxisGroupData>::const_iterator it;
    size_t offset=0;
    memset(szAxisInfo, 0, nSizeOfAxisInfo);
    char szTmpAxis[8];
    for (it=AxisList.begin();it!=AxisList.end();++it)
    {
        if ((offset+7)>nSizeOfAxisInfo)
        {
            break;
        }
        //snprintf_s(szTmpAxis, sizeof szTmpAxis, _TRUNCATE, "%d%.6d", it->m_nAxisGroupType%10, it->m_nAxisGroupWeight%1000000);
        memcpy(szAxisInfo+offset,szTmpAxis,7);
        offset+=7;
    }
    return;
}
// 璁＄畻鍚勮酱缁勯€熷害鐨勭畻鏈钩鍧囧€硷紝浣滀负骞冲潎杞﹂€
double CVehAxisInfo::GetAverageVehSpeed(const QList<CAxisGroupData> &AxisList)
{
    double fTotalSpeed=0;
    int nCount=0;
    QList<CAxisGroupData>::const_iterator it;
    for (it=AxisList.begin(); it!=AxisList.end(); ++it)
    {
        fTotalSpeed+=it->m_fSpeed;
        nCount++;
    }
    if (0==nCount)
    {
        return 0;
    }
    else
    {
        return fTotalSpeed/nCount;
    }
}
int CVehAxisInfo::GetAxisCount( const QList<CAxisGroupData> &AxisList )
{
    int Count=0;
    QList<CAxisGroupData>::const_iterator it;
    for (it=AxisList.begin();it!=AxisList.end();++it)
    {
        Count+=it->m_nAxisNum;
    }
    return Count;
}
// 鍒ゆ柇鏄惁鍙互鍒囧壊
bool CVehAxisInfo::CanSplit() const
{
    if (0==m_RawAxisList.size())
    {
        return false;
    }
    if (!WeightCut)
    {
        // 宸茶繘琛岃繃鍒囧壊鎿嶄綔
        return false;
    }
    return true;
}
bool CVehAxisInfo::Split(CVehAxisInfo *pVeh1, CVehAxisInfo *pVeh2, int nFirstAxisGroupNum) const
{
    QList<CAxisGroupData> AxisGroupList1, AxisGroupList2;
    QList<CAxisGroupData>::const_iterator it;
    int i=0;
    for (it=this->m_RawAxisList.begin();it!=this->m_RawAxisList.end();it++)
    {
        i++;
        if (i<=nFirstAxisGroupNum)
        {
            AxisGroupList1.push_back(*it);
        }
        else
        {
            AxisGroupList2.push_back(*it);
        }
    }
    *pVeh1=*this;
    *pVeh2=*this;
    pVeh1->ModifyAxisGroupList(AxisGroupList1);
    pVeh2->ModifyAxisGroupList(AxisGroupList2);
    pVeh1->WeightCut=false;
    pVeh2->WeightCut=false;
    return true;
}
void CVehAxisInfo::Clear()
{
    WeightModifyStatus=true;
    WeightStandardStatus=true;
    WeightCut=true;
    m_fVehSpeed=0;
    m_nRawTotalWeight=0;
    m_dwTotalWeight=0;
    m_dwInputedWeightLimit=0;
    m_RawAxisList.clear();
    m_AutoAxisList.clear();
}
void CVehAxisInfo::SetAxisGroupList(const QList<CAxisGroupData> &AxisGroupList, double fVehSpeed)
{
    Clear();
    if (0==AxisGroupList.size())
    {
        return;
    }
    ModifyAxisGroupList(AxisGroupList);
    // 杞﹂€
    m_fVehSpeed=fVehSpeed;
    // 淇濆瓨鍘熷杞寸粍淇℃伅
    m_AutoAxisList=m_RawAxisList;
}
void CVehAxisInfo::ModifyAxisGroupList(const QList<CAxisGroupData> &AxisGroupList)
{
    // 娓呴櫎鎵嬪伐杈撳叆杞撮檺
    m_dwInputedWeightLimit=0;
    // 鏇存柊杞寸粍淇℃伅
    m_RawAxisList=AxisGroupList;
    // 瀵硅酱缁勪俊鎭繘琛岀疮璁★紝鑰冭檻璁惧璇樊杩涜璋冩暣锛屽苟纭繚璋冩暣鍚庡悇杞寸粍閲嶉噺涔嬪拰涓庢€婚噸閲忕浉鍚
    QList<CAxisGroupData>::iterator it;
    QList<CAxisGroupData>::iterator pMaxWeightAxisGroup;
    int nSumOfRawAxisWeight=0;
    int nSumOfAxisWeight=0;
    quint32 dwMaxAxisWeight=0;
    for (it=m_RawAxisList.begin();it!=m_RawAxisList.end();it++)
    {
        nSumOfRawAxisWeight+=it->m_nAxisGroupWeightRaw;
        nSumOfAxisWeight+=it->m_nAxisGroupweight;
        if (0==dwMaxAxisWeight || it->m_nAxisGroupweight>dwMaxAxisWeight)
        {
            pMaxWeightAxisGroup=it;
            dwMaxAxisWeight=it->m_nAxisGroupweight;
        }
    }
    // 鎬婚噸閲忚姹傛槸100kg鐨勫€嶆暟锛岄€氳繃鍥涜垗浜斿叆鏂瑰紡杩涜璁＄畻
    m_dwTotalWeight=qRound(nSumOfRawAxisWeight*g_fWeightDevErroRatio/100)*100;
    // 淇敼鏈€閲嶈酱缁勭殑閲嶉噺锛屼互渚垮悇杞寸粍閲嶉噺涔嬪拰绛変簬鎬婚噸
    pMaxWeightAxisGroup->m_nAxisGroupweight=pMaxWeightAxisGroup->m_nAxisGroupweight+m_dwTotalWeight-nSumOfAxisWeight;
}
void CVehAxisInfo::CombineVeh(const CVehAxisInfo &VehAxisInfo)
{
    QList<CAxisGroupData> AxisGroupList;
    AxisGroupList=m_RawAxisList;
    QList<CAxisGroupData>::const_iterator it;
    for (it=VehAxisInfo.m_RawAxisList.begin();it!=VehAxisInfo.m_RawAxisList.end();it++)
    {
        AxisGroupList.push_back(*it);
    }
    // 瀵瑰師濮嬭酱缁勮繘琛屽悎骞讹紝閲嶆柊璁＄畻鎬婚噸锛堝彲娑堥櫎璁＄畻璇樊锛
    this->ModifyAxisGroupList(AxisGroupList);
    WeightStandardStatus=true;
}
void CVehAxisInfo::ModifyVehInfo(long nTotalWeight, long nWeightLimit)
{
    if (nTotalWeight!=0 && nWeightLimit==0)
    {
        this->WeightModifyStatus=false;
        this->m_dwTotalWeight=nTotalWeight;
    }
    if (nTotalWeight==0 && nWeightLimit!=0)
    {
        this->WeightStandardStatus=false;
        this->m_dwInputedWeightLimit=nWeightLimit;
    }
    if (nTotalWeight!=0 && nWeightLimit!=0)
    {
        this->WeightModifyStatus=false;
        this->WeightStandardStatus=false;
        this->m_dwTotalWeight=nTotalWeight;
        this->m_dwInputedWeightLimit=nWeightLimit;
    }
}
void CLaneAxisInfo::CopyVehList(const QList<CVehAxisInfo> &VehList)
{
    QMutexLocker lock(&mutex);
    this->m_VehList=VehList;
}

int CLaneAxisInfo::GetVehCount()
{
    QMutexLocker lock(&mutex);
    int n=(int)m_VehList.size();
    return n;
}
void CLaneAxisInfo::GetAllVeh(QList<CVehAxisInfo> *pVehAxisInfoList)
{
    QMutexLocker lock(&mutex);
    *pVehAxisInfoList=m_VehList;
}
bool CLaneAxisInfo::IsFirstNullVeh()
{
    CVehAxisInfo FirstVeh;
    if (!GetFirstVeh(&FirstVeh))
    {
        //濡傛灉涓嶅瓨鍦ㄩ杈嗚溅锛岃皟鐢ㄦ鍑芥暟杩斿洖鐪燂紝鍗抽杈嗚溅涓虹┖杞
        return TRUE;
    }
    return FirstVeh.IsNullVeh();
}
bool CLaneAxisInfo::CanSplit()
{
    //QDebug() << "mutex" << "CanSplit";
    bool bCanSplit;
    QMutexLocker lock(&mutex);
    bCanSplit=(0<m_VehList.size());
    if (bCanSplit)
    {
        bCanSplit=m_VehList.front().CanSplit();
    }
    return bCanSplit;
}
bool CLaneAxisInfo::CanCombine()
{
    //QDebug() << "mutex" << "CanCombine";
    QMutexLocker lock(&mutex);
    bool bCanCombine=(2<=m_VehList.size());
    if (bCanCombine)
    {
        bCanCombine=!m_VehList.front().IsNullVeh();
        if (bCanCombine)
        {
            QList<CVehAxisInfo>::const_iterator it=m_VehList.begin();
            const CVehAxisInfo &Veh1=*it;
            it++;
            const CVehAxisInfo &Veh2=*it;
            int nAxisGroupCount=Veh1.GetAxisGroupCount()+Veh2.GetAxisGroupCount();
            // 12涓烘姤鏂囧畾涔変腑鍙彂閫佽酱缁勬暟鐨勬渶澶у€
            bCanCombine=(nAxisGroupCount<=12);
        }
    }
    return bCanCombine;
}
bool CLaneAxisInfo::CanInsert()
{
    //QDebug() << "mutex" << "CanInsert";
    QMutexLocker lock(&mutex);
    if (0==GetVehCount())
    {
        return true;
    }
    // 棣栬締杞﹀簲鏈鍒囧壊杩囷紝骞朵笖涓嶆槸绌鸿溅
    if (m_VehList.front().IsNullVeh())
    {
        return false;
    }
    if (m_VehList.front().IsSplitted())
    {
        return false;
    }
    return true;
}
void CLaneAxisInfo::AddVeh(const CVehAxisInfo &VehInfo, bool bPushFront/*=FALSE*/)
{
    //QDebug() << "mutex" << "AddVeh";
    QMutexLocker lock(&mutex);
    if (bPushFront)
    {
        m_VehList.push_front(VehInfo);
    }
    else
    {
        m_VehList.push_back(VehInfo);
    }
}
void CLaneAxisInfo::AddVeh(long nWeight, long nWeightLimit)
{
    //QDebug() << "mutex" << "AddVeh";

    QMutexLocker lock(&mutex);
    // 娉ㄦ剰绌鸿溅瑕佸姞鍦ㄦ渶鍓嶉潰
    m_VehList.push_front(CVehAxisInfo::MakeVehInfo(nWeight, nWeightLimit));
}
void CLaneAxisInfo::ModifyVeh(const CVehAxisInfo &VehInfo)
{
    //QDebug() << "mutex" << "ModifyVeh";
    QMutexLocker lock(&mutex);
    m_VehList.pop_front();
    m_VehList.push_front(VehInfo);
}
bool CLaneAxisInfo::RemoveAll(QList<CVehAxisInfo> *pVehList)
{
    bool bNeedRemove;
    QMutexLocker lock(&mutex);
    bNeedRemove=(0<m_VehList.size());
    if (bNeedRemove)
    {
        if (NULL!=pVehList)
        {
            *pVehList=m_VehList;
        }
        m_VehList.clear();
    }
    return bNeedRemove;
}
bool CLaneAxisInfo::RemoveFirst(CVehAxisInfo *pFirstVeh)
{
    bool bCanRemove;
    //QDebug() << "mutex" << "RemoveFirst";
    QMutexLocker lock(&mutex);
    bCanRemove=(0<m_VehList.size());
    if (bCanRemove)
    {
        if (NULL!=pFirstVeh)
        {
            *pFirstVeh=m_VehList.front();
        }
        m_VehList.pop_front();
    }
    return bCanRemove;
}
bool CLaneAxisInfo::GetFirstVeh(CVehAxisInfo *pVehAxisInfo)
{
    bool bResult;

    QMutexLocker lock(&mutex);
    bResult=(m_VehList.size()>0);
    if (bResult)
    {
        *pVehAxisInfo=m_VehList.front();
    }
    return bResult;
}
bool CLaneAxisInfo::CombineFirst2Veh(CVehAxisInfo *pVeh1, CVehAxisInfo *pVeh2)
{
    //QDebug() << "mutex" << "CombineFirst2Veh";
    QMutexLocker lock(&mutex);
    bool bCanCombine=(2<=m_VehList.size());
    if (bCanCombine)
    {
        QList<CVehAxisInfo>::iterator it=m_VehList.begin();
        CVehAxisInfo &FirstVeh=*it;
        it++;
        CVehAxisInfo &SecondVeh=*it;
        if (NULL!=pVeh1)
        {
            *pVeh1=FirstVeh;
        }
        if (NULL!=pVeh2)
        {
            *pVeh2=SecondVeh;
        }
        FirstVeh.CombineVeh(SecondVeh);
        // 灏嗙2涓妭鐐瑰垹闄
        m_VehList.erase(it);
    }
    return bCanCombine;
}
bool CLaneAxisInfo::SplitVeh(int dwFirstAxisGroup,CVehAxisInfo *pSplittedVeh)
{
    bool bCanSplit;
    QMutexLocker lock(&mutex);
    bCanSplit=(0<m_VehList.size());
    if (bCanSplit)
    {
        CVehAxisInfo Veh1, Veh2;
        if (NULL!=pSplittedVeh)
        {
            *pSplittedVeh=m_VehList.front();
        }
        bCanSplit=m_VehList.front().Split(&Veh1, &Veh2, dwFirstAxisGroup);
        if (bCanSplit)
        {
            m_VehList.pop_front();
            m_VehList.push_front(Veh2);
            m_VehList.push_front(Veh1);
        }
    }
    return bCanSplit;
}
