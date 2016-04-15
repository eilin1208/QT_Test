#include "vehtollinfoclass.h"
#include "FareCalcFunc.h"
#include "FareCalc.h"

// 计算车辆限载、超重等信息
void VehTollInfoClass::CalcOverweightInfo()
{
    m_dwWeightLimit=CalcWeightLimitAndOverweightRate(m_nOverweightRate, m_nAxisNum, m_dwTotalWeight, m_dwWeightLimit);
    if (m_dwTotalWeight>m_dwWeightLimit)
    {
        m_dwWeightUnderLimit=m_dwWeightLimit;
        m_dwWeightOverLimit=m_dwTotalWeight-m_dwWeightLimit;
    }
    else
    {
        m_dwWeightUnderLimit=m_dwTotalWeight;
        m_nOverweightRate=0;
    }
}

// 判断是否符合站间免费的条件
void VehTollInfoClass::JudgeIsBetweenFreeSta(quint32 dwSta1, quint32 dwSta2)
{
    m_bIsBetweenFreeSta=false;
    if (dwSta1==dwSta2)
    {
        // 疑似U型车，不符合条件
        return;
    }
    if (FreeRegionClass::frNeighborStaPlus!=m_FreeRegion.m_nFreeRegionType)
    {
        // 类型不符，不符合条件
        return;
    }
    bool bHasError;
    m_bIsBetweenFreeSta=m_FreeRegion.IsFreeRegion(&bHasError, 0, 0, dwSta1, dwSta2);
}

// 根据最小单元代号查询收费单元信息，并计算应收和免收金额（单位为千分之一）
bool VehTollInfoClass::CalcMoney(CSectMoney &SectMoney, STollCellValue &TollCellValue,
    quint32 nSta1, quint32 nSta2, int nTollCell,bool *bFree) const
{
    // 查询收费单元
    if (!QueryTollCellValue(TollCellValue, nTollCell, nSta1, nSta2))
    {
        return false;
    }
    return CalcMoney(SectMoney, TollCellValue,bFree);
}

// 根据查询出的最小单元计算应收和免收金额（单位为分）
bool VehTollInfoClass::CalcMoney(CSectMoney &SectMoney, const STollCellValue &TollCell,bool *bFree) const
{
    // 判断是按车型还是按吨公里收费
    bool bChargeByVehClass=ChargeByVehClass();
    double fTotalMoney=0;
    double fOverweightMoney=0;
    if (bChargeByVehClass)
    {
        if (!CalcMoneyByVehClass(fTotalMoney, TollCell))
        {
            return false;
        }
    }
    else
    {
        if (!CalcMoneyByWeight(fTotalMoney, fOverweightMoney, TollCell))
        {
            return false;
        }
    }
    *bFree = false; //wumax
    // 判别通行费为免收还是应收
    bool bHasError=false;
    bool bIsFreeRegion;
    SectMoney.Clear(TollCell.wOwner);
    SectMoney.m_dwOrgCode=TollCell.wBL_Road;
    if (m_bIsFullFreeVeh || m_bIsBetweenFreeSta)
    {
        bIsFreeRegion=true;
        SectMoney.m_fCentFree=fTotalMoney;
        *bFree=true;
    }
    else
    {
        bIsFreeRegion=m_FreeRegion.IsFreeRegion(&bHasError,
            TollCell.wOwner, TollCell.wBL_Country, TollCell.dwStation1, TollCell.dwStation2);
    }
    if (bIsFreeRegion)
    {
        SectMoney.m_fCentFree=fTotalMoney;
        SectMoney.m_fCentOverweightFree=fOverweightMoney;
        *bFree=true;
    }
    else
    {
        SectMoney.m_fCentDue=fTotalMoney;
        SectMoney.m_fCentOverweightDue=fOverweightMoney;
    }
    return (!bHasError);
}

// 根据车型计费
bool VehTollInfoClass::CalcMoneyByVehClass(double &fTotalMoney, const STollCellValue &TollCell) const
{
    int nRateType;
    quint32 dwTollDistance;
    // 查询基础费率、里程
    if (STollCellValue::ROUTE==TollCell.cCellFlag)
    {
        nRateType=SBaseRateValue::ROUTE_VEH_CLASS;
        dwTollDistance=TollCell.dwTollDistance;
    }
    else if (STollCellValue::POINT==TollCell.cCellFlag)
    {
        nRateType=SBaseRateValue::POINT_VEH_CLASS;
        dwTollDistance=1000;
    }
    else
    {
        return false;
    }
    SBaseRateValue BaseRate;
    if (!QueryBaseRate(BaseRate, TollCell.sRateCode, nRateType, m_bVehClass))
    {
        return false;
    }
    // 计算通行费：用费率乘以路段里程
    fTotalMoney=(double)BaseRate.wVCRate*dwTollDistance/1000.0;
    return true;
}

// 根据重量计费
bool VehTollInfoClass::CalcMoneyByWeight(double &fTotalMoney, double &fOverweightMoney, const STollCellValue &TollCell) const
{
    int nNormalRateType, nOverweightRateType;
    quint32 dwTollDistance;
    quint32 dwMinWeight;
    // 查询基础费率、里程
    if (STollCellValue::ROUTE==TollCell.cCellFlag)
    {
        nNormalRateType=SBaseRateValue::ROUTE_WEIGHT_UNDER_LIMIT;
        //nOverweightRateType=SBaseRateValue::ROUTE_WEIGHT_EXCEED_LIMIT;
        // 高速公路不足5吨（按5吨计）
        dwMinWeight=5000;
        dwTollDistance=TollCell.dwTollDistance;
    }
    else if (STollCellValue::POINT==TollCell.cCellFlag)
    {
        nNormalRateType=SBaseRateValue::POINT_WEIGHT_UNDER_LIMIT;
        //nOverweightRateType=SBaseRateValue::POINT_WEIGHT_EXCEED_LIMIT;
        // 特大桥不足10吨（按10吨计）
        dwMinWeight=10000;
        dwTollDistance=1000;
    }
    else
    {
        return false;
    }
    SBaseRateValue BaseRate_Normal, BaseRate_Overweight;
    // 查询正常费率
    if (!QueryBaseRate(BaseRate_Normal, TollCell.sRateCode, nNormalRateType, m_bVehClass))
    {
        return false;
    }

    // 计算单点时的通行费
    double fNormalMoney=0;
    fOverweightMoney=0;
    double MoneyPerTon = 0;//每吨单价
    quint32 dwWeightUnderLimit=qMax(m_dwWeightUnderLimit, dwMinWeight);
    //wumax  //计算不超限部分金额
    fNormalMoney=CalNormalFare(dwWeightUnderLimit, dwTollDistance, BaseRate_Normal);
    //wumax ==
    //计算总金额
    MoneyPerTon = BaseRate_Normal.wWBaseRate * dwTollDistance / 1000.0;


    if (m_nOverweightRate <= 0)
    {
      //不超重
      fTotalMoney = fNormalMoney;
    }
    else if(m_nOverweightRate <= 300)
    {
      //超重30%之内
      fTotalMoney = m_dwTotalWeight * MoneyPerTon;
    }
    else if(m_nOverweightRate<=1000)
    {
      //超重30%到100%
      //云南算法
      //由业主提供代码中移植
      //point := (Gi - 1.3*W) *(1.5 + (1/14)*((20*Gi)/W - 5))*0.08 * L;
      //Toll := 1.3 * W * 0.08 * L + point;
      fTotalMoney = 1.3*dwWeightUnderLimit* MoneyPerTon
          +(m_dwTotalWeight-1.3*dwWeightUnderLimit)
          *(1.5+(1/14.0)*(20*(m_nOverweightRate+1000) / 1000.0 - BaseRate_Normal.wWAMaxParam/1000.0))*MoneyPerTon;

    }
    else
    {
      //超重100%以上
      //云南算法
      //由业主提供代码中移植
      //point := 0.7*W*L*4*0.08 + (Gi - 2*W)*5*0.08*L;
      //Toll := 1.3*W*0.08*L + point;
      fTotalMoney = 1.3*dwWeightUnderLimit* MoneyPerTon
          + 0.7 * dwWeightUnderLimit*4* MoneyPerTon
          + (m_dwTotalWeight-2*dwWeightUnderLimit)*BaseRate_Normal.wWAMaxParam/1000.0*MoneyPerTon;
    }
    //wumax 公用变量
    fOverweightMoney = ((double)(fTotalMoney-fNormalMoney))/1000.0;
    fTotalMoney=(double)fTotalMoney/ 1000.0 ;
    if(1 == GetFtoLFlag())
    {
        fOverweightMoney = fOverweightMoney/10.0;
        fTotalMoney = fTotalMoney/ 10.0 ;
    }
    return true;
}
double CalNormalFare(quint32 weight,quint32 Meters, const SBaseRateValue &baseRate)
{
    double dWeiget = weight;
    const double  dwWeightRange1 = 20000;
    const double  dwWeightRange2 = 40000;
    double K1 = 0;
    double MoneyPerTon = 0;
    double drlt = 0;
    MoneyPerTon =  (double)baseRate.wWBaseRate * (double)Meters / 1000.0;
    if(weight <=dwWeightRange1)
    {
        drlt = weight*MoneyPerTon;
    }
    else if(weight<=dwWeightRange2)
    {
        K1 = 1.5-dWeiget/dwWeightRange2;
        drlt = dwWeightRange1*MoneyPerTon+K1*(weight-dwWeightRange1)*MoneyPerTon;
    }
    else
    {
        drlt= dwWeightRange1*MoneyPerTon+baseRate.wWMMinParam/1000*(weight-dwWeightRange1)*MoneyPerTon;
    }
    return drlt;
}
