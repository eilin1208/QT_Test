#ifndef WTTHREAD_H
#define WTTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QMutex>
#include <QWaitCondition>
#include "datadef.h"
using namespace TNX;
class WtThread : public QThread
{
    Q_OBJECT
public:
    explicit WtThread(QObject *parent = 0);
    virtual ~WtThread();
signals:
public:
    int WimDevClearOneImpl();
    int WimDevClearOneDataImpl(int VehicleID);
    int WimDevTestImpl(char *bStatus);
    int WimDevInitImpl(int IniType);
    int WimDevSetComImpl(char *comNo,int bps);
    int WimDevGetCountImpl();
    int WimDevAxisDataImpl(int VehicleID, int AxisID, int *AxisType, long TyreWeight[4]);
    int WimDevDataImpl(int VehicleID, long *Weight, long *WtLimit, int AxisType[10], double *speed);
    int WimDevAxisInfoImpl(int VehicleID, long AxisWeight[MAX_AXLE_NUM], double *speed);
    int WimDevAxisCountImpl(int VehicleID);
    int WimDevAxisCount1Impl(int VehicleID);
    int WimDevAxisData1Impl(int VehicleID, int AxisID, int *AxisSpeed, int *Weight);
    int WimDevGetVerNoImpl(char *VerNo);
    int WimDevGetInfoImpl(char *Info);
    int WimDevSynchroImpl();
    int WimDevStopImpl();
public slots:
protected:
    void run();
protected:
    private:
    QSerialPort *m_pPort;
private:
    QMutex m_mutex;
    QString m_sComName;
    int m_nBps;
};

#endif // WTTHREAD_H
