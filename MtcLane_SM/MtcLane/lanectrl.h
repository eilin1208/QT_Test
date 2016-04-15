#ifndef LANECTRL_H
#define LANECTRL_H
//业务流程控制类
#include <QObject>
#include "vehqueuemgr.h"
#include "tollitem.h"
#include "IoBoardArm/devioboard.h"
#include "MtcKey/MtcKeyDef.h"
enum LANECTRLSTATE
{
    LCTRL_Normal,       //正常车辆
    LCTRL_Motor,        //车队
    LCTRL_Tract         //拖车
};
class LaneCtrl : public QObject
{
    Q_OBJECT
public:
    explicit LaneCtrl(QObject *parent = 0);
    
signals:
    void AllowCarPassing(int nNum);
public slots:
    void IoInputChanged(quint32 dev, quint8 nStatus);
    void IoOutputChanged(int dev, int nStatus);
    void KeyPressed(const MtcKeyPropertity &keyPropertity);
public:
    bool IsMotoring()
    {
        return m_nState == LCTRL_Motor;
    }

    //车辆到达后线圈
    void CarPassing();
    //车辆离开后线圈
    void CarPassed();
    //获取等待车辆离开数
    int GetVehAllowPass();
    //是否允许处理拖车
    bool IsAllowTract();
    //是否允许重打发票
    bool IsAllowReprint();
    //是否是拖车
    bool IsTract();
    //是否可以进行下一辆车业务
    bool CanDoNextCar();
    //是否允许下班
    bool GetAllowLogOut();
    //是否允许倒车
    bool GetAllowReverse(int& nCardType, quint32& dwPhysicalCardId, QString& sCardId);
    //车队开始、结束
    void DoMotor(bool bStart, int nVct = 0);
    //车队过车
    void MotorPass();
    //拖车处理
    void DoTract();
    //锁杆处理
    void DoLockBar();
    //倒车处理
    void DoReverse();
    //重新打印发票
    bool DoReprintInvoice();
    //处理闯关
    void DoViolate(int nViolateType);
    //获取报文发送对象
    TollItem* getTollItem();
    //初始化LaneCtrl
    void Init();
    //发送车道事件报文
    void DoLaneEvent(TLaneEventType nLaneEvent);
    //启动定时器
    void StartNewParamTimer();
    void StartDownLoadTimer();
    void StartVerifyTimer();
    void StartBigShiftTimer();
    // liujian
    void StartDelDBDataTimer();
    void StartDelPicDataTimer();
    void startUpdateTimer();
public:
    //重置
    void Reset();
    //保存图片
    bool SavePic(const QDateTime& capTime, int nPicType);
    //设置允许过车数
    void SetAllowPass(int nCount = 1);
    //抬杆确认 入口写卡成功，按确认键后，出口付费完毕
    void ConfirmComplete(int nAllowCount = 1);
    //发送正常车辆正式报文
    void CompleteTransact(int nInductCount);
    //处理完毕，入口写卡成功 出口按[现金]键，或其他付费结束
    //填充报文，并保存业务临时数据以及工班数据
    //对于一般发卡、收费业务nAllowPassCount=1;
    //对于补卡和重新处理,nAllowPassCount=0;
    bool Complete(const QDateTime& OccurTime, bool bFinished=true);
    //牵引车业务结束
    bool TractComplete();
    void setRepairCard(bool ret);
    bool getRepairCard();
    //检查加载新版参数文件
    void CheckLoadNewParam();
    //检查下载新版参数文件
    void CheckDownLoadParam();
    //检查系统校时
    void CheckVerifyTime();
    //检查更新大工班
    void CheckUpdateShift();
    //检查定时删除图片信息
    //void CheckDelPic(int saveDays = 3);
protected:
    void timerEvent(QTimerEvent *event);
private:
    void autoClose();
private:
    VehQueueMgr m_vehQueue;         //过车计数器
    TollItem* m_pTollItem;          //数据保存对象
    int m_nTractIndex;              //拖车数
    bool m_bReSendCard;             //是否补卡
    LANECTRLSTATE m_nState;         //过车状态
    bool m_bWaitingCarPass;         //等待过车
    //判断参数文件是否达到加载时间的定时器ID
    int m_nParamTimerId;
    //校时判断定时器ID
    int m_nVerifyTimerId;
    //参数文件更新定时器
    int m_nUpdateTimerId;
    //更新大工班定时器
    int m_nBigShiftTimerId;
    //下载参数文件次数统计
    int m_nDownloadCnt;
    //判断是否删除xml及数据库数据的定时器ID
    int m_nDelDBDataTimerId;

    //定时更新timerid
    int m_updateTimerId;
};

LaneCtrl* getLaneCtrl();

#endif // LANECTRL_H
