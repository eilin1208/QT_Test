#include "wtdatamgrunit.h"

bool WtDataMgr::Init()
{
    //    FWtDev.DLLFileName= ExtractFilePath(Application.ExeName) +  "WimDev.dll";
    //    FWtDev.DiscountRate= FDiscountRate;

    //    FWtDev.OnNewVeh= NewVehDataEvent;
    //    FWtDev.OnBackVeh= BackVehEvent;
    //    FWtDev.OnDevStatusChange= DevStatusChangeEvent;
    //FWtDev->Init(m_sConStr1.toInt(),m_sConStr2.toInt());

    FWtDev.Init(m_sConStr1.toInt(),m_sConStr2.toInt());

    return true;
}
bool WtDataMgr::Close()
{
    qDebug() << "begin release wt";
    FWtDev.Close();
    qDebug() << "end release wt";
    return true;
}

bool WtDataMgr::LoadFunction()
{
    FWtDev.setLibName(m_sLibName);
    return FWtDev.LoadFunction();
}

bool WtDataMgr::ReleaseFunction()
{
    return FWtDev.ReleaseFunction();
}

int WtDataMgr::GetVehicleCount()
{
    return FVehList.count();
}

void WtDataMgr::setFDiscountRate(float value)
{
    FDiscountRate=value;
}

float WtDataMgr::GetFDiscountRate()
{
    return FDiscountRate;
}

void WtDataMgr::Delete(int AIndex, bool SaveToPreVeh)
{
    TWtVehicle veh;

    if (AIndex>GetVehicleCount())
    {
        LogMsg("lane",tr("[TWtDataMgr.Delete]AIndex不能大于车辆��"));
        return;
    }

    if (FVehList.count()== 0)
    {
        return;
    }

    veh= GetVehicle(AIndex);
    if (veh.GetFDataStatus()==dsRaw)
        FWtDev.DeleteVehicle(GetDevVehID(AIndex));

    //list中的每辆车最终都经由此进行free
    if (SaveToPreVeh)
    {
        FPreVehicle = veh;
        FVehList.append(veh);
    }
    else
    {
        FVehList.removeAt(AIndex- 1);
    }
    DoWtDataChange(dccDeleteVeh, AIndex);
    FVehList.clear();

}
//�� �� AIndex: Integer; AxisID: Byte
//  返回�� ��
//  �� �� 将AIndex车辆 从AxisID轴开始拆��
//            1.至少有一辆车 2.对于输入或修改过的车辆不能拆��
void WtDataMgr::Split(int AIndex, quint8 AxisID)
{
    int  i, j;
    TWtVehicle  veh1, veh2;
    int DevVehID;
    if (AIndex > GetVehicleCount())
    {
        LogMsg("lane",tr("AIndex必须小于或等于车辆总数."));
        return;
    }
    veh1=GetVehicle(AIndex);

    if (veh1.GetFDataStatus() == dsEdit)
    {
        LogMsg("lane",tr("输入的车辆不能再拆分."));
        return;
    }

    if (AxisID >= veh1.count())
    {
        LogMsg("lane",tr("轴序号必须小于总轴��"));
        return;
    }

    if (veh1.GetFDataStatus()== dsRaw)
    {
        DevVehID= GetDevVehID(AIndex);
        FWtDev.DeleteVehicle(DevVehID);
    }

    SavePreData(veh1);
    veh1.setFDataStatus(dsSplit);

    // veh2 = TWtVehicle.Create;
    veh2.setFDataStatus(dsSplit);
    veh2.setFSpeed(veh1.GetFSpeed());
    FVehList.insert(AIndex, veh2);

    j= veh1.count() ;
    for( i= AxisID;i<j;i++)
    {
        veh2.append(veh1.at(AxisID));
    }

    veh2.setFPreAxisInfo(veh1.GetFPreAxisInfo());
    veh2.setFPreAxisCount(veh1.GetFPreAxisCount());
    veh2.setFPreWeight(veh1.GetFPreWeight());
    veh2.setFPreLimitWeight(veh1.GetFPreLimitWeight());
    veh2.setFPreOverloadRate(veh1.GetOverloadRate());

    DoWtDataChange(dccSpitVeh, AIndex);
}

void WtDataMgr::Merge(int AIndex)
{

    TWtVehicle  veh1, veh2;
    int  DevVehID = 0;

    if (GetVehicleCount()< 2)
    {
        LogMsg("lane",tr("至少要有两辆车才可以合并."));
        return;
    }

    if (AIndex >=GetVehicleCount())
    {
        LogMsg("lane",tr("车辆序号必须小于车辆总数."));
        return;
    }

    veh1 = GetVehicle(AIndex);
    veh2 = GetVehicle(AIndex + 1);

    if (veh1.GetFDataStatus()==dsEdit||veh2.GetFDataStatus()==dsEdit)
    {
        LogMsg("lane",tr("输入的车辆不能合��"));
        return;
    }

    DevVehID= GetDevVehID(AIndex);
    if (veh1.GetFDataStatus()== dsRaw)
    {
        FWtDev.DeleteVehicle(DevVehID);
    }
    if (veh2.GetFDataStatus()== dsRaw)
    {
        FWtDev.DeleteVehicle(DevVehID);
    }
    SavePreData(veh1);

    for (int i= 0;i<veh2.count();i++)
    {
        veh1.append(veh2.first());
    }
    veh1.setFDataStatus( dsMerge);

    FVehList.removeAt( FVehList.indexOf(veh2));

    DoWtDataChange(dccMergeVeh, AIndex);

}

void WtDataMgr::Modify(int AIndex, quint32 Weight, quint8 AxisCount)
{
    TWtVehicle  veh;
    if (AIndex > GetVehicleCount())
    {
        LogMsg("lane",tr("AIndex必须小于或等于车辆总数."));
        return;

    }

    veh= GetVehicle(AIndex);

    if (veh.GetFDataStatus()== dsRaw)
    {
        FWtDev.DeleteVehicle(GetDevVehID(AIndex));
    }

    SavePreData(veh);

    veh.setFWeight(Weight);
    veh.setFAxisCount(AxisCount);
    veh.setFDataStatus(dsEdit);
    DoWtDataChange(dccModifyVeh, AIndex);
}

void WtDataMgr::Insert(int AIndex, quint32 Weight, quint8 AxisCount)
{
    TWtVehicle veh;

    if (AIndex < 1)
    {
        LogMsg("lane",tr("AIndex必须大于0."));
        return;

    }

    //veh =new  TWtVehicle();
    veh.setFWeight(Weight);
    veh.setFAxisCount(AxisCount);
    veh.setFDataStatus(dsEdit);

    SavePreData(veh);

    FVehList.insert((AIndex - 1), veh);

    DoWtDataChange(dccInsertVeh, AIndex);
}

bool WtDataMgr::GetVehicle(int AIndexr, TWtVehicle &veh)
{
    if(AIndexr > GetVehicleCount())
    {
        return false;
    }
    veh = FVehList.at(AIndexr);
    return true;
}

TWtVehicle WtDataMgr::GetVehicle(int AIndexr)
{
    TWtVehicle veh;
    if(AIndexr > GetVehicleCount())
    {
        return veh;
    }
    return FVehList.at(AIndexr);
}

bool WtDataMgr::GetFirstVehAxisInfo(TWtVehicle& AxisInfo)
{
    TWtVehicle v;
    bool nRet = GetVehicle(1, v);
    if(nRet)
    {
        AxisInfo = v;
        return true;
    }
    return false;
}

void WtDataMgr::SavePreData(TWtVehicle veh)
{
    veh.setFPreAxisInfo(veh.GetFAxisInfo());
    veh.setFPreAxisCount(veh.GetAxisCount());
    veh.setFPreWeight(veh.GetWeight());
    veh.setFPreLimitWeight(veh.GetLimitWeight());
    veh.setFPreOverloadRate(veh.GetOverloadRate());
}

int WtDataMgr::GetDevVehID(int ALogicID)
{
    TWtVehicle veh;

    int sult=0;
    for(int  i= 0;i< ALogicID;i++)
    {
        veh.append(FVehList[i]);
        if (veh.GetFDataStatus()== dsRaw)
        {
            sult= sult+1;
        }
    }
    return sult;
}

TWtVehicle WtDataMgr::RefersVehicle(int AIndex)
{
    if (AIndex >GetVehicleCount())
    {
        TWtVehicle tmp;

        return tmp;
    }

    return FVehList.at(AIndex - 1);

}

void WtDataMgr::DoWtDevStatusChange(quint8 AStatus)
{

}

void WtDataMgr::DoWtDataChange(TWtDataChangeCause ACause, int AffectedVehID)
{

}

void WtDataMgr::DevStatusChangeEvent(quint8 ANewStatus)
{
    DoWtDevStatusChange(ANewStatus);
}

void WtDataMgr::BackVehEvent()
{
    int id;
    id = GetVehicleCount();
    if (id == 0)
    {
        return;
    }
    FVehList.removeLast();

    DoWtDataChange(dccBackVeh, id);    //1->0��
}

void WtDataMgr::NewVehDataEvent(TWtVehicle AVehicle)
{
    FVehList.append(AVehicle);
    DoWtDataChange(dccNewVeh, GetVehicleCount());    //0->1��
}

WtDataMgr::WtDataMgr(const QString &sName)
{
    this->setObjectName(sName);
}


