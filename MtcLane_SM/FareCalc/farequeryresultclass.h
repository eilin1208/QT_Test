#ifndef FAREQUERYRESULTCLASS_H
#define FAREQUERYRESULTCLASS_H

#include "vehtollinfoclass.h"

struct TFreeInfo
{
    quint32 wRoadID;    //路段编号
    quint32 wBeginSta;         //起点站
    quint32 wEndSta;         //终点站
};


// 通行费查询输入
class FareQueryConditionClass
{
public:
    VehTollInfoClass m_vehTollInfo;     //	收费车辆信息
    quint32 m_nEnSta;                   //	入口站
    quint32 m_nExSta;                   //	出口站
    quint32 m_dwTollMeters;             //	计费里程，U型车计费用（非0时优先于nDriveTimeInSeconds使用）
    int m_nDriveTimeInSeconds;          //	行驶时间，U型车计费用（负数按0处理）
    QList<quint32> m_FlagStaList;       //	经过的标识站列表
    int m_nDiscountRate;                //	折扣率，0表示无折扣，1000表示全免（如果赋好了初值，则直接计算折扣后的结果，适用于ETC车道）
public:
    FareQueryConditionClass()
    {
        clear();
    }
    void clear()
    {
        m_vehTollInfo.clear();
        m_nEnSta=0;
        m_nExSta=0;
        m_dwTollMeters=0;
        m_nDriveTimeInSeconds=0;
        m_FlagStaList.clear();
        m_nDiscountRate=0;
    }
    void operator =(const FareQueryConditionClass &b)
    {
        m_vehTollInfo=b.m_vehTollInfo;
        m_nEnSta=b.m_nEnSta;
        m_nExSta=b.m_nExSta;
        m_dwTollMeters=b.m_dwTollMeters;
        m_nDriveTimeInSeconds=b.m_nDriveTimeInSeconds;
        m_FlagStaList=b.m_FlagStaList;
        m_nDiscountRate=b.m_nDiscountRate;
    }
};

// 通行费查询输出
class FareQueryResultClass
{
public:
    quint32 m_dwRealMeters;         //	实际里程（米）
    quint32 m_dwTollMeters;         //	计费里程（米）
    double m_fCentDue;              //	总应收金额（分），实际汇总得出，用于拆分比例的基数
    double m_fCentFree;             //	总免收金额（分），实际汇总得出，用于拆分比例的基数
    double m_fCentOverweightDue;    //	超限金额（分，只考虑应收部分）
    quint32 m_dwMoneyDue;           //	总应收金额（分），根据特殊或车型收费标准调整，为折扣前最终结果
    quint32 m_dwMoneyFree;          //	总免收金额（分），根据特殊或车型收费标准调整，为折扣前最终结果
    quint32 m_dwMoneyOverweightDue; //	应收部分超限金额（分），根据特殊或车型收费标准调整，为折扣前结果
    quint32 m_dwDiscountedDue;      //	折扣后总应收金额（分）
    quint32 m_dwDiscountedFree;     //	折扣后总免收金额（分）
    quint32 m_dwDiscountedOverweightDue;    //	折扣后应收部分超限金额（分）
    quint32 m_dwMaxSeconds;         //	最大行驶时间 - 判断超时用
    quint32 m_dwMinSeconds;         //	最小行驶时间 - 判断超速用
    int m_nDiscountRate;            //	折扣率，0表示无折扣，1000表示全免
    QList<CSectMoney> m_SplitInfo;  //	拆分信息
    QList<quint32> m_PassStaList;   //	经过收费站
    QList<STollCellValue> m_PassedTollCellList;     //	经过的拆分单元信息
    QList<TFreeInfo> m_FreeInfoList;  ////免费信息 List of PFreeInfo;
public:
    VehTollInfoClass m_vehTollInfo; //	收费车辆信息（从输入复制，但包含车辆超限率等信息）


public:
    FareQueryResultClass()
    {
        clear();
    }
    void clear()
    {
        m_dwRealMeters=0;
        m_dwTollMeters=0;
        m_fCentDue=0;
        m_fCentFree=0;
        m_fCentOverweightDue=0;
        m_dwMoneyDue=0;
        m_dwMoneyFree=0;
        m_dwMoneyOverweightDue=0;
        m_dwDiscountedDue=0;
        m_dwDiscountedFree=0;
        m_dwDiscountedOverweightDue=0;
        m_dwMaxSeconds=0;
        m_dwMinSeconds=0;
        m_nDiscountRate=0;
        m_SplitInfo.clear();
        m_PassStaList.clear();
        m_PassedTollCellList.clear();
        //liujian 免费区域信息清空
        m_FreeInfoList.clear();
    }
    void operator =(const FareQueryResultClass &b)
    {
        m_dwRealMeters=b.m_dwRealMeters;
        m_dwTollMeters=b.m_dwTollMeters;
        m_fCentDue=b.m_fCentDue;
        m_fCentFree=b.m_fCentFree;
        m_fCentOverweightDue=b.m_fCentOverweightDue;
        m_dwMoneyDue=b.m_dwMoneyDue;
        m_dwMoneyFree=b.m_dwMoneyFree;
        m_dwMoneyOverweightDue=b.m_dwMoneyOverweightDue;
        m_dwDiscountedDue=b.m_dwDiscountedDue;
        m_dwDiscountedFree=b.m_dwDiscountedFree;
        m_dwDiscountedOverweightDue=b.m_dwDiscountedOverweightDue;
        m_dwMaxSeconds=b.m_dwMaxSeconds;
        m_dwMinSeconds=b.m_dwMinSeconds;
        m_nDiscountRate=b.m_nDiscountRate;
        m_SplitInfo=b.m_SplitInfo;
        m_PassStaList=b.m_PassStaList;
        m_PassedTollCellList=b.m_PassedTollCellList;
    }
    // 对分段计算结果进行汇总、取整，传入最小收费金额（单位为分）
    void SumSectMoney(const quint32 dwMinMoney);
    // 对计算结果进行拆分
    void SplitMoney();
    // 修改折扣后金额（注意：拆分基于折扣后金额），并根据修改后的金额重新拆分，用于欠费、分段付款等情况
    // 因欠费、分段付款不折扣，此函数将折扣率修改为0
    void ModifyDiscountedMoney(quint32 dwRealDue, bool bClearFree);
    // 进行折扣，折扣后重新拆分
    void Discount(int nDiscountRate);
    // 加倍
    void Multiply(int nTimes);
    // 返回折扣后金额（不更改其中的应收通行费）
    quint32 GetDiscountedDutyMoney(int nDiscountRate) const;
    // 获取字符串型的拆分结果，返回填充的拆分路段数
    int GetSplitInfo(char *szSplitInfo, size_t nSizeOfSplitInfo) const;
    // 获取字符串型收费站结果，返回值为收费站数量
    int GetStationInfo(char *szStationInfo, size_t nSizeOfStationInfo) const;
    // 获取字符串收费单元和收费单元实际里程，返回值为收费单元数
    int GetTollCellInfo(char *szTollCellInfo, size_t nSizeOfTollCell) const;
    /////////////////YN(wumax)
    // 获取字符串型的免费区间信息，返回填充的免费区间数
    int GetFreeInfo(char *szFreeInfo,size_t nSizeOfFreeInfo) const;
    // 转换免费区域信息//wumax yn
    bool TranFreeInfo();
    /////////////////
private:
      //转换路段免费
      bool TranFreeInfo_Road();
      //转换相邻站集合免费(相邻站两端在集合内才免)
      bool TranFreeInfo_Station();
      //转换相邻起止站集合免费(起止站在集合内才免)
      bool TranFreeInfo_StationEnEx();
      //增加一个免费段
      void AddFreeInfo(QList<quint32> FreeList, const TFreeInfo &FreeInfo);
      ///////////////////

};

#endif // FAREQUERYRESULTCLASS_H
