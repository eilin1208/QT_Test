#ifndef WTDATAMGRUNIT_H
#define WTDATAMGRUNIT_H
#include <QObject>
#include <QString>
#include "WtDevArm/wtdatadefineunit.h"
#include <QLibrary>
#include "BaseInterface/devbaseinterface.h"
#include "wtdevunit.h"
#include <QList>

//引起数据变化的因素有两类:
//A:自动变化的 比如 新车 倒车                   (1变0, 0变1 时需要刷新首辆车数据)
//B:人工变化的 比如 编辑,插入,合并,拆分,删除    (都需要刷新首辆车数据)
enum  TWtDataChangeCause
{
    dccNewVeh,   //来了一辆新车
    dccBackVeh,  //倒出了一辆车
    dccDeleteVeh, //删除
    dccModifyVeh, //修改
    dccInsertVeh, //插入
    dccMergeVeh,  //合并
    dccSpitVeh    //拆分
};
class WtDataMgr:public DevBaseInterface
{
    Q_OBJECT
public:

    WtDataMgr(const QString & sName);
    //新车辆数据事件
    void NewVehDataEvent(TWtVehicle AVehicle);
    //倒车事件
    void BackVehEvent();
    //设备状态变化事件
    void DevStatusChangeEvent(quint8 ANewStatus);
    //计重信息发生变化
    void DoWtDataChange(TWtDataChangeCause ACause,int AffectedVehID);
    //计重信息状态改变
    void DoWtDevStatusChange(quint8 AStatus);
    //刷新计重信息
    TWtVehicle RefersVehicle(int AIndex);
    //将DataMgr中的车辆序号转换为实际设备车辆的序号,删车时使用
    int GetDevVehID(int ALogicID);
    //保存修改前的计重信息
    void SavePreData(TWtVehicle veh);
    //{------------------------------------------------------------------------------
    // FWtDev的所有事件都是在线程中以Synchronize方式抛出的
    // 所以对于数据的操作(插入,删除,合并,拆分等)应该不需要保护即可
    //------------------------------------------------------------------------------}


    //取第一辆车,AxisInfo是调用方已经实例化的TWtVehicle
    //内部实现为复制数据到AxisInfo
    bool GetFirstVehAxisInfo(TWtVehicle& AxisInfo);
    //取一辆车 内部实现是Create一辆车并复制所有轴,需要调用方释放
    TWtVehicle GetVehicle(int AIndexr);
    bool GetVehicle(int AIndexr, TWtVehicle& veh);
    //在最前面插入车辆     (界面需要刷新首辆车数据, 车辆数也变化了)
    //此处的Insert意指收费员手工造了一辆车的数据
    void Insert(int AIndex, quint32 Weight,quint8 AxisCount);
    void Modify(int AIndex,quint32 Weight,quint8 AxisCount);
    //合并前两辆车         (界面需要刷新首辆车数据, 车辆数也变化了)
    void Merge(int AIndex);
    //将第一辆拆分成两辆车 (界面需要刷新首辆车数据, 车辆数也变化了)
    void Split(int AIndex, quint8 AxisID);
    //删除车
    void Delete(int AIndex, bool SaveToPreVeh= false);

    // void RegListener(IWtDataListener AListenner);
    //void UnRegListener( IWtDataListener AListenner);
    //设备误差折扣率 0.05 表示原始重量*(1-0.05)
    float GetFDiscountRate();
    void setFDiscountRate(float volue);
    int GetVehicleCount();
    //上一辆车
    TWtVehicle GetFPreVehicle();
    TWtVehicle GetRefersVehicle();
protected:
    //打开计重设备x5
    bool Init();
    bool Close();
    bool LoadFunction();
    bool ReleaseFunction();
private:
    //车辆计重信息列表
    // TObjectList FVehList ;
    QList <TWtVehicle> FVehList;
    //设备误差折扣率
    float FDiscountRate;
    //计重设备线程
    TWtDev FWtDev;
    // TInterfaceList FListenerLst;
    // QList  FListenerLst;
    //计重数据
    TWtVehicle FPreVehicle;

};
#endif // WTDATAMGRUNIT_H
