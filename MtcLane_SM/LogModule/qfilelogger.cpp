#include "qfilelogger.h"
#include "log4qt/patternlayout.h"
#include "log4qt/varia/stringmatchfilter.h"
#include <QtGui/QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QProcess>
bool QFileLogger::isInited = false;
QString QFileLogger::m_sMaxFileSize = "1000KB";
int QFileLogger::m_nBackupIndex = 5;
QFileLogger::QFileLogger()
{
}

void QFileLogger::FinalizeLog()
{
    Log4Qt::Logger * tempRootLogger = Log4Qt::Logger::rootLogger();
    tempRootLogger->removeAllAppenders();
}

void QFileLogger::WriteLog(const QString & title, const QString & content, LOG_LEVEL level/*=LOG_INFO*/)
{
    if(isInited == false)
    {
        InitLog();
        isInited = true;
    }
    switch(level)
    {
    case LOG_ERROR:
        Log4Qt::Logger::logger(title)->error(title + ":" + content);
        break;
    case LOG_INFO:
        Log4Qt::Logger::logger(title)->info(title + ":" + content);
        break;
    case LOG_WARN:
        Log4Qt::Logger::logger(title)->warn(title + ":" + content);
        break;
    case LOG_DEBUG:
        Log4Qt::Logger::logger(title)->debug(title + ":" + content);
        break;
    default:
        break;
    }

}

void QFileLogger::InitLog()
{
    Log4Qt::Logger::rootLogger()->removeAllAppenders();
    AddFileLogger("lane");
    AddFileLogger("net");
    AddFileLogger("key");
    AddFileLogger("mon");
    //AddFileLogger("RSU");
}

void QFileLogger::AddFileLogger(const QString &sFileName)
{
    Log4Qt::RollingFileAppender * rollingAppender = new Log4Qt::RollingFileAppender;
    rollingAppender->setName(sFileName);
    rollingAppender->setFile(sFileName + ".log");
    rollingAppender->setAppendFile(true);
    rollingAppender->setMaxFileSize(m_sMaxFileSize);
    rollingAppender->setMaxBackupIndex(m_nBackupIndex);
    rollingAppender->setEncoding(QTextCodec::codecForName("UTF8"));
    Log4Qt::PatternLayout * layout = new Log4Qt::PatternLayout;
    layout->setConversionPattern("%d{yyyy-MM-dd HH:mm:ss} [%t] [%c] [%p] - %m%n");
    rollingAppender->setLayout(layout);

    Log4Qt::StringMatchFilter *stringFilter = new Log4Qt::StringMatchFilter;
    stringFilter->setStringToMatch(sFileName);
    stringFilter->setAcceptOnMatch(true);

    rollingAppender->addFilter(stringFilter);
    rollingAppender->activateOptions();
    Log4Qt::Logger::rootLogger()->addAppender(rollingAppender);
}

void QFileLogger::LogMsg(const QString &sTitle, const QString &sContent, bool bWnd)
{
    if(!sTitle.isEmpty())
    {
        WriteLog(sTitle,sContent);
    }
    if(bWnd)
    {
        emit showLog(sContent);
    }
}

