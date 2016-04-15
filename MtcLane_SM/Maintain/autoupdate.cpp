#include "autoupdate.h"
#include "maintaincfg.h"
#include <QProcess>
#include <QFile>
#include <QDebug>
AutoUpdate::AutoUpdate(QObject *parent) :
    QObject(parent)
{
    MaintainCFG::readAllcfg();
    sNewPath=MaintainCFG::getUpdateFilePath()+MaintainCFG::getUpdateFileName();
    sOldPath=MaintainCFG::getOldFilePath()+MaintainCFG::getUpdateFileName();
}
bool AutoUpdate::softWareUpdate()
{
    if(!haveUpdateFile())
        return true;
    if(!softWareBackup())
        return false;
    if(!copyNewSoftWare())
        return false;
    return true;
}
bool AutoUpdate::startSoftware()
{
    QStringList arguments;
    arguments<<"-qws";
    bool ret=QProcess::startDetached(MaintainCFG::getOldFilePath()+MaintainCFG::getUpdateFileName(),arguments);
    if(!ret)
        setErrorMessage(tr("开启程序失败"));
    return ret;
}
bool AutoUpdate::haveUpdateFile()
{
    if(sNewPath.isEmpty())
    {
        setErrorMessage(tr("新程序路径获取失败"));
        return false;
    }
    if(!QFile::exists(sNewPath))
    {
        qDebug()<<sNewPath;
        setErrorMessage(tr("没有要更新的程序"));
        return false;
    }
    return true;
}
bool AutoUpdate::softWareBackup()
{
    if(sNewPath.isEmpty())
    {
        setErrorMessage(tr("新程序路径获取失败"));
        return false;
    }
    bool ret=QFile::exists(sOldPath);
    if(!ret)
    {
        setErrorMessage(tr("查找收费软件失败"));
        return ret;
    }
    ret=QFile::exists(sOldPath+"_bak");
    if(ret)
    {
        QFile::remove(sOldPath+"_bak");
    }
    ret=QFile::rename(sOldPath,sOldPath+"_bak");
    if(!ret)
    {
        setErrorMessage(tr("现有程序备份失败"));
    }
    return ret;
}
bool AutoUpdate::copyNewSoftWare()
{
    bool ret=QFile::copy(sNewPath,sOldPath);
    if(!ret)
    {
        setErrorMessage(tr("复制新程序失败"));
        return ret;
    }
    ret=QFile::remove(sNewPath);
    if(!ret)
    {
        setErrorMessage(tr("删除新程序失败"));
        return ret;
    }
    ret=QFile::setPermissions(sOldPath,QFile::ExeOwner | QFile::permissions(sOldPath));
    if(!ret)
    {
        setErrorMessage(tr("程序执行权限设置失败"));
        return ret;
    }
    return ret;
}

QString AutoUpdate::getErrorMessage()
{
    return sErrMessage;
}

void AutoUpdate::setErrorMessage(QString sError)
{
    sErrMessage=sError;
}

bool AutoUpdate::delNewSoftWare()
{
    QStringList arguments;
    arguments<<sNewPath;
    bool ret=QProcess::startDetached("rm",arguments);
    if(!ret)
        setErrorMessage(tr("删除新程序失败"));
    return ret;
}
