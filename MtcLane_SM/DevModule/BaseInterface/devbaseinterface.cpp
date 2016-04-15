#include "devbaseinterface.h"
#include <QSettings>
#include <QFile>
#include <QStringList>
#include <QDebug>
DevBaseInterface::DevBaseInterface(const QString & sDevName, QObject *parent) :
    QThread(parent)
{
    setObjectName(sDevName);
    m_bDriverLoaded = false;
    m_bInited = false;
    m_nStatus = DEV_STATUS_Offline;
    m_bIsOpen = false;
    m_nErrCode = 0;
}

void DevBaseInterface::readCfgInfo()
{
    QString sName = objectName();
    QSettings settings("./Lane.ini", QSettings::IniFormat);
    m_sLibName = settings.value(sName + "/Driver").toString();
    m_sConStr1 = settings.value(sName + "/ConnStr1").toString();
    QStringList tmp = settings.value(sName + "/ConnStr2").toStringList();
    foreach(QString item, tmp)
    {
        m_sConStr2.append(item);
        m_sConStr2.append(",");
    }
    m_sConStr2.remove(m_sConStr2.length() - 1, 1);
}

bool DevBaseInterface::StartDev()
{
    //防止重复加载
    if(m_bInited)
    {
        return true;
    }

    //读取配置文件
    readCfgInfo();

    //加载类库
    if(!LoadDriver())
    {
        LogMsg("lane", tr("%1设备打开失败，加载类库失败,errcode=[%2],driver=[%3],connstr1=[%4],connstr2=[%5],errstring=[%6]")
               .arg(objectName())
               .arg(m_nErrCode)
               .arg(m_sLibName)
               .arg(m_sConStr1)
               .arg(m_sConStr2)
               .arg(m_sErrString)
               );
        m_nStatus = DEV_STATUS_LibErr;
        emit StatusChanged(m_nStatus);
        return false;
    }
    m_bDriverLoaded = true;
    //初始化
    if(!Init())
    {
        LogMsg("lane", tr("%1设备打开失败，初始化失败,errcode=[%2],driver=[%3],connstr1=[%4],connstr2=[%5],errstring=[%6]")
               .arg(objectName())
               .arg(m_nErrCode)
               .arg(m_sLibName)
               .arg(m_sConStr1)
               .arg(m_sConStr2)
               .arg(m_sErrString)
               );
        m_nStatus = DEV_STATUS_Offline;
        emit StatusChanged(m_nStatus);
        return false;
    }

    LogMsg("lane", tr("%1设备初始化成功,m_sLibName = %2,connstr1 = %3,m_sConStr2 = %4")
           .arg(objectName())
           .arg(m_sLibName)
           .arg(m_sConStr1)
           .arg(m_sConStr2));
    m_nStatus = DEV_STATUS_OK;
    emit StatusChanged(m_nStatus);
    ////qDebug()<<"emit status change m_nStatus:"<<m_nStatus;
    //m_bInited = true;
    m_bIsClose =false;
    m_bIsOpen = true;
    return true;
}

bool DevBaseInterface::StopDev()
{
    m_bIsClose =true;
    m_bInited = false;
    if(!Close())
    {
        return false;
    }
    if(!ReleaseFunction())
    {
        return false;
    }
    else
    {
        m_bDriverLoaded = false;
        return true;
    }
}

bool DevBaseInterface::LoadDriver()
{
    if(m_bDriverLoaded)
    {
        return true;
    }
    m_qLibrary.setFileName(m_sLibName);
    bool ret = LoadFunction();
    return ret;
}

