#include "FareCalcFunc.h"

// 按二舍八入、三七作五原则进行取整
//  输入：浮点型金额，单位为分
//  输出：整数型金额，单位为
quint32 roundFareTo5Yuan(double fCent)
{
    const quint32 LowToFive=300;
    const quint32 LowToTen=800;
    quint32 nTenYuan=(int)(fCent/1000);
    quint32 nYuan=nTenYuan*10;
    double fRemain=fCent-nYuan*100;
    if (fRemain<LowToFive)
    {
        // 余额舍弃
    }
    else if (fRemain<LowToTen)
    {
        nYuan+=5;
    }
    else
    {
        nYuan+=10;
    }
    return nYuan*100;
}

// 计算拆分金额
quint32 splitMoney(double fPart, double fTotal, quint32 dwTotal)
{
    if ((0==dwTotal) ||
        (fTotal<0.0000000001))
    {
        return 0;
    }
    quint32 dwRlt;
    dwRlt=round_int32(fPart/fTotal*dwTotal);
    return dwRlt;
}

// 根据轴数计算限载
quint32 getWeightLimit(int nAxisNum)
{
    // 分别定义轴数为1-8的车辆的限载
    const int MAX_AXIS=8;
    const quint32 WeightLimitArray[8]={17000, 17000, 25000, 35000, 43000, 49000, 49000, 49000};
    if (nAxisNum<=1)
    {
        return WeightLimitArray[0];
    }
    else if (nAxisNum<=MAX_AXIS)
    {
        return WeightLimitArray[nAxisNum-1];
    }
    else
    {
        return WeightLimitArray[MAX_AXIS-1];
    }
}

// 计算限载、超限率
quint32 CalcWeightLimitAndOverweightRate(int &nOverweightRate, int nAxisNum, quint32 nTotalWeight, quint32 nWeightLimit)
{
    const int nMaxOverweightRate=9999;
    // NOTE: 编辑计重信息时手工输入限载，此时计算费率的限载不应根据轴数查找
    if (0==nWeightLimit)
    {
        nWeightLimit=getWeightLimit(nAxisNum);
    }
    if (nTotalWeight>nWeightLimit)
    {
        quint32 nOverweight;
        nOverweight=nTotalWeight-nWeightLimit;
        if (0!=nWeightLimit)
        {
            nOverweightRate=round_int32((double)1000*nOverweight/nWeightLimit);
        }
        else
        {
            nOverweightRate=nMaxOverweightRate;
        }
    }
    else
    {
        nOverweightRate=0;
    }
    if (nOverweightRate>nMaxOverweightRate)
    {
        nOverweightRate=nMaxOverweightRate;
    }
    return nWeightLimit;
}

// 根据速度（公里/小时）、里程（米）计算车辆行驶时间（秒）
quint32 calcDriveTime(quint32 meters, quint16 kph)
{
    const quint32 dwSecondsPerHour=3600;
    const quint32 dwSecondsPerDay=24*dwSecondsPerHour;
    if (0==kph)
    {
        return 10*dwSecondsPerDay;
    }
    quint32 dwSeconds=meters*dwSecondsPerHour/(kph*1000);
    return dwSeconds;
}

// 计算折扣后金额，金额输入/输出单位，按四舍五入原则精确至分
quint32 discountedMoney(quint32 dwCent, int nDiscountRate)
{
    if (nDiscountRate<=0)
    {
        return dwCent;
    }
    if (nDiscountRate>=1000)
    {
        return 0;
    }
    quint64 llDiscountedPart=dwCent*nDiscountRate;
    quint32 dwDiscountedPart=quint32(llDiscountedPart/1000);
    quint32 dwRemain=quint32(llDiscountedPart-(quint64)dwDiscountedPart*1000);
    if (dwRemain>=500) dwDiscountedPart++;
    return (dwCent-dwDiscountedPart);
}
