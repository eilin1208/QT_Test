#ifndef SHIFTSUM_H
#define SHIFTSUM_H

#include <QObject>
#include "CommonModule/LaneType.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/MsgDefine.h"
#include "PacketSendModule/msgsendfunc.h"
class ShiftSum : public QObject
{
    Q_OBJECT
public:
    explicit ShiftSum(QObject *parent = 0);
    
signals:
    
public slots:
public:
    //*************以下为大班汇总信息*****************
    //设置大班结束时间
    void SetBigShiftEndTime(const QDateTime& endTime);
    //开始新的大班汇总
    void StartNewBigShift(int nShiftNo,const QDate& dateTime, const QDateTime& startTime);
    //返回大班汇总信息
    TMsgShiftSum &GetBigShiftSum();
    void GetBigShiftInfo(TMsgShiftSum& AShiftSum);
    //返回班次、逻辑日期是否相同
    bool IsBigShiftSame(const int nShiftNo, const QDate& workDate);
    //返回工班中班次、逻辑日期是否比当前的要快
    bool LessThan(const int nShiftNo, const QDate& workDate);
    //检查是否有大班汇总信息
    bool HaveBigShiftSumInfo();
    //清除大班汇总信息
    void ClearBigShiftSum();
public:
    //*************以下为小班汇总相关信息*********************
    //开始新的小工班汇总 .
    void StartNewSmallShift(const TMsgToSend& msg);
    //设置小班结束时间
    void SetSmallShiftEndTime(const char* sEndTime);
    //返回小班汇总信息
    TMsgShiftSum GetSmallShiftSum();
    void GetSmallShiftInfo(TMsgShiftSum& shiftSum);
    //检查是否有小班汇总信息
    bool HaveSmallShiftSumInfo();
    //清除小班汇总信息
    void ClearSmallShiftSum();
public:
    //****************工班汇总共用**********************
    //从文件中加载工班数据
    bool LoadFromFile();
    //增加工班数
    void Sum(const TMsgToSend& msg);
    //增加车队过车增加总车辆计数
    void AddMotor();
    //增加线圈计数   1-前线圈 2-后线圈
    void AddLoop(int nLoop);
    //增加栏杆计数  True 抬杆 False 落杆
    void AddBar(bool bStatus);
    //保存卡盒信息
    void SaveCardBox();
public:
    //更新班次汇总事件
    //UpdateShiftSum:TNotifyEvent;
    //返回卡盒信息
    void GetShiftCardBoxInfo(TMsgCardBoxInfo& cardBoxInfo);
    //获取正常过车数(不包含闯关)
    int GetNormalVCnt();
    //获取总收费额
    int GetTotalMoney();
    //获取总通行卡数
    int GetTotalCardVCNt();

private:
    void AddSum(const TMsgShiftSum& single, TMsgShiftSum& sum);

private:
    //小工班合计信息
    TMsgShiftSum m_smallShiftSum;
    //大工班合计信息
    TMsgShiftSum m_bigShiftSum;
    //卡盒信息
    TMsgCardBoxInfo m_cardBoxInfo;

};
ShiftSum* getShiftSum();
#endif // SHIFTSUM_H
