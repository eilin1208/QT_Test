#ifndef VEHQUEUEMGR_H
#define VEHQUEUEMGR_H
#include <QMutexLocker>
#include <QMutex>

class VehQueueMgr
{
public:
    VehQueueMgr();

public:
    //更新车辆记数
    int SetAllowPass(int nCount);

    //清空车辆记数
    void SetRefusePass(bool bResetCount = false);

    //车辆通过处理
    bool CardOnBackLoop();

    //返回允许过车数
    int VehCountAllowPass();

private:
    int m_nVehCountAllowPass;
    QMutex m_locker;

};

#endif // VEHQUEUEMGR_H
