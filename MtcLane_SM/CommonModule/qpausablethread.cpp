#include "qpausablethread.h"
#include "ilogmsg.h"
QPausableThread::QPausableThread(QObject *parent) :
    QThread(parent)
{
    m_bStop = false;

    //允许继续执行
    m_bContinue = true;
}

QPausableThread::~QPausableThread()
{
    if(this->isRunning())
    {
        Stop();
    }
}

void QPausableThread::run()
{
    forever
    {
        m_continueMutex.lock();
        if(m_bStop)
        {
            m_bStop = false;
            break;
        }

        RunOnce();
        if(!m_bContinue)
        {
            LogMsg("mon", tr("%1准备Wait...").arg(objectName()));
            m_waitCondition.wait(&m_continueMutex);
        }
        m_continueMutex.unlock();
        if(m_bStop)
        {
            m_bStop = false;
            break;
        }
    }

}

void QPausableThread::RunOnce()
{
}

void QPausableThread::Pause()
{
    m_bContinue = false;
}

void QPausableThread::Resume()
{
    //线程没有启动则启动一次
    LogMsg("lane", tr("可中断线程%1恢复运行...").arg(objectName()));
    if(!this->isRunning())
    {
        this->start();
        LogMsg("mon", tr("%1启动...").arg(objectName()));
    }
    else
    {
        LogMsg("mon", tr("%1恢复运行...").arg(objectName()));
        m_bContinue = true;
        LogMsg("mon", tr("%1唤醒...").arg(objectName()));
        m_waitCondition.wakeAll();
    }
}

void QPausableThread::Stop()
{
    LogMsg("lane", tr("可中断线程%1正在停止...").arg(this->objectName()));
    m_bStop = true;
    m_waitCondition.wakeAll();
    m_bContinue = true;

    //等待线程结束
    this->exit();
    this->wait();
    LogMsg("lane", tr("可中断线程%1停止完成...").arg(this->objectName()));
}

void QPausableThread::CheckExit(unsigned long time)
{
    m_stopMutex.lock();
    m_waitCondition.wait(&m_stopMutex, time);
    m_stopMutex.unlock();
}
