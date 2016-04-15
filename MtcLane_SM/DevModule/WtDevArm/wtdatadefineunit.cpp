#include "wtdatadefineunit.h"
#include "FareCalc/FareCalcFunc.h"

WtDataDefineUnit::WtDataDefineUnit()
{
}

quint8 TWtVehicle::GetAxisCount()
{
    if (FDataStatus ==dsEdit)
        return FAxisCount;
    else
        return this->count();
}

quint32 TWtVehicle::GetLimitWeight()
{
    return getWeightLimit(GetAxisCount());
}

QString TWtVehicle::GetDataStatusName()
{
    switch (FDataStatus)
    {
    case dsRaw:
        return "原始";
    case dsEdit:
        return "输入";
    case dsSplit:
        return "分割";
    case dsMerge:
        return"合并";
    default:
        return "未知";
    }
}

quint32 TWtVehicle::GetOverloadRate()
{
    int OverloadRat=0;
    //此函数从山东代码移植而来
    //nAxisNum参数<>0时,会按照轴数计算限载,从而得出超限率
    //nAxisNum=0时,会直接使用传入的dwWeightLimit参数来计算超限率
    //CalcWeightLimitAndOverweightRate(OverloadRate, AxisCount, Weight);
    // return OverloadRate;
    return OverloadRat;
}

quint32 TWtVehicle::GetWeight()
{
    quint32 wt;
    if (FDataStatus==dsEdit)
    {
        return FWeight;
    }
    else
    {
        wt=0;
        for(int i= 0 ;i<this->count() ;i++)
        {
            wt = wt + this->at(i).getFWeight();
        }
        return wt;
    }

}

void TWtVehicle::InitData()
{
    FAxisCount = 0;
    FSpeed = 0;
    FWeight = 0;
    FDataStatus = dsRaw;
    FAxisInfo = "";

    FPreAxisCount = 0;
    FPreWeight = 0;
    FPreLimitWeight= 0;
    FPreOverloadRate = 0;
    FPreAxisInfo= "";
}


void TWtVehicle::Clear()
{
    //  inherited;
    //  InitData;
    InitData();
    this->clear();
}

bool TAxis::operator ==(const TAxis &lp)
{
    return lp.FWeight == this->FWeight;
}



