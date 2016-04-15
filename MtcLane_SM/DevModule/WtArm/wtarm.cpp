#include "wtarm.h"
typedef int (*WimDev_SetCom)(char *comNo, int  bps);
typedef int (*WimDev_Init)(int IniType);
typedef int (*WimDev_Test)(char * bStatus);
typedef int (*WimDev_Synchro)(void);
typedef int (*WimDev_ClearOne)(void);
typedef int (*WimDev_GetCount)(void);
typedef int (*WimDev_Data)(int VehicleID,long *Weight,long *WtLimit,int AxisType[10],double *speed);
typedef int (*WimDev_AxisCount)(int VehicleID);
typedef int (*WimDev_AxisData)(int VehicleID, int AxisID,int *AxisType, long TyreWeight[4]);
typedef int (*WimDev_AxisInfo)(int VehicleID,long AxisWeight [10],double *speed);
typedef int (*WimDev_AxisCount1)(int VehicleID);
typedef int (*WimDev_AxisData1)(int VehicleID, int AxisID,int *AxisSpeed, int *Weight);
typedef int (*WimDev_ClearOneData)(int VehicleID);

WimDev_SetCom DevSetCom = NULL;
WimDev_Init DevInit = NULL;
WimDev_Test DevTest = NULL;
WimDev_Synchro DevSynchro = NULL;
WimDev_ClearOne DevClearOne = NULL;
WimDev_GetCount DevGetCount = NULL;
WimDev_Data DevData = NULL;
WimDev_AxisCount DevAxisCount = NULL;
WimDev_AxisData DevAxisData = NULL;
WimDev_AxisInfo DevAxisInfo = NULL;
WimDev_AxisCount1 DevAxisCount1 = NULL;
WimDev_AxisData1 DevAxisData1 = NULL;
WimDev_ClearOneData DevClearOneData = NULL;

WtArm::WtArm(const QString &sDevName, QObject *parent) :
    DevBaseInterface(sDevName,parent)
{
    m_nVehCount = 0;
    m_dwFirstVehWeight = 0;
    QSettings settings("./Lane.ini", QSettings::IniFormat);
    m_bTruckTollByWt = (settings.value("Setup/UseWeight").toInt() == 1);
    //qDebug()<<"m_bTruckTollByWt"<<m_bTruckTollByWt;
}

bool WtArm::LoadFunction()
{
    if(m_bDriverLoaded)
    {
        return true;
    }
    //qDebug() << "WT LoadFunction";

    DevSetCom = (WimDev_SetCom)m_qLibrary.resolve("WimDev_SetCom");
    DevInit = (WimDev_Init)m_qLibrary.resolve("WimDev_Init");
    DevTest = (WimDev_Test)m_qLibrary.resolve("WimDev_Test");
    DevSynchro = (WimDev_Synchro)m_qLibrary.resolve("WimDev_Synchro");
    DevClearOne = (WimDev_ClearOne)m_qLibrary.resolve("WimDev_ClearOne");
    DevGetCount = (WimDev_GetCount)m_qLibrary.resolve("WimDev_GetCount");
    DevData = (WimDev_Data)m_qLibrary.resolve("WimDev_Data");
    DevAxisCount = (WimDev_AxisCount)m_qLibrary.resolve("WimDev_AxisCount");
    DevAxisData = (WimDev_AxisData)m_qLibrary.resolve("WimDev_AxisData");
    DevAxisInfo = (WimDev_AxisInfo)m_qLibrary.resolve("WimDev_AxisInfo");
    DevAxisCount1 = (WimDev_AxisCount1)m_qLibrary.resolve("WimDev_AxisCount1");
    DevAxisData1 = (WimDev_AxisData1)m_qLibrary.resolve("WimDev_AxisData1");
    DevClearOneData = (WimDev_ClearOneData)m_qLibrary.resolve("WimDev_ClearOneData");
    if(NULL == DevSetCom ||
            NULL == DevInit ||
            NULL == DevTest ||
            NULL == DevSynchro ||
            NULL == DevClearOne ||
            NULL == DevGetCount ||
            NULL == DevData ||
            NULL == DevAxisCount ||
            NULL == DevAxisData ||
            NULL == DevAxisInfo ||
            NULL == DevAxisCount1 ||
            NULL == DevAxisData1 ||
            NULL == DevClearOneData)
    {
        LogMsg("lane", tr("获取计重动态库[%1]中的一个或多个函数错误").arg(m_sLibName));
        return false;
    }
    m_bDriverLoaded = true;
    ////qDebug() << "WT INIT END";
    return true;
}

bool WtArm::ReleaseFunction()
{
    DevSetCom = NULL;
    DevInit = NULL;
    DevTest = NULL;
    DevSynchro = NULL;
    DevClearOne = NULL;
    DevGetCount = NULL;
    DevData = NULL;
    DevAxisCount = NULL;
    DevAxisData = NULL;
    DevAxisInfo = NULL;
    DevAxisCount1 = NULL;
    DevAxisData1 = NULL;
    DevClearOneData = NULL;
    m_bDriverLoaded = false;
    return true;
}

bool WtArm::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    ////qDebug() << "WT INIT";
    //Close();
    int nRlt = WimSetCom(m_sConStr1.toLocal8Bit().data(), m_sConStr2.toInt());
    if(0 != nRlt)
    {
        ////qDebug()<<"打开计重设备失败,Error Code = "<<nRlt;
        LogMsg("lane", tr("打开计重设备失败，Error Code=%1").arg(nRlt));
        emit StatusChanged(DEV_STATUS_ParamErr);
        return false;
    }
    ////qDebug()<<"打开"<<objectName()<<"成功";
    LogMsg("lane", tr("打开%1成功").arg(objectName()));
    nRlt = WimSynchro();
    if(0 != nRlt)
    {
        LogMsg("lane", tr("程序启动时进行计重设备同步失败，Error Code=%1").arg(nRlt));
    }
    nRlt = WimInit(6);
    if(nRlt < 0)
    {
        LogMsg("lane", tr("程序启动时调用WimDev_Init失败，Error Code=%1").arg(nRlt));
    }
    //改变状态
    emit StatusChanged(DEV_STATUS_OK);
    //设置线程并启动
    m_wtThread.SetWeightDev(this);
    m_wtThread.Resume();
    return true;
}

bool WtArm::Close()
{
    //关闭线程
   ////qDebug()<< "close wt dev";
    m_wtThread.Stop();
    return true;
}

int WtArm::WimSetCom(char *szComNo, int nBps)
{
    if(NULL == DevSetCom)
    {
        return -1;
    }
    int nRlt = DevSetCom(szComNo, nBps);
    m_bIsOpen = (0 == nRlt);
    return nRlt;
}

int WtArm::WimInit(int nIniType)
{
    //////qDebug() << "mutex" << "WimInit";
    QMutexLocker locker(&m_mutex);
    if(NULL == DevInit)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    int nRlt = DevInit(nIniType);
    return nRlt;
}

int WtArm::WimTest(quint8 *pbStatus)
{
    QMutexLocker locker(&m_mutex);
    if(NULL == DevTest)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    *pbStatus = 0;
    int nRlt = DevTest((char*)pbStatus);
    return nRlt;
}

int WtArm::WimSynchro()
{
    //////qDebug() << "mutex" << "WimSynchro";
    QMutexLocker locker(&m_mutex);
    if(NULL == DevSynchro)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    int nRlt = DevSynchro();
    return nRlt;
}

int WtArm::WimClearOne()
{
    //QMutexLocker locker(&m_mutex);
    if(NULL == DevClearOne)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    int nRlt = DevClearOne();
    return nRlt;
}

int WtArm::WimGetVehCount()
{
    QMutexLocker locker(&m_mutex);
    if(NULL == DevGetCount)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    const int MAXCOUNT = 99;
    int nRlt = DevGetCount();
    if(nRlt > MAXCOUNT)
    {
        return -1;
    }
    return nRlt;
}

int WtArm::WimGetVehData(int nVehicleID, long *pnWeight, long *pnWtLimit, int naAxisType[], double *pfSpeed)
{
    //////qDebug() << "mutex" << "WimGetVehData";

    QMutexLocker locker(&m_mutex);
    if(NULL == DevData)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return - 1;
    }
    int nRlt = DevData(nVehicleID, pnWeight, pnWtLimit, naAxisType, pfSpeed);
    return nRlt;
}

int WtArm::WimGetAxisCount(int nVehicleID)
{
    //////qDebug() << "mutex" << "WimGetAxisCount";

    QMutexLocker locker(&m_mutex);
    if(NULL == DevAxisCount)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    const int MAXCOUNT = 99;
    int nRlt = DevAxisCount(nVehicleID);
    if(nRlt > MAXCOUNT)
    {
        return -1;
    }
    return nRlt;
}

int WtArm::WimGetAxisData(int nVehicleID, int nAxisID, int *pnAxisType, long naTyreWeight[])
{
    //////qDebug() << "mutex" << "WimGetAxisData";
    QMutexLocker locker(&m_mutex);
    if(NULL == DevAxisData)
    {
        return -1;
    }
    if(!m_bIsOpen)
    {
        return -1;
    }
    int nRlt = DevAxisData(nVehicleID, nAxisID, pnAxisType, naTyreWeight);
    return nRlt;
}

void WtArm::CheckWimDataChange()
{
    // 只有第一辆车总重变化或车辆总数发生变化才刷新界面
    CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
    quint32 dwFirstVehWeight=m_VehAxisInfo.GetTotalWeight();
    int nVehCount=CLaneAxisInfo::GetLaneAxisInfo()->GetVehCount();
    ////qDebug()<<"nVehCount"<<nVehCount;
    //qDebug()<<"m_dwFirstVehWeight="<<m_dwFirstVehWeight<<"dwFirstVehWeight="<<dwFirstVehWeight
           //<<"m_nVehCount"<<m_nVehCount<<"m_bTruckTollByWt"<<m_bTruckTollByWt;
    if ((m_dwFirstVehWeight!=dwFirstVehWeight ||
            m_nVehCount!=nVehCount) &&
            m_bTruckTollByWt)
    {
        m_dwFirstVehWeight=dwFirstVehWeight;
        m_nVehCount=nVehCount;
        //m_bTruckTollByWt = !getLaneInfo()->getUseWeight();
        emit VehCount(m_nVehCount);
    }
}

bool WtArm::DeleteFirstVehicle()
{
    bool rlt = false;
    int VehCount=WimGetVehCount();
    m_mutex.lock();
    if( VehCount > 0)
    {
        rlt = WimClearOne() == 0;
//        if (m_nVehCount > 0)
//        {
//            m_nVehCount= m_nVehCount - 1;
//        }
        m_ModifiedLaneAxisInfo.RemoveFirst();
    }
    m_mutex.unlock();
    return rlt;
}

bool WtArm::DeleteVehicle(int AIndex)
{
    bool ret;
    int VehCount;
    m_mutex.lock();
    VehCount=DevGetCount();

    if (VehCount > 0)
    {
        ret = (DevClearOneData(AIndex)== 0);
//        if (m_nVehCount > 0)
//        {
//            m_nVehCount= m_nVehCount - 1;
//        }
        m_mutex.unlock();
    }
    return true;
}

bool WtArm::ModifyFirstVeh(int nWeight, int nWeightStandard, CVehAxisInfo *pVehAxisInfo_Before, CVehAxisInfo *pVehAxisInfo_After)
{
    bool bRlt;
    if (m_ModifiedLaneAxisInfo.GetVehCount()==0)
    {
        // 已编辑车辆队列中为空，修改原始称重信息
        bRlt=CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
        if (!bRlt)
        {
            return false;
        }
        if (pVehAxisInfo_Before)
        {
            *pVehAxisInfo_Before=m_VehAxisInfo;
        }
        m_VehAxisInfo.ModifyVehInfo(nWeight, nWeightStandard);
        if (pVehAxisInfo_After)
        {
            *pVehAxisInfo_After=m_VehAxisInfo;
        }
        // 已编辑车辆队列中增加一辆车
        m_ModifiedLaneAxisInfo.AddVeh(m_VehAxisInfo);
        // 通知计重设备删除一辆车的信息
        WimClearOne();
    }
    else
    {
        bRlt=m_ModifiedLaneAxisInfo.GetFirstVeh(&m_VehAxisInfo);
        if (!bRlt)
        {
            return false;
        }
        if (pVehAxisInfo_Before)
        {
            *pVehAxisInfo_Before=m_VehAxisInfo;
        }
        m_VehAxisInfo.ModifyVehInfo(nWeight, nWeightStandard);
        if (pVehAxisInfo_After)
        {
            *pVehAxisInfo_After=m_VehAxisInfo;
        }
        m_ModifiedLaneAxisInfo.ModifyVeh(m_VehAxisInfo);
    }
    // 修改CLaneAxisInfo::GetLaneAxisInfo()
    CLaneAxisInfo::GetLaneAxisInfo()->ModifyVeh(m_VehAxisInfo);
    // 刷新界面显示
    CheckWimDataChange();
    return true;
}

bool WtArm::InsertFirstVeh(int nWeight, int nWeightStandard)
{
    m_ModifiedLaneAxisInfo.AddVeh(nWeight,nWeightStandard);
    CLaneAxisInfo::GetLaneAxisInfo()->AddVeh(nWeight,nWeightStandard);
    CheckWimDataChange();
    return true;
}

bool WtArm::SplitFirstVeh(int nIndex)
{
    if (nIndex<=0)
    {
        return false;
    }
    bool bRlt;
    if (m_ModifiedLaneAxisInfo.GetVehCount()==0)
    {
        bRlt=CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
        if (!bRlt)
        {
            // 无车辆可供拆分
            return false;
        }
        if (nIndex>=m_VehAxisInfo.GetAxisGroupCount())
        {
            return false;
        }
        // 取出原始轴重信息，转到已编辑车辆队列中，准备拆分
        m_ModifiedLaneAxisInfo.AddVeh(m_VehAxisInfo);
        WimClearOne();
    }
    else
    {
        m_ModifiedLaneAxisInfo.GetFirstVeh(&m_VehAxisInfo);
        if (nIndex>=m_VehAxisInfo.GetAxisGroupCount())
        {
            return false;
        }
    }
    bRlt=m_ModifiedLaneAxisInfo.SplitVeh(nIndex);
    if (!bRlt)
    {
        //qDebug()<<"Warning: CWeightDev::SplitFirstVeh()调用m_ModifiedLaneAxisInfo.SplitVeh(%1)出现不一致！";
        LogMsg("lane",tr("Warning: CWeightDev::SplitFirstVeh()调用m_ModifiedLaneAxisInfo.SplitVeh(%1)出现不一致！").arg(nIndex));

    }
    bRlt=CLaneAxisInfo::GetLaneAxisInfo()->SplitVeh(nIndex,NULL);
    if (!bRlt)
    {
        //qDebug()<<"Warning: CWeightDev::SplitFirstVeh()调用GetLaneAxisInfo()->SplitVeh(%1)出现不一致！";
        LogMsg("lane",tr("Warning: CWeightDev::SplitFirstVeh()调用GetLaneAxisInfo()->SplitVeh(%1)出现不一致！").arg(nIndex));
    }
    // 刷新界面显示
    CheckWimDataChange();
    return true;
}

bool WtArm::MergeFirstAndSecondVeh()
{
    bool bRlt=false;
    if (m_ModifiedLaneAxisInfo.GetVehCount()==0)
    {
        CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
        m_ModifiedLaneAxisInfo.AddVeh(m_VehAxisInfo);
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
        m_ModifiedLaneAxisInfo.AddVeh(m_VehAxisInfo);
        bRlt=m_ModifiedLaneAxisInfo.CombineFirst2Veh();
        if (!bRlt)
        {
            LogMsg("lane","CWeightDev::MergeFirstAndSecondVeh(): 错误0！");
            return false;
        }
        m_ModifiedLaneAxisInfo.GetFirstVeh(&m_VehAxisInfo);
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->AddVeh(m_VehAxisInfo,true);
        WimClearOne();
    }
    else if (m_ModifiedLaneAxisInfo.GetVehCount()==1)
    {
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->GetFirstVeh(&m_VehAxisInfo);
        m_ModifiedLaneAxisInfo.AddVeh(m_VehAxisInfo);
        bRlt=m_ModifiedLaneAxisInfo.CombineFirst2Veh();
        if (!bRlt)
        {
            //qDebug()<<"CWeightDev::MergeFirstAndSecondVeh(): 错误1！";
            LogMsg("lane","CWeightDev::MergeFirstAndSecondVeh(): 错误1！");
            return false;
        }
        m_ModifiedLaneAxisInfo.GetFirstVeh(&m_VehAxisInfo);
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->AddVeh(m_VehAxisInfo,true);
        WimClearOne();
    }
    else
    {
        bRlt=m_ModifiedLaneAxisInfo.CombineFirst2Veh();
        if (!bRlt)
        {
            return false;
        }
        m_ModifiedLaneAxisInfo.GetFirstVeh(&m_VehAxisInfo);
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->RemoveFirst();
        CLaneAxisInfo::GetLaneAxisInfo()->AddVeh(m_VehAxisInfo,true);
    }
    CheckWimDataChange();
    return true;
}

void WtArm::DeleteAllVeh()
{
    WimInit(0);
    m_ModifiedLaneAxisInfo.RemoveAll();
    CLaneAxisInfo::GetLaneAxisInfo()->RemoveAll();
    CheckWimDataChange();
}



void WtArm::GetWeightData()
{
    if (!UpdateWimDevStatus())
    {
        //qDebug()<<"!UpdateWimDevStatus";
        return;
    }
    /*
            int nResult;
            nResult=WimSynchro();
            if (0!=nResult)
            {
                    return;
            }
    */
    static int nPreviousVehCount=0;

    int nCount=WimGetVehCount();
    //qDebug()<<"nCount = "<<nCount;
    if (nCount<0)
    {

        // 取车辆数失败
        return;
    }
    if (nPreviousVehCount==nCount)
    {
        static int nTryIntervel=0;
        nTryIntervel++;
        //qDebug()<<"nTryIntervel = "<<nTryIntervel<<"nTryIntervel<10 可防止系统频繁调用计重接口库";
        if (nTryIntervel<10)
        {
            // 防止系统频繁调用计重接口库
            return;
        }
        nTryIntervel=0;
    }
    nPreviousVehCount=nCount;
    // 先取已编辑车辆的车辆信息
    QList<CVehAxisInfo> NewVehList;
    m_ModifiedLaneAxisInfo.GetAllVeh(&NewVehList);
    //qDebug()<<"nCount"<<nCount;
    for (int i=1;i<=nCount;i++)
    {
        CVehAxisInfo VehAxisInfo;
        //qDebug()<<"GetVehAxisInfo(&VehAxisInfo, i)="<<GetVehAxisInfo(&VehAxisInfo, i);
        if (GetVehAxisInfo(&VehAxisInfo, i)!=0)
        {
            // 取车辆数据失败，改取下一辆车的数据
            continue;
        }
        // 将取到的车辆数据加到列表中
        NewVehList.push_back(VehAxisInfo);
    }
    // NOTE: 对数据的更改由一个函数进行，以便保证数据的完整
    CLaneAxisInfo::GetLaneAxisInfo()->CopyVehList(NewVehList);
    CheckWimDataChange();
}

bool WtArm::UpdateWimDevStatus()
{
    quint8 bSubDevStatus;
    int nResult=WimTest(&bSubDevStatus);
    //qDebug()<<"nResult = "<<nResult;
    if (0!=nResult)
    {
        // 进行设备测试返回失败，认为计重设备与工控机之间出现通信故障
        //this->ChangeStatus(DEV_STATUS_CommErr);
        emit StatusChanged(DEV_STATUS_CommErr);
        return false;
    }
    //this->ChangeStatus(DEV_STATUS_OK);
    emit StatusChanged(DEV_STATUS_OK);
    // 分析处理计重设备零件的故障
    CLaneAxisInfo::GetLaneAxisInfo()->SetWeightDevStatus(bSubDevStatus);
    static quint8 bPreviousSubDevStatus=0;
    if (bPreviousSubDevStatus==bSubDevStatus)
    {
        return true;
    }
    bPreviousSubDevStatus=bSubDevStatus;

    return true;
}

int WtArm::GetVehAxisInfo(CVehAxisInfo *pVehAxisInfo, int nVehicleID)
{
    int nRlt;
    long nRawTotalWeight;
    long nWeightLimit;
    double fVehSpeed;
    int naAxisType[10];
    for (int i=0;i<sizeof(naAxisType);i++)
    {
        naAxisType[i]=0;
    }

    nRlt=WimGetVehData(nVehicleID, &nRawTotalWeight, &nWeightLimit, naAxisType, &fVehSpeed);
    //qDebug()<<"nRlt = "<<nRlt;
    if (nRlt<0)
    {
        // 取车辆数据失败，退出
        return -1;
    }
    int nAxisCount=WimGetAxisCount(nVehicleID);
    if (nAxisCount!=naAxisType[0])
    {
        LogMsg("lane",tr("WimDev_Data取轴组数(%1)与WimDev_AxisCount取轴组数(%2)不一致，使用WimDev_AxisCount返回值").arg(naAxisType[0]).arg(nAxisCount));
    }
    if (nAxisCount<=0)
    {
        // 取轴组数失败，或者轴组数为0（视为失败）
        return -1;
    }
    int nSumOfRawAxisWeight=0;
    QList<CAxisGroupData> AxisGroupList;
    for (int nAxisId=1; nAxisId<=nAxisCount; nAxisId++)
    {
        int nAxisGroupType;
        long naTyreWeight[4];
        for (int i=0;i<4;i++)
        {
            naTyreWeight[i]=0;
        }
        nRlt=WimGetAxisData(nVehicleID, nAxisId, &nAxisGroupType, naTyreWeight);
        if (nRlt<0)
        {
            // 取轴组数据失败
            return -1;
        }
        // 旧函数没有轴组速度，使用车辆速度代替
        CAxisGroupData AxisGroupData(nAxisGroupType, fVehSpeed, naTyreWeight, sizeof(naTyreWeight));
        AxisGroupList.push_back(AxisGroupData);
        // 对轴重进行求和
        nSumOfRawAxisWeight+=AxisGroupData.m_nAxisGroupWeightRaw;
    }
    if (nRawTotalWeight!=nSumOfRawAxisWeight)
    {
        LogMsg("lane",tr("WimDev_Data取车辆总重(%1)与WimDev_AxisData取轴重之和(%2)不一致，使用WimDev_AxisData返回值").arg(nRawTotalWeight).arg(nSumOfRawAxisWeight));
    }
    if (nSumOfRawAxisWeight<=0)
    {
        LogMsg("lane",tr("WimDev_AxisData取轴重之和(%1)异常，取车辆数据失败").arg(nSumOfRawAxisWeight));
        return -1;
    }
    pVehAxisInfo->SetAxisGroupList(AxisGroupList, fVehSpeed);
    return 0;
}

void CWeightDevDetectThread::RunOnce()
{

    if (m_pWeightDev->GetWimDriverLoaded())
    {
        //qDebug()<<"In RunOnce is Load";
        m_pWeightDev->GetWeightData();
        msleep(600);
    }
    else
    {
        msleep(2000);
        //qDebug()<<"In RunOnce do not Load";
    }
}

