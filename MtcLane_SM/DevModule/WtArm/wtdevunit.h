#ifndef WTDEVUNIT_H
#define WTDEVUNIT_H
#include <QString>
#include <QThread>
#include "wtdatadefineunit.h"
#include <QObject>
#include "BaseInterface/devbaseinterface.h"
#include "CommonModule/CardFileDef.h"
#include "CommonModule/qpausablethread.h"
#include "CommonModule/cardfileconverter.h"

class TWtDev: public DevBaseInterface
{
    Q_OBJECT
public:
    explicit TWtDev(const QString& sName,QObject *parent = 0);
signals:
    //新车事件
    void newVehEvent(TWtVehicle* wtInfo);
    //倒车事件
    void backVehEvent();
    //设备状态事件StatusChanged

public:
    void setFDiscountRate(float nDiscountRate)
    {
        m_FDiscountRate = nDiscountRate;
    }
    float getFDiscountRate()
    {
        return m_FDiscountRate;
    }
    void Term()
    {
        m_bTerm = true;
    }

public:
    void run();
    //删除第一辆车
    bool DeleteFirstVehicle();
    //删除指定车辆
    bool DeleteVehicle(int AIndex); //后来为满足新需求而增加


protected:
    //加载动态库
    bool LoadFunction();
    //    //注销动态库
    bool ReleaseFunction();
    //初始化
    bool Init();
    //关闭
    bool Close();
private:
    //检测设备状态
    void TestDevStatus();
    //同步车辆计重信息
    void SynWeightData();
    //取指定车辆的计重信息
    TWtVehicle* GetVehFromDev(int AIndex);
    //处理设备状态变化
    void DoDevStatusChange();
    //处理新车数据
    void DoNewVeh();
    //处理倒车数据
    void DoBackVeh();
    //通知上层有车辆计重数据发生变化
    void NotifyMainThread();
private:
    quint32 m_FOldStatus;
    //等待次数
    int m_FWaitCount;
    //设备中的车辆数
    int m_FDevVehCount;
    //设备状态变化事件_已有
    //_TODO  TDevStatusChangeEvent FOnDevStatusChange;
    //同步处理新车事件的时候使用,是个引用
    TWtVehicle *m_pCurrVeh;
    //同步处理倒车事件的时候使用,表示倒出了几辆车
    int m_FBackVehCount;
    //重量折扣
    double m_FDiscountRate;
    //新车辆计重信息列表
    QList<TWtVehicle*> m_newVehList;
    //是否中断
    volatile bool m_bTerm;
    //线程锁
    QMutex m_mutex;
};



























#endif // WTDEVUNIT_H
