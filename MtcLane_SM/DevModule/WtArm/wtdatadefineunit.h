#ifndef WTDATADEFINEUNIT_H
#define WTDATADEFINEUNIT_H
#include <QObject>
#include <FareCalc/FareCalcFunc.h>

/*****************************************************************************
 *本单元说明：计重数据保存，用于保存车辆详细的计重信息
 ****************************************************************************/

//计重数据处理状态
enum TWtDataStatus{

    dsRaw,              //原始称重信息
    dsEdit,             //编辑过的称重信息
    dsSplit,            //拆分
    dsMerge             //合并
};
struct TAxis
{
    quint32 FWeight;
};

class TWtVehicle : public QList<TAxis>
{

public:
    TWtVehicle(){}
    TWtVehicle(const TWtVehicle& source);
    TWtVehicle& operator =(const TWtVehicle& source);
    virtual ~TWtVehicle(){}
    //清除计重信息
    void Clear();
public:
    //对于原始的计重数据是通过累加计算各轴获得
    //对于手工插入的计重数据,直接读取成员变量的值
    //轴数
    void setAxisCount(quint8 AxisCount)
    {
        m_FAxisCount = AxisCount;
    }
    //总重
    void setWeight(quint32 Weight)
    {
        m_FWeight = Weight;
    }
    //速度
    void setSpeed(double speed)
    {
        m_FSpeed = speed;
    }

    double getSpeed()
    {
        return m_FSpeed;
    }


    //数据状态
    void setFDataStatus(TWtDataStatus DataStatus)
    {
        m_FDataStatus= DataStatus;
    }
    TWtDataStatus getDataStatus()
    {
        return m_FDataStatus;
    }
    //轴组信息
    void setFAxisInfo(QString AxisInfo)
    {
        m_FAxisInfo = AxisInfo;
    }
    QString getFAxisInfo() const
    {
        return m_FAxisInfo;
    }
    //修改前轴组信息
//    property PreAxisInfo: string read FPreAxisInfo write FPreAxisInfo;
    void setFPreAxisInfo(QString PreAxisInfo)
    {
        m_FPreAxisInfo = PreAxisInfo;
    }
    QString getFPreAxisInfo() const
    {
        return m_FPreAxisInfo;
    }
    //修改前总轴数
    void setFPreAxisCount(quint8 PreAxisCount)
    {
        m_FPreAxisCount = PreAxisCount;
    }
    quint8 getFPreAxisCount() const
    {
        return m_FPreAxisCount;
    }
    //修改前总重
    void setFPreWeight(int PreWeight)
    {
        m_FPreWeight = PreWeight;
    }
    int getFPreWeight() const
    {
        return m_FPreWeight;
    }

    //修改前限载
    void setFPreLimitWeight(int PreLimitWeight)
    {
        m_FPreLimitWeight = PreLimitWeight;
    }
    int getFPreLimitWeight() const
    {
        return m_FPreLimitWeight;
    }
    //修改前超限率
    void setFPreOverloadRate(quint32 PreOverloadRate)
    {
        m_FPreOverloadRate = PreOverloadRate;
    }
    quint32 getFPreOverloadRate() const
    {
        return m_FPreOverloadRate;
    }
private:
    //轴组数
    quint8 m_FAxisCount;
    //车速
    double m_FSpeed;
    //车辆总重
    quint32 m_FWeight;
    //计重数据状态
    TWtDataStatus m_FDataStatus;
    //轴组信息
    QString m_FAxisInfo;
    //修改前轴组数
    quint8 m_FPreAxisCount;
    //修改前车辆总重
    int m_FPreWeight;
    //修改前限载
    int m_FPreLimitWeight;
    //修改前超限率
    quint32 m_FPreOverloadRate;
    //修改前轴组信息
    QString m_FPreAxisInfo;
//private:
public:
    //返回轴数
    quint8 GetAxisCount();
    //返回限载
    quint32 GetLimitWeight();
    //返回超限率
    quint32 GetOverloadRate();
    //返回车辆总重
    quint32 GetWeight();
    //返回计重数据状态名称
    QString GetDataStatusName();
    //初始化计重信息
    void InitData();
    bool operator ==(const TWtVehicle& veh);
};
#endif // WTDATADEFINEUNIT_H
