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
    void SendDelCommand(unsigned char del_id_b,unsigned char Cac_num);
    //发送查询命令
    void SendChkCommand();
    void SenOthCommand(unsigned char Con_chr);
    //数据验证
    bool DataCRC();
    //接受程序
    void DataReceive(void);
    void DataProc(void);
    void Un_Deal_data();
    void AddAxleGrp(unsigned char* axleGrpType,int index, int &axleGrpSum,unsigned char val1, unsigned char val2);
    bool Read_Synchro_data();
    bool Write_Synchro_data();
    void ClearVDOne(unsigned char a);
    void DivideAxleGrp(unsigned char* axleGrpType,unsigned char* axleGrpDiscription,int &axleGrpSum);
private:
    QSerialPort *m_pPort;
private:
    int g_Synchro_sig;//同步标志
    VehicleData VD[CACHNUM];//缓存
    unsigned char cVehNum;//车数
    unsigned char cCurVeh;//当前待发送的车辆指针
    unsigned char cLastVeh;//要插入车辆数据的指针
    unsigned char cEquSta;//设备状态
    unsigned char clowCon[SEND_LEN];//命令字
    unsigned char clowConDel[SEND_LEN];//删除
    unsigned char clowConChk[SEND_LEN];//查询
    unsigned char clowConRe[SEND_LEN];//重发
    unsigned char buf[1024];//接受缓存
    unsigned char DataBuf[100];//数据缓存
    unsigned char count,gcnt;
    int bReSend;
    unsigned char del_id;
    unsigned char del_sign;
    unsigned long dss;
    int lStaticVal;
    unsigned char ComTerNum;
    unsigned char nBreakFrame;
    int fsf,fvf;

    unsigned int Chech_Num; //设备状态变量
    bool bTerm;
    //涉及到操作缓存时，要注意加锁处理
    QMutex m_cachMutex;
    //串口操作加锁
    QMutex m_portMutex;
    //同步传输condition
    QMutex m_syn;
    QMutex m_com;
    QMutex m_del;
    QMutex m_fle;
    QWaitCondition m_synCondition;
    QWaitCondition m_comCondition;
    QWaitCondition m_delCondition;
    QWaitCondition m_fleCondition;
    QString m_sComName;
    int m_nBps;
};

#endif // WTTHREAD_H
