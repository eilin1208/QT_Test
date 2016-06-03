#include "ilogmsg.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/propertyconfigurator.h"

ILogMsg::ILogMsg(QObject *parent) : QObject(parent)
{
//    Log4Qt::BasicConfigurator::configure();
//    Log4Qt::PropertyConfigurator::configure("./log4qt.properties");
}

void ILogMsg::loadConfig(const QString &sFileName)
{
    Log4Qt::BasicConfigurator::configure();
    Log4Qt::PropertyConfigurator::configure(sFileName);
}

Log4Qt::Logger  *LogFmtMsg()
{
    return Log4Qt::Logger::logger("LaneLog");
}
