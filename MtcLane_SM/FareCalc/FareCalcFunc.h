#ifndef FARE_CALC_FUNC_H
#define FARE_CALC_FUNC_H

#include <QtGlobal>

// 按二舍八入、三七作五原则进行取整
quint32 roundFareTo5Yuan(double fCent);

// 四舍五入取整
inline quint32 round_int32(double f)
{
    return quint32(f+0.5);
}

// 四舍五入取整
inline quint64 round_int64(double f)
{
    return quint64(f+0.5);
}

// 计算拆分金额
quint32 splitMoney(double fPart, double fTotal, quint32 dwTotal);
// 根据轴数计算限载
quint32 getWeightLimit(int nAxisNum);
// 计算限载、超限率
quint32 CalcWeightLimitAndOverweightRate(int &nOverweightRate, int nAxisNum, quint32 nTotalWeight, quint32 nWeightLimit=0);
// 根据速度（公里/小时）、里程（米）计算车辆行驶时间（秒）
quint32 calcDriveTime(quint32 meters, quint16 kph);
// 计算折扣后金额，金额输入/输出单位，按四舍五入原则精确至分
quint32 discountedMoney(quint32 dwCent, int nDiscountRate);

#endif // FARE_CALC_FUNC_H



