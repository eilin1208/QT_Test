#include "wtdatadefineunit.h"
#include "CommonModule/ilogmsg.h"

double g_wtDevErrorRate = 0.98;
quint8 TWtVehicle::GetAxisCount()
{
    if(m_FDataStatus == dsEdit)
    {
        return m_FAxisCount;
    }
    else
    {
        return count();
    }
}
//计重数据状态名称
QString TWtVehicle::GetDataStatusName()
{
    switch(m_FDataStatus)
    {
    case dsRaw:
        return "原始";
        break;
    case dsEdit:
        return "输入";
        break;
    case dsSplit:
        return "分割";
        break;
    case dsMerge:
        return "合并";
        break;
    default :
        return "未知";
        break;
    }

}
quint32 TWtVehicle::GetLimitWeight()
{
    return getWeightLimit(GetAxisCount());
}

quint32 TWtVehicle::GetOverloadRate()
{
    int OverloadRate = 0;
    //此函数从山东代码移植而来
    //nAxisNum参数!=0时,会按照轴数计算限载,从而得出超限率
    //nAxisNum=0时,会直接使用传入的dwWeightLimit参数来计算超限率
    CalcWeightLimitAndOverweightRate(OverloadRate, GetAxisCount(), GetWeight());
    return OverloadRate;
}
quint32 TWtVehicle::GetWeight()
{
    quint32 wt = 0;
    if (m_FDataStatus == dsEdit)
    {
        return m_FWeight;
    }
    else
    {
        for (int i= 0;i < count();i++)
        {
            LogMsg("lane", QObject::tr("获取计重数据，[%1]轴原始重量为：[%2],矫正后为[%3]")
                   .arg(i).arg(at(i).FWeight).arg(at(i).FWeight * g_wtDevErrorRate));
            wt = wt + (quint32)(at(i).FWeight * g_wtDevErrorRate);
        }
        return wt;
    }
}

bool TWtVehicle::operator ==(const TWtVehicle& veh)
{
    if(this->count() != veh.count())
    {
        return false;
    }
    for(int i = 0; i < count(); i++)
    {
        if(at(i).FWeight != veh.at(i).FWeight)
        {
            return false;
        }
    }
    if(m_FAxisCount != veh.m_FAxisCount ||
            m_FSpeed != veh.m_FSpeed ||
            m_FWeight != veh.m_FWeight ||
            m_FDataStatus != veh.m_FDataStatus ||
            m_FAxisInfo != veh.m_FAxisInfo ||
            m_FPreAxisCount != veh.m_FPreAxisCount ||
            m_FPreWeight != veh.m_FPreWeight ||
            m_FPreLimitWeight != veh.m_FPreLimitWeight ||
            m_FPreOverloadRate != veh.m_FPreOverloadRate ||
            m_FPreAxisInfo != veh.m_FPreAxisInfo
            )
    {
        return false;
    }
    return true;
}

void TWtVehicle::InitData()
{
    m_FAxisCount = 0;
    m_FSpeed = 0;
    m_FWeight = 0;
    m_FDataStatus = dsRaw;
    m_FAxisInfo = "";
    m_FPreAxisCount = 0;
    m_FPreWeight = 0;
    m_FPreLimitWeight = 0;
    m_FPreOverloadRate = 0;
    m_FPreAxisInfo = "";
}
TWtVehicle::TWtVehicle(const TWtVehicle &source)
{
    *this = source;
}

TWtVehicle& TWtVehicle::operator =(const TWtVehicle& source)
{
    Clear();
    append(source);
    m_FWeight = source.m_FWeight;
    m_FSpeed = source.m_FSpeed;
    m_FDataStatus = source.m_FDataStatus;
    m_FAxisInfo = source.m_FAxisInfo;
    m_FPreAxisCount = source.m_FPreAxisCount;
    m_FPreWeight = source.m_FPreWeight;
    m_FPreLimitWeight = source.m_FPreLimitWeight;
    m_FPreOverloadRate = source.m_FPreOverloadRate;
    m_FPreAxisInfo = source.m_FPreAxisInfo;
    return *this;
}

void TWtVehicle::Clear()
{
    clear();
    InitData();
}
