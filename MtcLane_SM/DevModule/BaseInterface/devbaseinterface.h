#ifndef DEVBASEINTERFACE_H
#define DEVBASEINTERFACE_H

#include <QThread>
#include <QLibrary>
#include "../CommonModule/ilogmsg.h"
const int DEV_STATUS_OK = 0;
const int DEV_STATUS_ParamErr = 1;		//	参数错误
const int DEV_STATUS_LibErr   = 2;			//	动态库错误
const int DEV_STATUS_CommErr  = 3;			//	通讯失败
const int DEV_STATUS_Offline  = 4;		//	脱机


class DevBaseInterface : public QThread
{
    Q_OBJECT
public:
    explicit DevBaseInterface(const QString & sDevName, QObject *parent = 0);

signals:
    void StatusChanged(int nNewStatus);
public slots:

public:
    //QLibrary getm_libary();//取得文件
    //打开设备
    virtual bool StartDev();
    //关闭设备
    virtual bool StopDev();
    //读取INI文件
    virtual void readCfgInfo();
    //加载驱动
    virtual bool LoadDriver();
public:
    virtual int GetStatus()
    {
        return m_nStatus;
    }
    bool isOpen()
    {
        return m_bIsOpen;
    }
    bool isClose()
    {
        return m_bIsClose;
    }
    void sendSingle(int status)
    {
        emit StatusChanged(status);
    }
protected:
    //加载驱动
    virtual bool LoadFunction() = 0;
    //卸载驱动
    virtual bool ReleaseFunction() = 0;
    //关闭
    virtual bool Close() = 0;
    //初始化
    virtual bool Init() = 0;
protected:
    //Lane.ini文件中信息
    QString m_sLibName;
    QString m_sConStr1;
    QString m_sConStr2;
    QString m_sErrString;
    int m_nErrCode;
    bool m_bDriverLoaded;
    bool m_bInited;
    QLibrary m_qLibrary;
    int m_nStatus;
    bool m_bIsOpen;
    bool m_bIsClose;
};

#endif // DEVBASEINTERFACE_H
