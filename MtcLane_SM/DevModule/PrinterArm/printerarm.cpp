#include "printerarm.h"

PrinterArm::PrinterArm(const QString& sName, QObject *parent) :
    DevBaseInterface(sName, parent),
    m_pInit(NULL),
    m_pStop(NULL),
    m_pStart(NULL),
    m_pNote(NULL),
    m_pGetStatus(NULL),
    m_pClose(NULL)
{
    qMemSet(&m_NoteContent, 0, sizeof m_NoteContent);
}

bool PrinterArm::Print()
{
    LogMsg("lane", tr("打印机开始打印..."));
    if(!m_bDriverLoaded)
    {
        emit printFail();
        return false;
    }
    if(m_pGetStatus() != 0)
    {
        emit printFail();
        return false;
    }
    //qDebug()<<"显示结果:"<<m_NoteContent.LimitW;
    bool ret = m_pNote(m_NoteContent) == 1;
    if(ret)
    {
        emit printSuccess();
    }
    else
    {
        emit printFail();
    }
    LogMsg("lane", tr("打印机打印结果为[%1]").arg(ret));
    return ret;
}

bool PrinterArm::RePrint()
{
    LogMsg("lane", tr("打印机重新打印..."));
    if(!m_bDriverLoaded)
    {
        return false;
    }
    if(m_pGetStatus() != 0)
    {
        return false;
    }
    bool ret = m_pNote(m_NoteContent) == 1;
    if(ret)
    {
        emit printSuccess();
    }
    else
    {
        emit printFail();
    }
    LogMsg("lane", tr("打印机重打结果为[%1]").arg(ret));

    return ret;
}

bool PrinterArm::PrintClose()
{
    LogMsg("lane", tr("打印机关闭..."));
    if(!m_bDriverLoaded)
    {
        return false;
    }
    return m_pClose();
}

int PrinterArm::GetPrintStatus()
{
    if(!m_bDriverLoaded)
    {
        return -1;
    }
    m_nStatus = m_pGetStatus();
    LogMsg("lane", tr("获取打印机状态,状态为[%1]").arg(m_nStatus));
    switch(m_nStatus)
    {
    case 0:
        emit StatusChanged(DEV_STATUS_OK);
        break;
    case -1:
        emit StatusChanged(DEV_STATUS_Offline);
        break;
    case -2:
    case -3:
        emit StatusChanged(DEV_STATUS_CommErr);
        break;
    default:
        emit StatusChanged(DEV_STATUS_Offline);
        break;
    }

    return m_nStatus;
}

bool PrinterArm::LoadFunction()
{
    //QDebug() << m_sLibName;
    m_pInit = (Func_PrtInit)m_qLibrary.resolve("PrtInit");
    m_pStop = (Func_PrtStop)m_qLibrary.resolve("PrtStart");
    m_pStart = (Func_PrtStart)m_qLibrary.resolve("PrtStart");
    m_pNote = (Func_PrtNote)m_qLibrary.resolve("PrtNote");
    m_pGetStatus = (Func_PrtGetStatus)m_qLibrary.resolve("PrtGetStatus");
    m_pClose = (Func_PrtClose)m_qLibrary.resolve("PrtClose");
    if(m_pInit == NULL||
            m_pStop == NULL ||
            m_pStart == NULL ||
            m_pNote == NULL ||
            m_pGetStatus == NULL ||
            m_pClose == NULL)
    {
        m_sErrString = m_qLibrary.errorString();
        return false;
    }
    return true;
}

bool PrinterArm::ReleaseFunction()
{
    m_pInit = NULL;
    m_pStart = NULL;
    m_pStop = NULL;
    m_pNote = NULL;
    m_pGetStatus = NULL;
    m_pClose = NULL;
    return true;
}

bool PrinterArm::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    //PrintClose();
    return PrintInit();
}

bool PrinterArm::Close()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    return PrintClose();
}

bool PrinterArm::PrintStop()
{
    LogMsg("lane", tr("打印机停止..."));
    if(!m_bDriverLoaded)
    {
        return false;
    }
    return m_pStop();
}

bool PrinterArm::PrintStart()
{
    LogMsg("lane", tr("打印开始..."));
    if(!m_bDriverLoaded)
    {
        return false;
    }
    return m_pStart();
}

bool PrinterArm::PrintInit()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    bool ok = false;
    ok = m_pInit(m_sConStr1.toLocal8Bit().data(), m_sConStr2.toLocal8Bit().data(), m_sConStr2.isEmpty());
    LogMsg("lane", tr("打印机开始初始化,结果为[%1],ConnStr1[%2],ConnStr2[%3]").arg(ok).arg(m_sConStr1).arg(m_sConStr2));
    if(!ok)
    {
        return false;
    }
    m_nStatus = m_pGetStatus() == 0? 0 : 4;
    return m_nStatus == 0;
}
