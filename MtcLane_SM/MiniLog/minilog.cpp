#include "minilog.h"

//#include <QVector>
//QVector<FileOperate*> m_FileVector;
bool m_bInit = false;
//根据等级决定数据显示方式
void LogMsg(const QString &msg, LogDegree degree)
{
//    for(int i = 0; i != m_FileVector.size();++i)
//    {
//        if(m_FileVector.at(i)->GetFileName().contains(filename))
//        {
//            m_FileVector.at(i)->LogMsg(filename,msg,degree);
//            return;
//        }
//    }
    if(!m_bInit)
    {
        getLog()->SetLogDegree(DEBUG);
        getLog()->LogMsg(QObject::tr("打开日志文件失败"));
        return;
    }
    getLog()->SetLogDegree(degree);
    getLog()->LogMsg(msg);
}

bool InitLog(const QString &fileName)
{
//    FileOperate* oper = new FileOperate;
//    QString str = QObject::tr("%1.log").arg(filename);
//    oper->SetFileName(str);
//    m_FileVector.append(oper);
    getLog()->SetFileName(fileName);
    m_bInit = getLog()->OpenLog();
    return m_bInit;
}
