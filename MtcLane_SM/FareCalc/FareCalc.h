#ifndef FARE_CALC_H
#define FARE_CALC_H

#include <QtGlobal>
#include <QList>
#include "farequeryresultclass.h"

// 查询基础费率信息
bool QueryBaseRate(SBaseRateValue &BaseRate, const QString &sRateCode, int nRateType, int nVehClass);
// 查询收费表
bool QueryTollValue(SAllRateValue &tollValue, quint32 nSta1, quint32 nSta2);
// 查询相邻站包含的收费单元
bool QueryCellList(QList<quint16> &cellList, quint32 nSta1, quint32 nSta2);
// 查询收费单元信息
bool QueryTollCellValue(STollCellValue &TollCellValue, int nTollCell, quint32 nSta1, quint32 nSta2);
//YN wumax 增加设置费率由分变为厘的标志
void SetFtoLFlag(int flag);
//YN wumax 增加获取费率由分变为厘的标志
int GetFtoLFlag();

class FareCalc
{
public:
    // 检查入口站是否合法
    static bool VerifyEntryStaId(quint32 dwEntrySta, quint32 dwExitSta);
    // 正常计费
    static bool queryFare(FareQueryResultClass *pResult, const FareQueryConditionClass &condition);
    // 固定费额计费 - 仅适用于同一收费站，主、辅广场有道路可通，不按U型车计费的情况。原先用于滕州南收费站，后该站改建后山东已无类似收费站。
   // static bool QueryFare(FareQueryResultClass *pResult, const FareQueryConditionClass &condition, const int nFixedFare);
    // U型计费
    static bool QueryUTypeFare(FareQueryResultClass *pResult, const FareQueryConditionClass &condition);
    // U型计费
    static bool QueryUTypeFare(FareQueryResultClass *pResult,
                               const VehTollInfoClass &TollVehInfo,
                               const int nDriveTimeInSeconds,
                               const quint32 dwExSta,
                               int m_nDiscountRate=0);
    // 无卡车最大里程计费
    static bool QueryNoCardFare(FareQueryResultClass *pResult,
                                const VehTollInfoClass &TollVehInfo,
                                const quint32 dwExSta,quint32 dwEnSta,
                                int m_nDiscountRate=0);
    // 换卡车两倍最大里程计费
    static bool QueryChangeCareFare(FareQueryResultClass *pResult,
                                    const VehTollInfoClass &TollVehInfo,
                                    const quint32 dwExSta,quint32 dwEnSta,
                                    int m_nDiscountRate=0);
protected:
    // 查询两站之间经过的单元信息，计算金额并累加
    static bool AccumulateSectMoney(QList<CSectMoney> &SectMoneyList,
                                    QList<STollCellValue> &PassedTollCellList,
                                    QList<TFreeInfo> &FreeInfoList,
                                    const VehTollInfoClass &TollVehInfo,
                                    quint32 dwSta1, quint32 dwSta2);
    /////////////////YN(wumax)
    //增加一个免费段 wumax
    static void AddFreeInfo(QList<TFreeInfo> &FreeInfoList,quint16 dwRoad,quint32 dwSta1, quint32 dwSta2);
      ///////////////////
public:
    // 更新最远站、最低收费金额
    static bool updateFarthestStaAndMinimumFare();

public:
    static quint32 getFarthestSta()
    {
        return m_nFarthestSta;
    }
protected:
    // 最远站站代码
    static quint32 m_nFarthestSta;
    // 最低收费标准（元）
    static quint16 m_nMinimumFareInYuan;


};

#endif // FARE_CALC_H
