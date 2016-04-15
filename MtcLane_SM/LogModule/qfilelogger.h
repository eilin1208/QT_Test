#ifndef QFILELOGGER_H
#define QFILELOGGER_H

#include <QObject>
#include "log4qt/logger.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/rollingfileappender.h"
#include "log4qt/appender.h"
#include "CommonModule/ilogmsg.h"
class QFileLogger:public ILogMsg
{
    Q_OBJECT
public:
    enum LOG_LEVEL
    {
        LOG_ERROR,
        LOG_INFO,
        LOG_DEBUG,
        LOG_WARN
    };
    QFileLogger();
signals:
    void showLog(const QString& sLog);
public:
    virtual void LogMsg(const QString& sTitle, const QString &sContent, bool bWnd = false);
    //
    void WriteLog(const QString & title, const QString & content, LOG_LEVEL level=LOG_INFO);

    //初始化日志模块
    static void InitLog();

    // add a file logger
    // param: file name
    static void AddFileLogger(const QString& sFileName);
    //
    static void FinalizeLog();

    static void SetMaxFileSize(QString sMaxFileSize)
    {
        m_sMaxFileSize = sMaxFileSize;
    }

    static void SetBackupIndex(int nBackupIndex)
    {
        m_nBackupIndex = nBackupIndex;
    }
private:
    static bool isInited;                       //
    static QString m_strConfigFilePath;         //
    static QString m_sMaxFileSize;
    static int m_nBackupIndex;
};


#endif // QFILELOGGER_H
