#include "minilog.h"
#include <QFile>
#include <QMutex>
#include <QDateTime>
namespace mini
{
QFile g_file;
QMutex g_mutex;
bool g_init = false;
QString g_fileName;

void SetLogName(const QString &fileName)
{
    g_fileName = "./" + fileName;
    if(!g_file.isOpen())
    {
        g_file.setFileName(g_fileName);
    }
}

bool init()
{
    if(g_fileName.isEmpty())
    {
        SetLogName(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + ".log");
    }
    g_init = g_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    return g_init;
}

void LogMsg(const QString &log)
{
    bool ret;
    if(!g_init)
    {
        ret = init();
    }
    if(!ret)
    {
        return;
    }
    g_file.write(QObject::tr("%1 %2\n")
                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                 .arg(log).toLocal8Bit().data());
    if(g_file.size() > 1024 * 1024)
    {
        g_file.close();
        g_file.remove();
        g_init = false;
    }
}
}

