#include "fileoperate.h"
#include <QDebug>
#include <QDir>
#include <QTextCodec>
#include <QDateTime>
#include <QSettings>
//转换编码方式
QString UTF8toGB2312(QString str, int length = -1)
{
    QByteArray temp;
    temp = QTextCodec::codecForName("GB2312")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str.toLocal8Bit()));
    return QString::fromLocal8Bit(temp.constData());
}
FileOperate::FileOperate(QObject *parent) :
    QObject(parent)
{
    //此为定义初值，当无法读取配置文件时使用以下数据
    m_degree = BOTH;
    m_LogNum = 5;
    m_LogSize = 2;
}

bool FileOperate::OpenLog()
{
    bool ret;
    ReadSettings();
    m_File.setFileName(m_LogName);
    ret = m_File.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    return ret;
}

void FileOperate::LogInFile(const QString &msg)
{
    if(!m_File.isOpen())
    {
        if(!OpenLog())
        {
            qDebug()<<QObject::tr("%1日志创建或打开失败").arg(m_LogName);
            return;
        }
    }
    RenameLog();
    QString slogger = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(msg);
    QString sFinal = UTF8toGB2312(slogger);
    m_File.write(sFinal.toLocal8Bit());
    m_File.flush();
}

QStringList FileOperate::GetLogFileList()
{
    QDir tempDir;
    tempDir.setPath(QDir::currentPath());
    QStringList strlist;
    strlist<<tr("%1.*.log").arg(m_bakName);
    return tempDir.entryList(strlist,QDir::Files,QDir::Name);
}

bool FileOperate::DeleteLog()
{
    qDebug()<<"fengSize<<--"<<GetLogFileList().size();
    if(m_LogNum > GetLogFileList().size())
    {
        return false;
    }
    QString tempStr;
    bool ret;
    tempStr = QObject::tr("%1.1.log").arg(m_bakName);
    ret= QFile::remove(tempStr);
    return ret;
}

void FileOperate::SetFileName(const QString &filename)
{
    m_bakName = filename;
    m_LogName = tr("%1.log").arg(filename);
}
//从配置文件读取配置项
void FileOperate::ReadSettings()
{
    QSettings Settings("./MiniLog.ini",QSettings::IniFormat);
    QString groupName("Settings");
    m_LogNum = Settings.value("/LogNum",5).toInt();
    m_LogSize = Settings.value("/LogSize",2).toInt();
    m_degree = (LogDegree)Settings.value("/LogDegree",0).toInt();

}
//获取文件最大大小
int FileOperate::GetLogMaxSize()
{
    return m_LogSize*1024*1024;
}
//
void FileOperate::LogMsg(const QString &msg)
{
    switch(m_degree)
    {
    case BOTH:
    {
        LogInFile(msg);
        qDebug()<<QObject::tr("%1--%2").arg(m_bakName).arg(msg);
        break;
    }
    case DEBUG:
    {
        qDebug()<<QObject::tr("%1--%2").arg(m_bakName).arg(msg);
        break;
    }
    case LOG:
    {
        LogInFile(msg);
        break;
    }
    default:
        break;
    }
}

QString FileOperate::GetFileName()
{
    return m_LogName;
}

void FileOperate::RenameLog()
{
    bool ret;
    if(m_File.size() > GetLogMaxSize())
    {
        if(DeleteLog())
        {
            RenameLogList(GetLogFileList());
        }
        if(m_File.isOpen())
        {
            m_File.close();
        }
        m_File.rename(QObject::tr("%1.%2.log").arg(m_bakName).arg(QString::number(GetLogFileList().size()+1)));
        //文件名中如Lane.log1中1是否正确
        m_File.setFileName(m_LogName);
    }
    if(!m_File.isOpen())
    {
        ret = m_File.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
}
FileOperate* getLog()
{
    static FileOperate oper;
    return &oper;
}

void FileOperate::SetLogDegree(LogDegree degree)
{
    m_degree = degree;
}

void FileOperate::RenameLogList(const QStringList &LogList)
{
    for(int i = 2;(i-2) != LogList.size(); ++i )
    {
        QString temp = QString::number(i);
        for(int j = 0; j != LogList.size(); ++j)
        {
            if(LogList.at(j).contains(temp))
            {
                temp = QObject::tr("%1.%2.log").arg(m_bakName).arg(QString::number(i-1));
                QFile::rename(LogList.at(j),temp);
                break;
            }
        }
    }
}
