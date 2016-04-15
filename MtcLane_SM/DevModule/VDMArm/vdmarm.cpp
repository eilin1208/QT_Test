#include "vdmarm.h"
#include "CommonModule/LaneType.h"
#include "CommonModule/laneutils_new.h"
VDMArm::VDMArm(const QString& sName, QObject *parent) :
    DevBaseInterface(sName,parent), pFunVdmLine(NULL),
    pFuncInit(NULL), pFuncSyncVdmTime(NULL),m_timerId(-1)
{
}

bool VDMArm::LoadFunction()
{
    pFunVdmLine = (Vdm_line)m_qLibrary.resolve("Vdm_line");
    pFuncInit = (Vdm_Init)m_qLibrary.resolve("Vdm_init");
    pFuncSyncVdmTime = (Sync_Vdm_Time)m_qLibrary.resolve("Sync_Vdm_Time");
    if(pFunVdmLine == NULL ||
            pFuncInit == NULL ||
            pFuncSyncVdmTime == NULL)
    {
        m_sErrString = m_qLibrary.errorString();
        return false;
    }
    return true;
}

bool VDMArm::ReleaseFunction()
{
    pFunVdmLine = NULL;
    return true;
}

bool VDMArm::Init()
{
    bool ret = false;
    if(m_bDriverLoaded)
    {
        ret = pFuncInit() == 0;
    }
    return ret;
}

void VDMArm::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerId)
    {
        if(m_bDriverLoaded)
        {
            pFuncSyncVdmTime();
        }
    }
    else
    {
        killTimer(event->timerId());
    }
}

bool VDMArm::Close()
{
    if(!m_bIsOpen)
    {
        return false;
    }
    return true;
}

bool VDMArm::UpdateLane(char *lpszStaName, int nLaneId)
{
    LogMsg("lane", tr("字符叠加设置车道名称[%1],ID[%2]").arg(lpszStaName).arg(nLaneId));
    if(!m_bIsOpen)
    {
        return false;
    }

    //    int ret = m_pFunVdmLine(0,0,strlen(lpszStaName),lpszStaName);
    //    if(ret== -1)
    //    {
    //        return false;
    //    }
    //    ret = m_pFunVdmLine(0, 8, 6, tr("%1").arg(nLaneId, 6, 10, QLatin1Char('0'))
    //                        .toLocal8Bit().data() );
    int ret = -1;
    if(nLaneId == 0)
    {
        ret= pFunVdmLine(0,0,24, "                        ");
    }
    else
    {
        QString laneId = QString::number(nLaneId);
        ret = pFunVdmLine(0,0,laneId.length(), laneId.toLocal8Bit().data());
    }
    return ret == 0;
}

bool VDMArm::UpdateCollector(quint32 dwCollectorId, bool bAddOffDuty)
{
    LogMsg("lane", tr("字符叠加设置操作员信息dwCollectorId[%1]").arg(dwCollectorId));
    if(!m_bIsOpen)
    {
        return false;
    }
    int ret = -1;
    if(dwCollectorId == 0)
    {
        ret = pFunVdmLine(1,0, 24, "                        ");
    }
    else
    {
        QString collectorId = QString::number(dwCollectorId);
        ret = pFunVdmLine(1,0,collectorId.length(),collectorId.toLocal8Bit().data());
    }
    return ret == 0;

}

bool VDMArm::UpdateVehClass(int nVehClass)
{
    //    LogMsg("lane", tr("字符叠加设置车型[%1]").arg(nVehClass));
    //    if(!m_bIsOpen)
    //    {
    //        return false;
    //    }
    //    QByteArray temp=Utf8toGB2312(GetVehClassName((CVehClass)nVehClass));
    //    int ret = m_pFunVdmLine(1, 0, temp.count(), temp.data());
    //    return ret != -1;
    return true;
}

bool VDMArm::UpdateVehTollType(int nVehTollType)
{
    //    LogMsg("lane", tr("字符叠加设置收费类型[%1]").arg(nVehTollType));
    //    if(!m_bIsOpen)
    //    {
    //        return false;
    //    }
    //    QString tmp=GetVehClassNoByName_LPCSTR((CVehType)nVehTollType);
    //    int ret = m_pFunVdmLine(1,12, strlen(tmp.toLocal8Bit().data()), tmp.toLocal8Bit().data());
    //    return ret!=-1;
    return true;
}

bool VDMArm::ClearVehInfo()
{
    if(!m_bIsOpen)
    {
        return false;
    }
    return true;
}

bool VDMArm::UpdateDateTime(const QDateTime &CurrentTime)
{
    //    LogMsg("lane", tr("字符叠加设置时间信息[%1]").arg(CurrentTime.toString("yyyy-MM-dd hh:mm:ss")));
    //    if(!m_bIsOpen)
    //    {
    //        return false;
    //    }
    //    QString sTime=CurrentTime.toString("yyyy-MM-dd hh:mm:ss");

    //    int ret=m_pFunVdmLine(2,24,sTime.length(),sTime.toLocal8Bit().data());
    //    return ret!=-1;
    return true;
}

bool VDMArm::UpdateDateTime()
{
    //    LogMsg("lane", tr("字符叠加设置时间信息[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    //    if(!m_bIsOpen)
    //    {
    //        return false;
    //    }
    //    QString sTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //    int ret = m_pFunVdmLine(0,0,sTime.length(), sTime.toLocal8Bit().data());
    //    if(ret >= 0)
    //        return true;
    //    else
    //        return false;
    bool ret = false;
    LogMsg("lane", tr("字符叠加设置时间信息[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    if(m_bDriverLoaded)
    {
        ret = pFuncSyncVdmTime() == 0;
        if(m_timerId != -1)
        {
            killTimer(m_timerId);
        }
        m_timerId = startTimer(1000);
    }

    return ret;
}

bool VDMArm::UpdateFare(int nFare)
{
    //2012年12月17日11:12:30按照祥凯要求，先空着，后续增加
    return true;
}

