#ifndef WTARM_H
#define WTARM_H

#include <QObject>
#include "BaseInterface/devbaseinterface.h"
#include "axisinfo.h"
#include "CommonModule/qpausablethread.h"

// 计重设备线程 - 用于定时取计重数据、检测设备状态
class WtArm;
class CWeightDevDetectThread : public QPausableThread
{
    Q_OBJECT
public:
    CWeightDevDetectThread():QPausableThread()
    {
        setObjectName("WeightDevDetectThread");
        m_pWeightDev = NULL;
    }
    void SetWeightDev(WtArm *pWeightDev)
    {
        m_pWeightDev = pWeightDev;
    }

protected:
    virtual void RunOnce();

protected:
    WtArm *m_pWeightDev;
};
class WtArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit WtArm(const QString& sDevName, QObject *parent = 0);
    
signals:
    void VehCount(int count);
public slots:
protected:
    bool LoadFunction();
    bool ReleaseFunction();
    bool Init();
    bool Close();
    // 串口名称
    //char m_szStdComPortName[20];
    // 用于判断计重信息是否发生变化
    quint32 m_dwFirstVehWeight;
    int m_nVehCount;
    bool m_bTruckTollByWt;
protected:
    int WimSetCom(char *szComNo, int nBps);
    int WimInit(int nIniType);
    int WimTest(quint8 *pbStatus);
    int WimSynchro();
    int WimClearOne();
    int WimGetVehCount();
    int WimGetVehData(int nVehicleID, long *pnWeight, long *pnWtLimit, int naAxisType[10], double *pfSpeed);
    int WimGetAxisCount(int nVehicleID);
    int WimGetAxisData(int nVehicleID, int nAxisID, int *pnAxisType, long naTyreWeight[4]);
    // 检查计重数据变化，通知监听者进行界面更新等操作
    void CheckWimDataChange();
    // 取车辆数据（使用旧函数）
    int GetVehAxisInfo(CVehAxisInfo *pVehAxisInfo, int nVehicleID);
public:
    //
    // 获取计重动态库数据
    void GetWeightData();
    bool UpdateWimDevStatus();

    // 返回计重设备驱动是否已加载
    bool GetWimDriverLoaded()
    {
        return m_bDriverLoaded;
    }
    //
    //删除第一辆车
    bool DeleteFirstVehicle();
    //删除指定车辆
    bool DeleteVehicle(int AIndex);//后来为满足新需求而增加
    //修改首辆车
    bool ModifyFirstVeh(int nWeight, int nWeightStandard, CVehAxisInfo *pVehAxisInfo_Before, CVehAxisInfo *pVehAxisInfo_After);
    //插入首辆车
    bool InsertFirstVeh(int nWeight, int nWeightStandard);
    //拆分首辆车
    bool SplitFirstVeh(int nIndex);
    //合并前两辆车
    bool MergeFirstAndSecondVeh();
    //删除所有车辆
    void DeleteAllVeh();
private:
    QMutex m_mutex;
    // 存储修改后的车辆计重数据
    CLaneAxisInfo m_ModifiedLaneAxisInfo;
    // 车辆数据（作为临时局部变量使用）
    CVehAxisInfo m_VehAxisInfo;
    // 计重日志记录详细级别（0-只记录偶尔调用函数调用过程，1-记录一般函数调用过程，2-记录循环检测函数调用过程）
    int m_nLogDetail;
    // 计重设备是否已打开
    bool m_bWimDevOpened;
    //新车辆计重信息列表
    CWeightDevDetectThread m_wtThread;

};

#endif // WTARM_H
