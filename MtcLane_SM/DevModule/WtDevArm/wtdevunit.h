#ifndef WTDEVUNIT_H
#define WTDEVUNIT_H
#include <QObject>
#include <QString>
#include <QMutexLocker>
#include "CommonModule/laneutils_new.h"
#include "wtdatadefineunit.h"
#include "BaseInterface/devbaseinterface.h"
#include <QList>
#include <QLibrary>
#include "CommonModule/qpausablethread.h"
//TNewVehDataEvent =TWtVehicle AVehicle;//to do
// //倒车事件
// TBackVehEvent = procedure() of object;//to do
// //设备状态变化事件
// TDevStatusChangeEvent = procedure(ANewStatus: Byte) of object;// to do
class TWtDev: public QPausableThread
{
    Q_OBJECT
public:
    explicit TWtDev(QObject* parent=0);
    virtual ~TWtDev();
    bool Init(quint8 nPort, int bps);
    bool LoadFunction();
    bool ReleaseFunction();
    bool Close();
protected:
    void RunOnce();
private:
    //动态库是否已加载
    bool FDriverLoaded;
    //动态库名
    QString FDllFileName;
    //等待次数
    int  FWaitCount;
    //设备中的车辆数
    int FDevVehCount;
    //上一次设备状态
    char FOldStatus;
    QLibrary m_qLibrary;
    //设备状态变化事件
    // TDevStatusChangeEvent FOnDevStatusChange;  to do
    //倒车事件
    // TBackVehEvent   FOnBackVeh ;
    //新车辆数据事件
    // TNewVehDataEvent FOnNewVehData ; to do
    //同步处理新车事件的时候使用,是个引用
    TWtVehicle  FCurrVeh;
    //同步处理倒车事件的时候使用,表示倒出了几辆车
    int FBackVehCount;
    //重量折扣
    double  FDiscountRate;
    //新车辆计重信息列表
    QList<TWtVehicle> FNewVehList;
public:
    //注销动态库
    //检测设备状态
    void TestDevStatus();
    //同步车辆计重信息
    void SynWeightData();
    //取指定车辆的计重信息
    TWtVehicle GetVehFromDev(int AIndex);
    //处理设备状态变化
    void DoDevStatusChange();
    //处理新车数据
    void DoNewVeh();
    //处理倒车数据
    void DoBackVeh();
    //通知上层有车辆计重数据发生变化
    void NotifyMainThread();

    //删除第一辆车
    bool DeleteFirstVehicle();
    //删除指定车辆
    bool DeleteVehicle(int AIndex);//后来为满足新需求而增加
    //动态库名
    QString GetFDllFileName();
    void setFDllFileName(QString volue);

    // TNewVehDataEvent  GetFOnNewVehData(); todo
    // void setFOnNewVehData(TNewVehDataEvent vlaueVehData); to du
    //    TBackVehEvent GetFOnBackVeh()
    //   void setFOnBackVeh(TBackVehEvent vlaueBackVeh); todu
    //    TDevStatusChangeEvent  GetFOnDevStatusChange(); todu
    //    void setFOnDevStatusChange(TDevStatusChangeEvent StatuschaneEvent); todu
    //重量折扣
    float GetFDiscountRate();
    void setFDiscountRate(float countRate);
    void setLibName(QString name);
signals:
public slots:
private:
    QMutex m_mutex;
    bool m_bLoadflag;

};
#endif // WTDEVUNIT_H

