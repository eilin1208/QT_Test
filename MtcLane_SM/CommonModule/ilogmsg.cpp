#include "ilogmsg.h"

ILogMsg* g_Log = NULL;
ILogMsg::ILogMsg(QObject *parent) :
    QObject(parent)
{
}
namespace DONOTUSE
{
ILogMsg* getLogger()
{
    QMutexLocker locker(&DONOTUSE::g_Mutex);
    if(g_Log == NULL)
    {
        //设置程序实例为父对象，是否可以自动删除
        g_Log = new ILogMsg();
    }
    return g_Log;
}
}
void LogMsg(const QString &sTitle, const QString &sContent, bool bWnd)
{
    DONOTUSE::getLogger()->LogMsg(sTitle, sContent, bWnd);
}

void SetLogger(ILogMsg* log)
{
    QMutexLocker locker(&DONOTUSE::g_Mutex);
    if(g_Log != NULL)
    {
        delete g_Log;
    }
    g_Log = log;
}
