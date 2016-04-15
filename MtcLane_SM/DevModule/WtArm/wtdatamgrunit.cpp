#include "wtdatamgrunit.h"

TWtDataMgr::TWtDataMgr(QObject *parent):
     QObject(parent),m_VehicleCount(0)
{

}

TWtDataMgr::~TWtDataMgr()
{
    if(m_pPreVehicle != NULL)
    {
        delete m_pPreVehicle;
    }
}

void TWtDataMgr::Merge(int AIndex)
{
    TWtVehicle* veh1 = NULL;
    TWtVehicle* veh2 = NULL;

    int DevVehID = 0;
    if (m_VehicleCount < 2)
    {
        return ;
    }

    if (AIndex >= m_VehicleCount)
    {
        return ;
    }
    veh1 = RefersVehicle(AIndex);
    veh2 = RefersVehicle(AIndex + 1);

    if ((veh1->getDataStatus() == dsEdit)||(veh2->getDataStatus() == dsEdit))
    {
        return;
    }

    DevVehID = GetDevVehID(AIndex);
    if (veh1->getDataStatus() == dsRaw)
    {
        FWtDev->DeleteVehicle(DevVehID);
    }

    if (veh2->getDataStatus() == dsRaw)
    {
        FWtDev->DeleteVehicle(DevVehID);
    }
    SavePreData(veh1);
    int tmpCount = veh2->count();
    for (int i = 0;i < tmpCount;i++)
    {
        veh1->append(veh2->first());
        veh2->removeFirst();
    }
    veh1->setFDataStatus(dsMerge);
    m_VehList.removeOne(veh2);
    delete veh2;
    DoWtDataChange(dccMergeVeh, AIndex);
}
//删除指定的车辆
void TWtDataMgr::Delete(int AIndex, bool SaveToPreVeh)
{
    TWtVehicle* veh = NULL;
    if (AIndex > m_VehicleCount)
    {
        LogMsg("lane",tr("[TWtDataMgr.Delete]AIndex不能大于车辆数"));
        return ;
    }

    if (m_VehList.count() == 0)
    {
        return ;
    }

    veh = RefersVehicle(AIndex);
    if (veh->getDataStatus() == dsRaw)
    {
        FWtDev->DeleteVehicle(GetDevVehID(AIndex));
    }

    //list中的每辆车最终都经由此进行free
    if(SaveToPreVeh)
    {
        if(m_pPreVehicle != NULL)
        {
            delete m_pPreVehicle;
        }
        m_pPreVehicle = veh;
        m_VehList.removeOne(veh);
    }
    else
    {
        delete m_VehList.at(AIndex - 1);
        m_VehList.removeAt(AIndex - 1);
    }
    DoWtDataChange(dccDeleteVeh, AIndex);

}
//返回指定车辆的计重信息
TWtVehicle *TWtDataMgr::GetVehicle(int AIndex)
{
    if (AIndex > m_VehicleCount)
    {
        return NULL;
    }
    return RefersVehicle(AIndex);
}
//在AIndex前插入一辆车
void TWtDataMgr::Insert(int AIndex,quint32 Weight,quint8 AxisCount)
{
    if (AIndex < 1 )
    {
        //::showWarning(tr("AIndex必须大于0"));
        return;
    }
    TWtVehicle* veh = new TWtVehicle();
    veh->setWeight(Weight);
    veh->setAxisCount(AxisCount);
    veh->setFDataStatus(dsEdit) ;
    SavePreData(veh);
    m_VehList.insert((AIndex - 1),veh);
    DoWtDataChange(dccInsertVeh, AIndex);
}
//将AIndex车辆 从AxisID轴开始拆分1.至少有一辆车 2.对于输入或修改过的车辆不能拆分
void TWtDataMgr::Split(int AIndex, quint8 AxisID)
{
    TWtVehicle *veh1 = NULL;
    TWtVehicle *veh2 = NULL;
    int DevVehID = 0;
    if (AIndex > m_VehicleCount)
    {
        return;
    }

    veh1 = RefersVehicle(AIndex);
    if (veh1->getDataStatus() == dsEdit)
    {
        return;
    }

    if (AxisID >= veh1->count())
    {
        return;
    }
    if (veh1->getDataStatus() == dsRaw)
    {
        DevVehID = GetDevVehID(AIndex);
        FWtDev->DeleteVehicle(DevVehID);
    }
    SavePreData(veh1);
    veh1->setFDataStatus(dsSplit);

    //   veh2 = TWtVehicle.Create;
    veh2 = new TWtVehicle();
    veh2->setFDataStatus(dsSplit);
    veh2->setSpeed(veh1->getSpeed());
    m_VehList.insert(AIndex,veh2);

    int tmpCount = veh1->count();
    TAxis tmpWt;
    for(int i=AxisID;i < tmpCount; i++)
    {
        tmpWt = veh1->at(AxisID);
        veh1->removeAt(AxisID);
        veh2->append(tmpWt);
    }
    veh2->setFPreAxisInfo(veh1->getFPreAxisInfo());
    veh2->setFPreAxisCount(veh1->getFPreAxisCount()) ;
    veh2->setFPreWeight(veh1->getFPreWeight()) ;
    veh2->setFPreLimitWeight( veh1->getFPreLimitWeight());
    veh2->setFPreOverloadRate(veh1->getFPreOverloadRate()) ;
    DoWtDataChange(dccSpitVeh, AIndex);
}
//倒车事件_TODO
void TWtDataMgr::BackVehEvent()
{
    int id = m_VehicleCount;
    if (id == 0)
    {
        return;
    }
    delete m_VehList.last();
    m_VehList.removeLast();
    DoWtDataChange(dccBackVeh, id);    //1->0时
}

//新车辆数据事件_TODO
void TWtDataMgr::NewVehDataEvent(TWtVehicle* AVehicle)
{
    m_VehList.append(AVehicle);
    DoWtDataChange(dccNewVeh, m_VehicleCount);    //0->1时
}

//返回当前车道中的车辆总数
int TWtDataMgr::GetVehicleCount()
{
    return m_VehList.count();
}
//计重设备状态的变化
void TWtDataMgr::DoWtDevStatusChange(int AStatus)
{
    int i;
    for(i= 0 ;i<=m_VehList.count()- 1;i++)
    {
    }
}
//更新计重数据
TWtVehicle* TWtDataMgr::RefersVehicle(int AIndex)
{
    if (AIndex > m_VehicleCount)
    {
        return NULL;
    }
    return m_VehList.at(AIndex-1);
}

void TWtDataMgr::setWtDev(TWtDev *pDev)
{
    FWtDev = pDev;
    connect(FWtDev, SIGNAL(newVehEvent(TWtVehicle*)), this, SLOT(NewVehDataEvent(TWtVehicle*)));
    connect(FWtDev, SIGNAL(backVehEvent()), this, SLOT(BackVehEvent()));
    connect(FWtDev, SIGNAL(StatusChanged(int)), this, SLOT(DoWtDevStatusChange(int)));
}
//计重信息发生变化
void TWtDataMgr::DoWtDataChange(TWtDataChangeCause ACause, int AffectedVehID)
{
    m_VehicleCount = m_VehList.count();
    emit WtDataStatusChanged(ACause, AffectedVehID);
}
//修改计重信息
void TWtDataMgr::Modify(int AIndex, quint32 Weight, quint8 AxisCount)
{
    TWtVehicle* veh;
    if(AIndex > m_VehicleCount)
    {
        return;
    }

    veh = RefersVehicle(AIndex);
    if (veh->getDataStatus() == dsRaw)
    {
        FWtDev->DeleteVehicle(GetDevVehID(AIndex));
    }

    SavePreData(veh);
    veh->setWeight(Weight);
    veh->setAxisCount(AxisCount);
    veh->setFDataStatus(dsEdit);
    DoWtDataChange(dccModifyVeh, AIndex);
}
//将DataMgr中的车辆序号转换为实际设备车辆的序号,删车时使用
int TWtDataMgr::GetDevVehID(int ALogicID)
{
    TWtVehicle* veh;
    int Result = 0;
    for (int i = 0;i<=ALogicID - 1;i++)
    {
        veh = m_VehList.at(i);
        if (veh->getDataStatus() == dsRaw)
        {
            Result = Result + 1;
        }
    }
    return Result;
}
//返回第一辆车的计重信息
bool TWtDataMgr::GetFirstVehAxisInfo(TWtVehicle** AxisInfo)
{
    TWtVehicle *v;
    v = RefersVehicle(1);
    if(v != NULL)
    {
        *AxisInfo = v;
        return true;
    }
    else
    {
        *AxisInfo = NULL;
        return false;
    }
}
//保存计重修改前信息
void TWtDataMgr::SavePreData(TWtVehicle* veh)
{
    veh->setFPreAxisInfo(veh->getFAxisInfo());
    veh->setFPreAxisCount(veh->GetAxisCount());
    veh->setFPreWeight(veh->GetWeight());
    veh->setFPreLimitWeight(veh->GetLimitWeight());
    veh->setFPreOverloadRate(veh->GetOverloadRate());
}

TWtDataMgr* getWtDataMgr()
{
    static TWtDataMgr mgr;
    return &mgr;
}
