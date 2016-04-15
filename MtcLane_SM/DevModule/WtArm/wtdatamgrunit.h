/*****************************************************************************
 *本单元说明：计重数据管理类，管理计重设备的打开关闭以及对计重数据的编辑及维护。
 *作者： 刘博
 ****************************************************************************/
#ifndef WTDATAMGRUNIT_H
#define WTDATAMGRUNIT_H
#include "wtdevunit.h"
#include "wtdatadefineunit.h"
#include <QObject>


//引起数据变化的因素有两类:
//A:自动变化的 比如 新车 倒车                   (1变0, 0变1 时需要刷新首辆车数据)
//B:人工变化的 比如 编辑,插入,合并,拆分,删除    (都需要刷新首辆车数据)
enum TWtDataChangeCause {
    dccNewVeh,   //来了一辆新车
    dccBackVeh,  //倒出了一辆车
    dccDeleteVeh, //删除
    dccModifyVeh, //修改
    dccInsertVeh, //插入
    dccMergeVeh,  //合并
    dccSpitVeh    //拆分
};


class TWtDataMgr :public QObject
{
    Q_OBJECT
public:
    explicit TWtDataMgr(QObject* parent = 0);
    virtual ~TWtDataMgr();

signals:

    //计重状态变化
    void WtDataStatusChanged(int ACause,int AffectedVehID);
public slots:
    //新车辆数据事件
    void NewVehDataEvent(TWtVehicle *AVehicle);
    //倒车事件
    void BackVehEvent();
    //计重信息状态改变
    void DoWtDevStatusChange(int AStatus);
public:

    //取第一辆车,AxisInfo是调用方已经实例化的TWtVehicle
    //内部实现为复制数据到AxisInfo
    bool GetFirstVehAxisInfo(TWtVehicle **AxisInfo);
    //取一辆车 内部实现是Create一辆车并复制所有轴,需要调用方释放
    TWtVehicle* GetVehicle(int AIndex);
    //在最前面插入车辆     (界面需要刷新首辆车数据, 车辆数也变化了)
    //此处的Insert意指收费员手工造了一辆车的数据
    void Insert(int AIndex,quint32 Weight,quint8 AxisCount);
    void Modify(int AIndex,quint32 Weight,quint8 AxisCount);
    //合并前两辆车         (界面需要刷新首辆车数据, 车辆数也变化了)
    void Merge(int AIndex);
    //将第一辆拆分成两辆车 (界面需要刷新首辆车数据, 车辆数也变化了)
    void Split(int AIndex,quint8 AxisID);
    //删除车
    void Delete(int AIndex,bool SaveToPreVeh=false);
    //返回车辆总数
    int GetVehicleCount();

    void setDiscountRate(double discountRate)
    {
        m_FDiscountRate = discountRate;
    }
    double getDiscountRate()
    {
        return m_FDiscountRate;
    }
    TWtVehicle* RefersVehicle(int AIndex);
    TWtVehicle* getPreVehicle()
    {
        return m_pPreVehicle;
    }
    void setWtDev(TWtDev* pDev);


private:
    //车辆计重信息列表
    QList<TWtVehicle*> m_VehList;
    //设备误差折扣率
    float m_FDiscountRate;
    //计重设备线程
    TWtDev *FWtDev;
    //计重数据
    TWtVehicle* m_pPreVehicle;
private:
    //计重信息发生变化
    void DoWtDataChange(TWtDataChangeCause ACause,int AffectedVehID);

    //刷新计重信息
    //将DataMgr中的车辆序号转换为实际设备车辆的序号,删车时使用
    int GetDevVehID(int ALogicID);
    //保存修改前的计重信息
    void SavePreData(TWtVehicle* veh);
    int m_VehicleCount;
};

TWtDataMgr* getWtDataMgr();


#endif // WTDATAMGRUNIT_H
