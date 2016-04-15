#ifndef WTDATADEFINEUNIT_H
#define WTDATADEFINEUNIT_H
#include <QObject>
#include <QString>
#include <QDateTime>
class WtDataDefineUnit
{
public:
    WtDataDefineUnit();

};

//计重数据处理状态
enum TWtDataStatus
{
    dsRaw,            //原始称重信息
    dsEdit,             //编辑过的称重信息
    dsSplit,            //拆分
    dsMerge             //合并
};
class TAxis
{
private:
    quint32 FWeight;
public:
    TAxis(){}

    //设置车辆总重
    void setWeight(quint32 Weight)
    {
        FWeight =Weight;
    }

    quint32 getFWeight() const
    {
        return FWeight;
    }
    bool operator ==(const TAxis& lp);
};

class TWtVehicle:public QList<TAxis>
{
private:
    //轴组数
    quint8 FAxisCount;
    //车速
    double FSpeed;
    //车辆总重
    quint32 FWeight;
    //计重数据状态
    TWtDataStatus FDataStatus;
    //轴组信息
    QString FAxisInfo;
    //修改前轴组数
    quint8 FPreAxisCount;
    //修改前车辆总重
    int  FPreWeight;
    //修改前限载
    int  FPreLimitWeight;
    //修改前超限率
    quint32 FPreOverloadRate;
    //修改前轴组信息
    QString  FPreAxisInfo;

public:

    //清除计重信息
    void Clear(); //override;
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
    //对于原始的计重数据是通过累加计算各轴获得
    //对于手工插入的计重数据,直接读取成员变量的值

    //返回轴数

    void setFAxisCount(quint8 value)
    {
        FAxisCount=value;
    }

    void setFWeight(quint32 value)
    {
        FWeight=value;

    }

    //速度
    double GetFSpeed()
    {
        return FSpeed;
    }

    void setFSpeed(double value)
    {
        FSpeed=value;
    }
    //数据状态
    TWtDataStatus  GetFDataStatus()
    {
        return FDataStatus;
    }
    void setFDataStatus(TWtDataStatus TWvalue)
    {
        FDataStatus=TWvalue;
    }
    //轴组信息
    QString  GetFAxisInfo()
    {
        return FAxisInfo;
    }
    void setFAxisInfo(QString value)
    {
        FAxisInfo=value;
    }

    //修改前轴组信息
    QString GetFPreAxisInfo()
    {
        return FPreAxisInfo;
    }

    void setFPreAxisInfo(QString value)
    {
        FPreAxisInfo=value;
    }

    //修改前总轴数
    quint8 GetFPreAxisCount()
    {
        return FPreAxisCount;
    }

    void setFPreAxisCount(quint8 value)
    {
        FPreAxisCount=value;
    }

    //修改前总重
    int  GetFPreWeight()
    {
        return FPreWeight;
    }

    void setFPreWeight(int value)
    {
        FPreWeight=value;
    }

    //修改前限载
    int  GetFPreLimitWeight()
    {
        return FPreLimitWeight;
    }
    void setFPreLimitWeight(int value)
    {
        FPreLimitWeight=value;
    }

    void setFPreOverloadRate(quint32 value)
    {
        FPreOverloadRate=value;
    }
};


#endif // WTDATADEFINEUNIT_H
