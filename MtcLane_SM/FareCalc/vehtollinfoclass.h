#ifndef VEHTOLLINFOCLASS_H
#define VEHTOLLINFOCLASS_H

#include <QtGlobal>
#include "ParamModule/qtollcellparamfile.h"
#include "freeregionclass.h"
#include "ParamModule/qbaserateparamfile.h"
// 拆分信息 - 小业主的应收/免收（有可能为0）
class CSectMoney
{
public:
    quint32 m_dwOrgCode;			//	机构代码   ///////YN(wumax)
    double m_fCentOverweightDue;	//	超限加收金额（分）   ///////YN(wumax)
    double m_fCentOverweightFree;	//	免收部分的超限加收金额（分）
    double m_fCentDue;			//	应收金额（分）      ///////YN(wumax)
    double m_fCentFree;			//	免收金额（分）      ///////YN(wumax)
    quint32 m_dwMoneyDue;			//	应收金额（分） ///////YN(wumax)
    quint32 m_dwMoneyFree;		//	免收金额（分）         ///////YN(wumax)
//
public:
    CSectMoney(quint32 dwOrgCode)
    {
            Clear(dwOrgCode);
    }
    void Clear(quint32 dwOrgCode)
    {
            m_dwOrgCode=dwOrgCode;
            Clear();
    }
    void Clear()
    {
            m_fCentOverweightDue=0;
            m_fCentOverweightFree=0;
            m_fCentDue=0;
            m_fCentFree=0;
            m_dwMoneyDue=0;
            m_dwMoneyFree=0;
    }
    void operator =(const CSectMoney &b)
    {
            m_dwOrgCode=b.m_dwOrgCode;
            m_fCentOverweightDue=b.m_fCentOverweightDue;
            m_fCentOverweightFree=b.m_fCentOverweightFree;
            m_fCentDue=b.m_fCentDue;
            m_fCentFree=b.m_fCentFree;
            m_dwMoneyDue=b.m_dwMoneyDue;
            m_dwMoneyFree=b.m_dwMoneyFree;
    }
    void Inc(const CSectMoney &b)
    {
            m_fCentOverweightDue+=b.m_fCentOverweightDue;
            m_fCentOverweightFree+=b.m_fCentOverweightFree;
            m_fCentDue+=b.m_fCentDue;
            m_fCentFree+=b.m_fCentFree;
    }
};

// 车辆收费信息
class VehTollInfoClass  ///////YN(wumax)
{
public:
    bool m_bIsFullFreeVeh;          //	是否是全免车辆
    bool m_bChargeByVehClass;       //	货车按车型计费
    quint8 m_bVehClass;             //	输入车型
    int m_nAxisNum;                 //	车辆轴数
    quint32 m_dwTotalWeight;        //	车辆总重（千克）
    FreeRegionClass m_FreeRegion;   //	免费区间信息
public:
    quint32 m_dwWeightLimit;        //	车辆限载 - 正常情况下是0，修改限载后为手工输入值
    quint32 m_dwWeightUnderLimit;   //	低于车辆限载的车辆总重
    quint32 m_dwWeightOverLimit;    //	超出车辆限载的车辆总重
    int m_nOverweightRate;          //	车辆超限率
protected:
    bool m_bIsBetweenFreeSta;       //	入、出站是否都在免费站集合内
public:
    VehTollInfoClass()
    {
        clear();
    }
    void clear()
    {
        m_bIsFullFreeVeh=false;
        m_bChargeByVehClass=false;
        m_bVehClass=0;
        m_nAxisNum=0;
        m_dwTotalWeight=0;
        m_FreeRegion.clear();

        m_dwWeightLimit=0;
        m_dwWeightUnderLimit=0;
        m_dwWeightOverLimit=0;
        m_nOverweightRate=0;
        m_bIsBetweenFreeSta=false;
    }
    void operator =(const VehTollInfoClass &b)
    {
        m_bIsFullFreeVeh=b.m_bIsFullFreeVeh;
        m_bChargeByVehClass=b.m_bChargeByVehClass;
        m_bVehClass=b.m_bVehClass;
        m_nAxisNum=b.m_nAxisNum;
        m_dwTotalWeight=b.m_dwTotalWeight;
        m_FreeRegion=b.m_FreeRegion;

        m_dwWeightLimit=b.m_dwWeightLimit;
        m_dwWeightUnderLimit=b.m_dwWeightUnderLimit;
        m_dwWeightOverLimit=b.m_dwWeightOverLimit;
        m_nOverweightRate=b.m_nOverweightRate;
        m_bIsBetweenFreeSta=b.m_bIsBetweenFreeSta;
    }
    // 判断该车应按车型收费，还是应按吨公里收费
    bool ChargeByVehClass() const
    {
        return (m_bChargeByVehClass || IsCar(m_bVehClass));
    }
    // 计算车辆限载、超重等信息
    void CalcOverweightInfo();
    // 判断是否符合站间免费的条件
    void JudgeIsBetweenFreeSta(quint32 dwSta1, quint32 dwSta2);
    // 根据最小单元代号查询收费单元信息，并计算应收和免收金额（金额保留至小数点后10位）
    bool CalcMoney(CSectMoney &SectMoney, STollCellValue &TollCellValue,
        quint32 nSta1, quint32 nSta2, int nTollCell,bool *bFree) const;
    // 根据查询出的最小单元计算应收和免收金额（金额保留至小数点后10位）
    bool CalcMoney(CSectMoney &SectMoney, const STollCellValue &TollCell,bool *bFree) const;
    // 根据车型计费（分）
    bool CalcMoneyByVehClass(double &fTotalMoney, const STollCellValue &TollCell) const;
    // 根据重量计费（分）
    bool CalcMoneyByWeight(double &fTotalMoney, double &fOverweightMoney, const STollCellValue &TollCell) const;

protected:


};
//计算不超重费率	Gi：重量（kg） Meters：里程（m），返回千分
double CalNormalFare(quint32 weight,quint32 Meters, const SBaseRateValue &baseRate);

#endif // VEHTOLLINFOCLASS_H
