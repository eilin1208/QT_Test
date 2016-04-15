#include "FareCalc.h"
#include "FareCalcFunc.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qbaserateparamfile.h"
#include "ParamModule/qallrateparamfile.h"
#include "ParamModule/qtollcellparamfile.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "ParamModule/qvccodeparamfile.h"
#include "ParamModule/qsprateparamfile.h"

// 最远站站代码
quint32 FareCalc::m_nFarthestSta=0;
// 最低收费标准（元）
quint16 FareCalc::m_nMinimumFareInYuan=0;

//YN wumax 增加费率由分变为厘的标志  1为厘标志，其他数值暂为不更改
static int m_FtoLFlag = 0;

// 查询基础费率信息
bool QueryBaseRate(SBaseRateValue &BaseRate, const QString &sRateCode, int nRateType, int nVehClass)
{
    QBaseRateParamFile *pFile;
    if (!getParamFile(&pFile, cfBaseRate))
    {
        return false;
    }
    return pFile->queryBaseRate(BaseRate, sRateCode, nRateType, nVehClass);
}

// 查询收费表
bool QueryTollValue(SAllRateValue &tollValue, quint32 nSta1, quint32 nSta2)
{
    QAllRateParamFile *pFile;
    if (!getParamFile(&pFile, cfAllRate))
    {
        return false;
    }
    return pFile->queryTollValue(tollValue, nSta1, nSta2);
}

// 查询相邻站包含的收费单元
bool QueryCellList(QList<quint16> &cellList, quint32 nSta1, quint32 nSta2)
{
    QTollCellParamFile *pFile;
    if (!getParamFile(&pFile, cfTollCell))
    {
        return false;
    }
    return pFile->queryCellListByNeighborSta(cellList, nSta1, nSta2);
}

// 查询车型编码
bool QueryVehClassInfo(SVCCodeValue &vehClassInfo, int nVehClass)
{
    QVCCodeParamFile *pFile;
    if (!getParamFile(&pFile, cfVCCode))
    {
        return false;
    }
    return pFile->queryVehClassInfo(vehClassInfo, nVehClass);
}

// 查询特殊收费
bool QuerySpecialTollInfo(SSpRateValue &SpecialRate, quint32 nSta1, quint32 nSta2, int nVehClass)
{
    QSpRateParamFile *pFile;
    if (!getParamFile(&pFile, cfSpRate))
    {
        return false;
    }
    return pFile->querySpecialTollInfo(SpecialRate, nSta1, nSta2, nVehClass);
}

// 查询收费单元信息
bool QueryTollCellValue(STollCellValue &TollCellValue, int nTollCell, quint32 nSta1, quint32 nSta2)
{
    QTollCellParamFile *pFile;
    if (!getParamFile(&pFile, cfTollCell))
    {
        return false;
    }
    // NOTE: 同一nTollCell对应的STollCellValue除所属相邻站外唯一，因为同一nTollCell可能属于多个相邻站，
    //       但所属相邻站代码对于判断区间免费比较重要，故如果采用只查询nTollCell的方式，查询后更改所属相邻站赋值
    bool bRlt=pFile->queryTollCellValue(TollCellValue, nTollCell);
    if (bRlt)
    {
        TollCellValue.dwStation1=nSta1;
        TollCellValue.dwStation2=nSta2;
    }
    return bRlt;
}

// 根据U型车计算行驶里程、出口站仿制一个收费单元
bool MakeTollCell(STollCellValue &tollCellValue, quint32 nExSta, quint32 dwTollMeters)
{
    QOrgCodeParamFile *pFile;
    if (!getParamFile(&pFile, cfOrgCode))
    {
        return false;
    }
    SOrgCodeValue orgCodeValue;
    if (!pFile->queryOrgCodeValue(orgCodeValue, nExSta))
    {
        return false;
    }
    tollCellValue.wCellCode=0;
    tollCellValue.cCellFlag=STollCellValue::ROUTE;
    tollCellValue.dwStation1=nExSta;
    tollCellValue.dwStation2=nExSta;
    tollCellValue.wStationSN=0;
    tollCellValue.wBL_Road=orgCodeValue.GetRoadBelong();
    tollCellValue.wBL_Country=orgCodeValue.GetAdmAreaBelong();
    tollCellValue.wOwner=orgCodeValue.GetOwnerBelong();
    tollCellValue.sWayFlag="";
    tollCellValue.sSpFreeFlag="";
    tollCellValue.sRateCode=UNIFIED_ROUTE_RATE_CODE;
    tollCellValue.dwRealDistance=0;
    tollCellValue.dwTollDistance=dwTollMeters;
    tollCellValue.wLimitSpeedPara=1000;
    tollCellValue.sLastVer="";
    tollCellValue.sUseTime=orgCodeValue.sUseTime;
    return true;
}

// 向SectMoneyList中添加SectMoney
void AddToSectMoneyList(QList<CSectMoney> &SectMoneyList, const CSectMoney &SectMoney)
{
    bool bFound=false;
    QList<CSectMoney>::iterator it;
    for (it=SectMoneyList.begin();it!=SectMoneyList.end();it++)
    {
        if (it->m_dwOrgCode==SectMoney.m_dwOrgCode)
        {
            it->Inc(SectMoney);
            bFound=true;
            break;
        }
    }
    if (!bFound)
    {
        SectMoneyList.push_back(SectMoney);
    }
}

// 检查入口站是否合法
bool FareCalc::VerifyEntryStaId(quint32 dwEntrySta, quint32 dwExitSta)
{
    if (dwEntrySta==dwExitSta)
    {
        // U型
        return true;
    }
    SAllRateValue tollValue;
    return QueryTollValue(tollValue, dwEntrySta, dwExitSta);
}

// 正常计费
bool FareCalc::queryFare(FareQueryResultClass *pResult, const FareQueryConditionClass &condition)
{
    // 初始化查询结果，为计费进行准备
    pResult->clear();
    pResult->m_vehTollInfo=condition.m_vehTollInfo;
    pResult->m_vehTollInfo.CalcOverweightInfo();
    pResult->m_vehTollInfo.JudgeIsBetweenFreeSta(condition.m_nEnSta, condition.m_nExSta);
    // 查询车型信息
    SVCCodeValue VehClassInfo;
    if (!QueryVehClassInfo(VehClassInfo, condition.m_vehTollInfo.m_bVehClass))
    {
        // LogFmtMsg("计算应收金额时查询车型信息失败，车型=%d", condition.m_vehTollInfo.m_bVehClass);
        return false;
    }
    // 查询不经标识站的收费信息
    SAllRateValue AllRate;
    if (!QueryTollValue(AllRate, condition.m_nEnSta, condition.m_nExSta))
    {
        // LogFmtMsg("计算应收金额时查询收费表失败，入口=%.6u，出口=%.6u", condition.m_nEnSta, condition.m_nExSta);
        return false;
    }
      quint16 wMinimumFareInYuan=5;  //wumax 最小金额5元


    // 确定车辆经过站点
    QList<quint32> PassStaList;
    if (0==condition.m_FlagStaList.size())
    {
        PassStaList=AllRate.tPassStaAry;
    }
    else
    {
        // TODO: 暂不考虑标识站的情况
        // return false;
        // LogFmtMsg("警告：忽略标识站信息，标识站数=%d！", condition.m_FlagStaList.size());
        PassStaList=AllRate.tPassStaAry;
    }
    quint32 sta1, sta2;
    QList<quint32>::const_iterator it_sta=PassStaList.begin();
    if (PassStaList.end()==it_sta)
    {
        return false;
    }
    sta1=*it_sta;
    it_sta++;
    while (it_sta!=PassStaList.end())
    {
        // 取sta2
        sta2=*it_sta;
        // 计算并累加sta1到sta2之间的通行费、经过收费单元信息
        bool bRlt;
        bRlt=AccumulateSectMoney(pResult->m_SplitInfo,
            pResult->m_PassedTollCellList,pResult->m_FreeInfoList,
            pResult->m_vehTollInfo, sta1, sta2);
        if (!bRlt)
        {
            return false;
        }
        // 保存sta2为sta1，进行下一循环
        sta1=sta2;it_sta++;
    }

    pResult->m_PassStaList = PassStaList;

    // 对分段计算结果进行汇总、取整
    pResult->SumSectMoney(wMinimumFareInYuan*100);
    // 记录里程信息、计算车辆行驶时间
    pResult->m_dwRealMeters=AllRate.dwRealDistance;
    pResult->m_dwTollMeters=AllRate.dwTollDistance;
    pResult->m_dwMinSeconds=calcDriveTime(AllRate.dwRealDistance, VehClassInfo.wMaxSpeed);
    pResult->m_dwMaxSeconds=calcDriveTime(AllRate.dwRealDistance, VehClassInfo.wOverSpeed);
    // 超时时间至少是1小时，即3600秒
    if (pResult->m_dwMaxSeconds<3600) pResult->m_dwMaxSeconds=3600;

    // 对车型计费的情况，应收金额和免收金额之和应与车型费率一致，不必特别计算
    // （暂不考虑标识站的情况）
    quint32 dwMoneyTotal=quint32(-1);
    if (condition.m_vehTollInfo.ChargeByVehClass())
    {
        quint16 wPriceInYuan;
        if (!AllRate.getPrice(wPriceInYuan, condition.m_vehTollInfo.m_bVehClass))
        {
            return false;
        }
        dwMoneyTotal=wPriceInYuan*100;
    }

    if (quint32(-1)!=dwMoneyTotal)	// 预设初值已改变，为车型计费，或者有特殊收费标准计费
    {
        if (0==pResult->m_dwMoneyFree)			// 全部收费的情况
        {
            pResult->m_dwMoneyDue=dwMoneyTotal;
        }
        else if (0==pResult->m_dwMoneyDue)		// 全部免费的情况
        {
            pResult->m_dwMoneyFree=dwMoneyTotal;
        }
        else									// 部分收费、部分免费的情况
        {
            // 如果计算应收大于调整后应收（实际应收），以调整后应收为准，这种情况下免收金额就会被调整为0了！
            //	如计算应收为11，免收为3，调整后收费10元，则最后免收为0，应收为10！
            //	如计算应收为7，免收为2或4，调整后收费10元，则最后免收为3，应收为7！
            //	这种调整采用首先满足计算应收的原则，有点简单，对免收部分考虑不足，可能按比例调整更为合理，因这种情况较少，暂保持这种方式。
            if (pResult->m_dwMoneyDue>dwMoneyTotal)
            {
                pResult->m_dwMoneyDue=dwMoneyTotal;
            }
            pResult->m_dwMoneyFree=dwMoneyTotal-pResult->m_dwMoneyDue;
        }
    }
    // 进行折扣和拆分处理
    pResult->Discount(condition.m_nDiscountRate);

    return true;
}

// U型计费
bool FareCalc::QueryUTypeFare(FareQueryResultClass *pResult, const FareQueryConditionClass &Condition)
{
    bool bFree;
    const quint64 METERS_MAX=20000*1000;        //	最大里程限制在2万公里，以防止计算过程中溢出
    const quint64 METERS_SUSPECT_MAX=1000*1000; //	1000公里以上怀疑超出最大收费金额，按换卡车计费进行排查
    // 初始化查询结果，为计费进行准备
    pResult->clear();
    pResult->m_vehTollInfo=Condition.m_vehTollInfo;
    pResult->m_vehTollInfo.CalcOverweightInfo();
    // 计算车辆行驶时间（U型车不统计超速、超时时间设置为最大允许超时时间）
    pResult->m_dwMinSeconds=0;
    pResult->m_dwMaxSeconds=calcDriveTime(0, 0);
    //wumax 增加最远站查询
    updateFarthestStaAndMinimumFare();
    if (pResult->m_vehTollInfo.m_bIsFullFreeVeh)
    {
        // 对全免车，通行费为0，也不必统计免收金额
        return true;
    }
    // 计算收费里程
    if (0==Condition.m_dwTollMeters)
    {
        if (Condition.m_nDriveTimeInSeconds<=0)
        {
            pResult->m_dwTollMeters=0;
        }
        else
        {
            // 查询车型信息
            SVCCodeValue vehClassInfo;
            if (!QueryVehClassInfo(vehClassInfo, Condition.m_vehTollInfo.m_bVehClass))
            {
                return false;
            }
            quint64 llMeters=round_int64((double)vehClassInfo.wVUSpeed*Condition.m_nDriveTimeInSeconds/3600*1000);
            if (llMeters>METERS_MAX)
            {
                pResult->m_dwTollMeters=METERS_MAX;
            }
            else
            {
                pResult->m_dwTollMeters=(quint32)llMeters;
            }
        }
    }
    else
    {
        pResult->m_dwTollMeters=Condition.m_dwTollMeters;
    }
    // 根据U型车行驶里程、出口站仿制一个收费单元
    STollCellValue TollCell;
    if (!MakeTollCell(TollCell, Condition.m_nExSta, pResult->m_dwTollMeters))
    {
        return false;
    }
    CSectMoney SectMoney(0);
    if (!pResult->m_vehTollInfo.CalcMoney(SectMoney, TollCell,&bFree))
    {
        return false;
    }
    AddToSectMoneyList(pResult->m_SplitInfo, SectMoney);
    // 对分段计算结果进行汇总、取整，传入U型车最小收费金额
    pResult->SumSectMoney(0);
    // 如果有可能超出U型车最大收费金额，对收费金额进行调整
    if (pResult->m_dwTollMeters>METERS_SUSPECT_MAX)
    {
        FareQueryResultClass ChangeCardFareResult;
        VehTollInfoClass ChangeCardTollVehInfo=Condition.m_vehTollInfo;
        ChangeCardTollVehInfo.m_bIsFullFreeVeh=false;
        ChangeCardTollVehInfo.m_FreeRegion.clear();
        if (!QueryChangeCareFare(&ChangeCardFareResult, Condition.m_vehTollInfo, Condition.m_nExSta,Condition.m_nEnSta))
        {
            return false;
        }
        pResult->m_dwMoneyOverweightDue=ChangeCardFareResult.m_dwMoneyOverweightDue;
        if (pResult->m_dwMoneyDue>ChangeCardFareResult.m_dwMoneyDue)
        {
            pResult->m_dwMoneyDue=ChangeCardFareResult.m_dwMoneyDue;
        }
        if (pResult->m_dwMoneyFree>ChangeCardFareResult.m_dwMoneyDue)
        {
            pResult->m_dwMoneyFree=ChangeCardFareResult.m_dwMoneyDue;
        }
    }
    // 因U型车为特殊拆分，清空计算过程中产生的拆分信息
    pResult->m_SplitInfo.clear();
    // 进行折扣和拆分处理
    pResult->Discount(Condition.m_nDiscountRate);
    return true;
}

// U型计费
bool FareCalc::QueryUTypeFare(FareQueryResultClass *pResult,
    const VehTollInfoClass &TollVehInfo,
    const int nDriveTimeInSeconds,
    const quint32 dwExSta,
    int m_nDiscountRate)
{
    FareQueryConditionClass condition;
    condition.m_vehTollInfo=TollVehInfo;
    condition.m_nEnSta=dwExSta;
    condition.m_nExSta=dwExSta;
    condition.m_dwTollMeters=0;
    condition.m_nDriveTimeInSeconds=nDriveTimeInSeconds;
    condition.m_FlagStaList.clear();
    condition.m_nDiscountRate=m_nDiscountRate;
    return QueryUTypeFare(pResult, condition);
}

// 无卡车最大里程计费
bool FareCalc::QueryNoCardFare(FareQueryResultClass *pResult,
    const VehTollInfoClass &TollVehInfo,
    const quint32 dwExSta,quint32 dwEnSta,
    int m_nDiscountRate)
{
    //wumax 增加最远站查询
    updateFarthestStaAndMinimumFare();
    dwEnSta=FareCalc::getFarthestSta();
    if (0==dwEnSta)
    {
            return false;
    }
    FareQueryConditionClass condition;
    condition.m_vehTollInfo=TollVehInfo;
    condition.m_nEnSta=dwEnSta;
    condition.m_nExSta=dwExSta;
    if (!queryFare(pResult, condition))
    {
        return false;
    }
    pResult->m_SplitInfo.clear();
    return true;
}

// 换卡车两倍最大里程计费
bool FareCalc::QueryChangeCareFare(FareQueryResultClass *pResult,
    const VehTollInfoClass &TollVehInfo,
    const quint32 dwExSta,quint32 dwEnSta,
    int m_nDiscountRate)
{
    //wumax 增加最远站查询
    updateFarthestStaAndMinimumFare();
    dwEnSta=FareCalc::getFarthestSta();
    if (0==dwEnSta)
    {
        return false;
    }
    FareQueryConditionClass condition;
    condition.m_vehTollInfo=TollVehInfo;
    condition.m_nEnSta=dwEnSta;
    condition.m_nExSta=dwExSta;
    if (!queryFare(pResult, condition))
    {
        return false;
    }
    pResult->m_SplitInfo.clear();
    pResult->Multiply(2);
    pResult->Discount(m_nDiscountRate);
    return true;
}

// 查询两站之间经过的单元信息，计算金额并累加
bool FareCalc::AccumulateSectMoney(QList<CSectMoney> &SectMoneyList,
    QList<STollCellValue> &PassedTollCellList,
    QList<TFreeInfo> &FreeInfoList,
    const VehTollInfoClass &vehTollInfo,
    quint32 dwSta1, quint32 dwSta2)
{
    bool bFree;
    QList<quint16> wCellList;
    if (!QueryCellList(wCellList, dwSta1, dwSta2))
    {
        return false;
    }
    QList<quint16>::const_iterator it;
    CSectMoney SectMoney(0);
    STollCellValue TollCell;
    for (it=wCellList.begin();it!=wCellList.end();it++)
    {
        if (!vehTollInfo.CalcMoney(SectMoney, TollCell, dwSta1, dwSta2, *it,&bFree))
        {
            return false;
        }
        AddToSectMoneyList(SectMoneyList, SectMoney);
        PassedTollCellList.push_back(TollCell);
        if (bFree)
        {
          quint16 wBL_Road = TollCell.wBL_Road;
          AddFreeInfo(FreeInfoList,wBL_Road, dwSta1, dwSta2);
        }
    }
    return true;
}

// 更新最远站、最低收费金额
bool FareCalc::updateFarthestStaAndMinimumFare()
{
    QAllRateParamFile *pTollFile;
    QSysParaDicParamFile *pSysFile;
    QOrgCodeParamFile *pOrgFile;

    if ((!getParamFile(&pTollFile, cfAllRate)) ||
            (!getParamFile(&pSysFile, cfSysParaDic)) ||
            (!getParamFile(&pOrgFile, cfOrgCode)))
    {
        return false;
    }

    if ((0==pTollFile->getRowCount()) ||
            (0==pSysFile->getRowCount()) ||
            (0==pOrgFile->getRowCount()))
    {
        return false;
    }

    quint32 nSta2=pSysFile->GetSysParamInfo().m_nStationID;
    if (0==nSta2) return false;

    // 查询候选站
    QList<quint32> enStaList;
    if (!pOrgFile->getStaList(enStaList))
    {
        return false;
    }

    // 查询最远站代码、最低收费金额
    quint32 nFarthestSta;
    quint16 nMinimumFareInYuan;
    if (!pTollFile->queryFarthestStaAndMinimumFare(&nFarthestSta, &nMinimumFareInYuan, enStaList, nSta2))
    {
        return false;
    }
    m_nFarthestSta=nFarthestSta;
    m_nMinimumFareInYuan=nMinimumFareInYuan;
    return true;
}


void FareCalc::AddFreeInfo(QList<TFreeInfo> &FreeInfoList,quint16 dwRoad,quint32 dwSta1, quint32 dwSta2)
{
    TFreeInfo pFree;
    bool bFound = false;
    for(int i=0;i<FreeInfoList.count()-1;i++)
    {
        pFree=FreeInfoList[i];
        //判断是否是同一个路段
        if(dwRoad == pFree.wRoadID)
        {
            //判断是否是相邻，可以接续
            //if ((pFree.wBeginSta==dwSta1||pFree.wBeginSta==dwSta2)||(pFree.wEndSta==dwSta1||pFree.wEndSta==dwSta2))
            if(dwRoad==pFree.wRoadID)
            {
                //起点相邻
                pFree.wBeginSta = dwSta2;
                bFound = true;
            }
            else if(pFree.wBeginSta == dwSta2)
            {
                //起点相邻
                pFree.wBeginSta = dwSta1;
                bFound = true;
            }
            else if(pFree.wEndSta == dwSta1)
            {
                //终点相邻
                pFree.wEndSta = dwSta2;

                bFound = true;
            }
            else if(pFree.wEndSta == dwSta2)
            {
                //终点相邻
                pFree.wEndSta = dwSta1;
                bFound = true;
            }
            if(bFound)
            {
                break;
            }

        }
    }
    if(!bFound)
    {
      //New(pFree);
      pFree.wRoadID = dwRoad;
      pFree.wBeginSta= dwSta1;
      pFree.wEndSta = dwSta2;
      FreeInfoList.push_back(pFree);
    }
}

void SetFtoLFlag(int flag)
{
    m_FtoLFlag = flag;
}

int GetFtoLFlag()
{
    return m_FtoLFlag;

}

