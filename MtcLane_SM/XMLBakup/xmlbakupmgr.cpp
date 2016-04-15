#include "xmlbakupmgr.h"
#include <QDateTime>
#include <QSettings>
#include "CommonModule/ilogmsg.h"
XMLBakupMgr::XMLBakupMgr(QObject *parent) :
    QObject(parent)
{
    connect(&m_xmlThread, SIGNAL(finished()), this, SLOT(bakupFinish()));
}

XMLBakupMgr::~XMLBakupMgr()
{
    //如果线程没有关闭，则等待线程关闭
    if(m_xmlThread.isRunning())
    {
        m_xmlThread.quit();
        m_xmlThread.wait();
    }
}

void XMLBakupMgr::bakupFinish()
{
    //再次启动定时器,每一小时检查一次
    startTimer(3600000);
    //startTimer(2000);
}

void XMLBakupMgr::init()
{
    LogMsg("lane", tr("XML备份开始..."));
    startTimer(1000);
}

void XMLBakupMgr::timerEvent(QTimerEvent *event)
{
    //首先停止定时器
    killTimer(event->timerId());
    if(QDateTime::currentDateTime().time().hour() == getBakUpHour())
    //启动线程进行备份
    {
        LogMsg("lane", tr("到达备份时间，开始进行XML备份文件转移"));
        m_xmlThread.start();
    }
    else
    {
        startTimer(3600000);
        //startTimer(10000);
    }
}

int XMLBakupMgr::getBakUpHour()
{
    QSettings setting("./Lane.ini", QSettings::IniFormat);
    return setting.value("Other/BakUpHour", 4).toInt();
}
