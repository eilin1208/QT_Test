#ifndef DEVIOBOARD_H
#define DEVIOBOARD_H
#include <QObject>
#include <QEvent>
#include "BaseInterface/devbaseinterface.h"
#include "IOBoardDef.h"


void CallBack(unsigned int status);
class IOBoardArm: public DevBaseInterface
{
    Q_OBJECT
public:
    explicit IOBoardArm(const QString& sName, QObject* parent =NULL);
    void setBarMsecond();
protected:
    //加载驱动
    virtual bool LoadFunction();
    //卸载驱动
    virtual bool ReleaseFunction();
    //关闭
    virtual bool Close();
    //初始化
    virtual bool Init();
    void customEvent(QEvent *event);
    void timerEvent(QTimerEvent *event);
signals:
    //后线圈触发，车辆过车
    void InputChanged(quint32 nDig,quint8 nStatus);
    void OutputChanged(int nDev, int nValue);
public:
    bool GetDIPortStatus(DIGITAL_IN devBit);
    bool GetDOPortStatus(DIGITAL_OUT devBit);
    bool SetDIPortStatus(DIGITAL_IN devBit, quint8 status);
    /******************************************
    IO_SetAlarm
    设置报警器状态，通过参数控制
    参数：[0 停止][1 报警]
    ******************************************/
     void SetAlarm(int var);

    /******************************************
    IO_SetCanopy
    设置天棚灯状态，通过参数控制
    参数：[0 红灯][1 绿灯][2 红绿一起亮] [3 红绿全部灭]
    ******************************************/
    void SetCanopy(int var);


    /******************************************
    IO_SetBar
    设置栏杆状态，通过参数控制
    参数：[0 落下][1 抬起]
    ******************************************/
    void SetBar(int var);

    /******************************************
    IO_SetFogLamp
    设置防雾灯，通过参数控制
    参数：[0 熄灭][1 亮起]
    ******************************************/
    void SetFogLamp(int var);

    /******************************************
    IO_SetLaneLamp
    设置车道通行灯状态，通过参数控制
    参数：[0 红灯][1 绿灯][2 红绿一起亮] [3 红绿全部灭]
    ******************************************/
    void SetLaneLamp(int var);

    /******************************************
    IO_SetOut2
    设置继电器输出2，通过参数控制
    参数：[0 落下][1 抬起]
    ******************************************/
    void SetOut2(int var);
    //设置一辆车通过
    void SetOneVehPass();
    //设置一辆车过完
    void SetOneVehHasPassed();
    //锁杆
    void LockBar();
    void UnLockBar();
    bool IsLockBar();
    //模拟后线圈无车
    void SimBackLoop();
    //模拟后线圈有车
    void SimBackLoopCar();
protected:
    void SetDOStatus(DIGITAL_OUT devBit, bool status);
private:
    bool loadIOCardIni();
private:
    Status m_nLastState;
    bool m_bBarLocked;
    //输入IO对应的状态存储，存储对应枚举每一位
    quint32 m_nDIStatus;
    //输出IO对应的状态存储，存储对应枚举每一位
    quint32 m_nDOStatus;
    int m_nAlarmTimer;
    int m_nBarControlTime;
    quint64 m_ncurTimeMF,m_ncurTimeMS;
    QMap<int, int> m_inputIndex;
};

class IOEvent : public QEvent
{
public:
    explicit IOEvent(quint32 status): QEvent((Type)(User + 1))
    {
        m_InputStatus = status;
    }
    quint32 m_InputStatus;
};

#endif // DEVIOBOARD_H

