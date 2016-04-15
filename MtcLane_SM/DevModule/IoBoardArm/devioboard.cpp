#include "devioboard.h"
#include <QLibrary>
#include <QSettings>
IO_Close LibIoClose = NULL;
IO_SetAlarm LibIoSetAlarm = NULL;
IO_SetCanopy LibIoSetCanopy = NULL;
IO_SetBar LibIoSetBar = NULL;
IO_SetFogLamp LibIoSetFogLamp = NULL;
IO_SetLaneLamp LibIoSetLaneLamp = NULL;
IO_SetOut2 LibIoSetOut2 = NULL;
IO_Init LibIoInit = NULL;
IOBoardArm *w ;
int g_nInputArray[] = {2,3,0,1,10,10,10};
const int INPUTNUM = 7;
void CallBack(unsigned int status)
{
    LogMsg("lane", QObject::tr("IO控制输入发生变化,输入值为[%1]").arg(status));
    QCoreApplication::postEvent(w, new IOEvent(status));
}

IOBoardArm::IOBoardArm(const QString& sName, QObject* parent):
    DevBaseInterface(sName, parent)
{
    m_nLastState.data = 0;
    m_bBarLocked = false;
    m_nDIStatus = 0;
    m_nDOStatus = 0;
    m_nAlarmTimer = -1;
    w = this;
    qRegisterMetaType<DIGITAL_IN>("DIGITAL_IN");
    m_nBarControlTime=1000;
    setBarMsecond();
    m_ncurTimeMF = 0;
    m_ncurTimeMS = 0;
}

void IOBoardArm::setBarMsecond()
{
    QSettings settings("./Lane.ini", QSettings::IniFormat);
    m_nBarControlTime = settings.value("BarControl/MSecond").toInt();
}

void IOBoardArm::LockBar()
{
    LogMsg("lane", tr("IO控制锁杆..."));
    m_bBarLocked = true;

}

void IOBoardArm::UnLockBar()
{
    LogMsg("lane", tr("IO控制栏杆解锁..."));
    m_bBarLocked = false;
}

bool IOBoardArm::IsLockBar()
{
    return m_bBarLocked;
}

void IOBoardArm::SimBackLoop()
{
    if(!IsLockBar())
    {
        emit InputChanged(DI_LoopBack, 0);
    }
}

void IOBoardArm::SetDOStatus(DIGITAL_OUT devBit, bool status)
{
    int tmp = 0x1;
    tmp = tmp << devBit;
    if(status)
    {
        m_nDOStatus = m_nDOStatus | tmp;
    }
    else
    {
        tmp = ~tmp;
        m_nDOStatus = m_nDOStatus & tmp;
    }
    emit OutputChanged(devBit, status);
}

bool IOBoardArm::loadIOCardIni()
{
    bool result = false;
    qDebug() << "IOCard" << m_sConStr1;
    if(QFile::exists(m_sConStr1))
    {
        qDebug() << "io card ini is exists";
        QSettings settings(m_sConStr1, QSettings::IniFormat);
        m_inputIndex[settings.value("Input/LOOP1HASCAR", 0).toInt()] = LOOPTrig1CAR;
        m_inputIndex[settings.value("Input/LOOP1NOCAR", 0).toInt()] = LOOPTrig1NOCAR;
        m_inputIndex[settings.value("Input/LOOP2HASCAR", 0).toInt()] = LOOPTrig2CAR;
        m_inputIndex[settings.value("Input/LOOP2NOCAR", 0).toInt()] = LOOPTrig2NOCAR;
        m_inputIndex[settings.value("Input/CAPLOOPHASCAR", 0).toInt()] = FLOOPCAR;
        m_inputIndex[settings.value("Input/CAPLOOPNOCAR", 0).toInt()] = FLOOPNOCAR;
        m_inputIndex[settings.value("Input/PASSLOOPHASCAR", 0).toInt()] = BLOOPCAR;
        m_inputIndex[settings.value("Input/PASSLOOPNOCAR", 0).toInt()] = BLOOPNOCAR;
        m_inputIndex[settings.value("Input/BARUP", 0).toInt()] = BARUP;
        m_inputIndex[settings.value("Input/BARDOWN", 0).toInt()] = BARDOWN;
        m_inputIndex[settings.value("Input/BARUPING", 0).toInt()] = BARUPING;
        m_inputIndex[settings.value("Input/BARDOWNING", 0).toInt()] = BARDOWNING;
        qDebug() << "io card map is" << m_inputIndex.count();
        result = true;
    }
    return result;
}

bool IOBoardArm::LoadFunction()
{
    LibIoClose = (IO_Close)m_qLibrary.resolve("IO_Close");
    LibIoSetAlarm = (IO_SetAlarm)m_qLibrary.resolve("IO_SetAlarm");
    LibIoSetCanopy = (IO_SetCanopy)m_qLibrary.resolve( "IO_SetCanopy");
    LibIoSetBar = (IO_SetBar)m_qLibrary.resolve( "IO_SetBar");
    LibIoSetFogLamp = (IO_SetFogLamp)m_qLibrary.resolve( "IO_SetFogLamp");
    LibIoSetLaneLamp = (IO_SetLaneLamp)m_qLibrary.resolve("IO_SetLaneLamp");
    LibIoSetOut2 = (IO_SetOut2)m_qLibrary.resolve("IO_SetOut2");
    LibIoInit = (IO_Init)m_qLibrary.resolve("IO_Init");
    if(LibIoClose && LibIoSetAlarm && LibIoSetCanopy && LibIoSetBar && LibIoSetFogLamp && LibIoSetLaneLamp
            && LibIoSetOut2 && LibIoInit)
    {
        return true;
    }
    else
    {
        m_sErrString = m_qLibrary.errorString();
        return false;
    }
}


void IOBoardArm::SetAlarm(int var)
{
    LogMsg("lane", tr("IO控制声光报警[%1]").arg(var));
    if(m_bInited)
    {
        LibIoSetAlarm(var);
        SetDOStatus(DO_AlarmLight, var);
        SetDOStatus(DO_AlarmSound, var);
        if(var == 1)
        {
            if(m_nAlarmTimer != -1)
            {
                killTimer(m_nAlarmTimer);
            }
            m_nAlarmTimer = startTimer(5000);
        }
    }
}

void IOBoardArm::SetCanopy(int var)
{
    LogMsg("lane", tr("IO控制雨棚灯[%1]").arg(var));

    if(m_bInited)
    {
        LibIoSetCanopy(var);
        SetDOStatus(DO_CanopyLightGreen, var);
        SetDOStatus(DO_CanopyLightRed, !var);
    }
}

void IOBoardArm::SetBar(int var)
{
    LogMsg("lane", tr("IO控制雨栏杆[%1]").arg(var));

    if(m_bInited)
    {
        LibIoSetBar(var);
        SetDOStatus(DO_BarrierUp, var);
        SetDOStatus(DO_BarrierDown, !var);
    }
}

void IOBoardArm::SetFogLamp(int var)
{
    LogMsg("lane", tr("IO控制雾灯[%1]").arg(var));
    if(m_bInited)
    {
        LibIoSetFogLamp(var);
    }
}

void IOBoardArm::SetLaneLamp(int var)
{
    LogMsg("lane", tr("IO控制通行灯[%1]").arg(var));

    if(m_bInited)
    {
        LibIoSetLaneLamp(var);
        SetDOStatus(DO_PassLightGreen, var);
        SetDOStatus(DO_PassLightRed, !var);
    }
}

void IOBoardArm::SetOut2(int var)
{
    LogMsg("lane", tr("IO控制备用输出[%1]").arg(var));

    if(m_bInited)
    {
        LibIoSetOut2(var);
    }
}

void IOBoardArm::SetOneVehPass()
{
    LogMsg("lane", tr("IO控制允许当前车辆通行，抬起栏杆，通行灯变绿"));
    SetBar(1);
    SetLaneLamp(1);
}

void IOBoardArm::SetOneVehHasPassed()
{
    LogMsg("lane", tr("IO控制当前车辆已经通过，非锁杆状态下落下栏杆，通行灯变红"));
    if(!m_bBarLocked)
    {
        SetBar(0);
    }
    SetLaneLamp(0);
}

bool IOBoardArm::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    if(m_bInited)
    {
        return true;
    }
    int nRet = LibIoInit(CallBack, 0);
    LogMsg("lane", tr("IO控制初始化完成,初始化结果为[%1][%2]").arg(nRet).arg(nRet == 0));
    //加载IOCard.ini文件
    if(!loadIOCardIni())
    {
        return false;
    }
    if(nRet == 0)
    {
        m_bInited = true;
        //落杆
        SetBar(0);
        return true;
    }
    else
    {
        return false;
    }
}

void IOBoardArm::customEvent(QEvent *event)
{
    if(event->type() == QEvent::User + 1)
    {
        IOEvent* ioEvent = static_cast<IOEvent*>(event);
        qDebug() << "IO input changed, native value is" << ioEvent->m_InputStatus;
        for(int i = 0; i < 32; i++)
        {
            if(ioEvent->m_InputStatus & (1 << i))
            {
                qDebug() << "IO input map ini file is" << i;
                switch(m_inputIndex.value(i, -1))
                {
                case LOOPTrig1CAR:
                    //qDebug()<<"LOOPTrig1CAR"<<1;
                    emit InputChanged(DI_LoopTrig1,1);
                    SetDIPortStatus(DI_LoopTrig1, 1);
                    break;
                case LOOPTrig1NOCAR:
                    //qDebug()<<"LOOPTrig1NOCAR"<<0;
                    emit InputChanged(DI_LoopTrig1,0);
                    SetDIPortStatus(DI_LoopTrig1,0);
                    break;
                case LOOPTrig2CAR:
                    //qDebug()<<"LOOPTrig2CAR"<<1;
                    emit InputChanged(DI_LoopTrig2,1);
                    SetDIPortStatus(DI_LoopTrig2,1);
                    break;
                case LOOPTrig2NOCAR:
                    //qDebug()<<"LOOPTrig2NOCAR"<<0;
                    emit InputChanged(DI_LoopTrig2,0);
                    SetDIPortStatus(DI_LoopTrig2,0);
                    break;
                case FLOOPCAR:
                    //qDebug()<<"FLOOPCAR"<<1;
                    emit InputChanged(DI_LoopFront, 1);
                    SetDIPortStatus(DI_LoopFront, 1);
                    break;
                case FLOOPNOCAR:
                    //qDebug()<<"FLOOPNOCAR"<<0;
                    m_ncurTimeMF = QDateTime::currentDateTime().toMSecsSinceEpoch();
                    emit InputChanged(DI_LoopFront, 0);
                    SetDIPortStatus(DI_LoopFront, 0);
                    break;
                case BLOOPCAR:
                    //qDebug()<<"BLOOPCAR"<<1;
                    emit InputChanged(DI_LoopBack, 1);
                    SetDIPortStatus(DI_LoopBack, 1);
                    break;
                case BLOOPNOCAR:
                    //qDebug()<<"BLOOPNOCAR"<<0;
                    m_ncurTimeMS = QDateTime::currentDateTime().toMSecsSinceEpoch();
                    if(m_ncurTimeMS - m_ncurTimeMF > m_nBarControlTime)
                    {
                        emit InputChanged(DI_LoopBack, 0);
                        SetDIPortStatus(DI_LoopBack, 0);
                    }
                    break;
                case BARUP:
                    emit InputChanged(DI_BarrierUp, 1);
                    SetDIPortStatus(DI_BarrierUp, 1);
                    break;
                case BARDOWN:
                    emit InputChanged(DI_BarrierDown, 1);
                    SetDIPortStatus(DI_BarrierDown, 1);
                    break;
                default:
                    qDebug() << "IOCard.ini file is invalid, please configure it";
                    break;
                }
            }
        }
    }
}

void IOBoardArm::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nAlarmTimer)
    {
        SetAlarm(0);
        m_nAlarmTimer = -1;
    }
}

bool IOBoardArm::Close()
{
    LogMsg("lane", tr("IO控制关闭..."));
    if(m_bDriverLoaded)
    {
        SetBar(1);
        LibIoClose();
    }
    return true;
}

bool IOBoardArm::ReleaseFunction()
{
    LibIoClose = NULL;
    LibIoSetAlarm = NULL;
    LibIoSetCanopy = NULL;
    LibIoSetBar = NULL;
    LibIoSetFogLamp = NULL;
    LibIoSetLaneLamp = NULL;
    LibIoSetOut2 = NULL;
    LibIoInit = NULL;
    return true;
}

bool IOBoardArm::GetDOPortStatus(DIGITAL_OUT devBit)
{
    return (m_nDOStatus >> devBit) & 0x1;
}

bool IOBoardArm::GetDIPortStatus(DIGITAL_IN devBit)
{
    return (m_nDIStatus >> devBit) & 0x1;
}

void IOBoardArm::SimBackLoopCar()
{
    emit InputChanged(DI_LoopBack, 1);
}

bool IOBoardArm::SetDIPortStatus(DIGITAL_IN devBit, quint8 status)
{
    if(status == 1)
    {
        m_nDIStatus = m_nDIStatus | (1 << devBit);
    }
    if(status == 0)
    {
        m_nDIStatus = m_nDIStatus & (~(1 << devBit));
    }
}
