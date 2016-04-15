#include "wtlib.h"
#include "wtthread.h"
WtThread* getThread()
{
    static WtThread* g_thread = NULL;
    if(g_thread == NULL)
    {
        g_thread = new WtThread();
    }
    return g_thread;
}

int  WimDev_SetCom(char *comNo, int  bps)
{
    return getThread()->WimDevSetComImpl(comNo, bps);
}
int  WimDev_Init(int IniType)
{
    return getThread()->WimDevInitImpl(IniType);
}
int WimDev_Test(char * bStatus)
{
    return getThread()->WimDevTestImpl(bStatus);
}
int WimDev_Synchro(void)
{
    return getThread()->WimDevSynchroImpl();
}
int WimDev_ClearOne (void)
{
    return getThread()->WimDevClearOneImpl();
}
int WimDev_GetCount(void)
{
    return getThread()->WimDevGetCountImpl();
}
int WimDev_Data(int VehicleID,long *Weight,long *WtLimit,int AxisType[10],double *speed)
{
    return getThread()->WimDevDataImpl(VehicleID, Weight, WtLimit, AxisType, speed);
}
int WimDev_AxisCount(int VehicleID)
{
    return getThread()->WimDevAxisCountImpl(VehicleID);
}
int WimDev_AxisData(int VehicleID, int AxisID,int *AxisType, long TyreWeight[4])
{
    return getThread()->WimDevAxisDataImpl(VehicleID, AxisID, AxisType, TyreWeight);
}
int WimDev_AxisInfo(int VehicleID,long AxisWeight [10],double *speed)
{
    return getThread()->WimDevAxisInfoImpl(VehicleID, AxisWeight, speed);
}
int WimDev_AxisCount1(int VehicleID)
{
    return getThread()->WimDevAxisCount1Impl(VehicleID);
}
int WimDev_AxisData1(int VehicleID, int AxisID,int *AxisSpeed, int *Weight)
{
    return getThread()->WimDevAxisData1Impl(VehicleID, AxisID, AxisSpeed, Weight);
}
int WimDev_ClearOneData (int VehicleID)
{
    return getThread()->WimDevClearOneDataImpl(VehicleID);
}

int WimDev_GetVerNo(char *VerNo)
{
    return getThread()->WimDevGetVerNoImpl(VerNo);
}

int WimDev_Stop()
{
    getThread()->WimDevStopImpl();
    return 0;
}
