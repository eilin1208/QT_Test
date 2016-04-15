#include "vehqueuemgr.h"
#include "devicefactory.h"
#include <QTime>
#include "laneinfo.h"
VehQueueMgr::VehQueueMgr()
{
}

int VehQueueMgr::SetAllowPass(int nCount)
{
    QMutexLocker locker(&m_locker);
    m_nVehCountAllowPass += nCount;
    //记录日志
    if(m_nVehCountAllowPass>0)
    {
        getDeviceFactory()->getIOBoard()->SetOneVehPass();
        if(getLaneInfo()->getControlDevType() == 1)
        {
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->SetPassLight(true);
            }
        }
    }
    return nCount;
}

void VehQueueMgr::SetRefusePass(bool bResetCount)
{
    QMutexLocker locker(&m_locker);
    //bResetCount非空，就清空
    if(bResetCount)
    {
        m_nVehCountAllowPass = 0;
    }

    bool flag=getDeviceFactory()->getIOBoard()->IsLockBar();
    if((m_nVehCountAllowPass==0)&&(!flag))
    {
        getDeviceFactory()->getIOBoard()->SetOneVehHasPassed();
        if(getLaneInfo()->getControlDevType() == 1)
        {
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->SetPassLight(false);
            }
        }
    }
}

int VehQueueMgr::VehCountAllowPass()
{
    QMutexLocker locker(&m_locker);
    return m_nVehCountAllowPass;
}

bool VehQueueMgr::CardOnBackLoop()
{
    QMutexLocker locker(&m_locker);
    if(m_nVehCountAllowPass > 0)
    {
        m_nVehCountAllowPass--;
        if(0 == m_nVehCountAllowPass && !getDeviceFactory()->getIOBoard()->IsLockBar())
        {
            getDeviceFactory()->getIOBoard()->SetOneVehPass();
            if(getLaneInfo()->getControlDevType() == 1)
            {
                if(getDeviceFactory()->getFD() != NULL)
                {
                    getDeviceFactory()->getFD()->SetPassLight(true);
                }
            }
        }
        return true;
    }
    return false;
}
