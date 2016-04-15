#ifndef VDMARM_H
#define VDMARM_H
#include <QDateTime>
#include "BaseInterface/devbaseinterface.h"
typedef int (*Vdm_line)(unsigned char line , unsigned char coloum, unsigned char nlen, char *VdmChar);
typedef int (*Vdm_Init)();
typedef int (*Sync_Vdm_Time)();
class VDMArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit VDMArm(const QString& sName, QObject *parent = 0);

signals:

public slots:

public:
    // 更新车道信息
    bool UpdateLane(char* lpszStaName, int nLaneId);
    // 更新采集者
    bool UpdateCollector(quint32 dwCollectorId, bool bAddOffDuty);
    // 更新车型
    bool UpdateVehClass(int nVehClass);
    // 更新车类
    bool UpdateVehTollType(int nVehTollType);
    // 清除所有信息
    bool ClearVehInfo();
    // 更新时间
    bool UpdateDateTime(const QDateTime &CurrentTime);
    // 更新时间
    bool UpdateDateTime();
    //叠加收费金额
    bool UpdateFare(int nFare);
protected:
    //加载驱动
    virtual bool LoadFunction();
    //卸载驱动
    virtual bool ReleaseFunction();
    //关闭
    virtual bool Close();
    //初始化
    virtual bool Init();
    //更新时间定时器
    void timerEvent(QTimerEvent *event);
private:
    Vdm_line pFunVdmLine;
    Vdm_Init pFuncInit;
    Sync_Vdm_Time pFuncSyncVdmTime;
    int m_timerId;
};

#endif // VDMARM_H
