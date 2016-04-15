#ifndef WTTHREAD_H
#define WTTHREAD_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include"TcpClient/qtcpclient.h"
#define MAX_AXLE_NUM 15
class WtThread : public QThread
{
    Q_OBJECT
public:
    explicit WtThread(QObject *parent = 0);
    virtual ~WtThread();
    quint8 GetXOR(const char *Buff,int nLen);

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
protected:
    void run();
private:
    QMutex m_mutex;
    int m_nBps;
};
void GetBack(QByteArray &data);
#endif // WTTHREAD_H
