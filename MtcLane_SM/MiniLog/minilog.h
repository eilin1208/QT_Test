#ifndef MINILOG_H
#define MINILOG_H
#include "MiniLog_global.h"
#include "fileoperate.h"

//写入日志内容
LOGAPI void LogMsg(const QString &msg, LogDegree degree);

LOGAPI bool InitLog(const QString &fileName);


#endif // MINILOG_H

