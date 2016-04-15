#ifndef RSUTRANSFRAME_H
#define RSUTRANSFRAME_H
/****************************************************/
/*文件说明：本文件包含天线的帧的解析方法和封装方式************/
/****************************************************/
#include "rsudatatype.h"
#include "CommonModule/cardfileconverter.h"//cardfileconverter.h"
class rsutransframe
{
public:
    rsutransframe();
public:
    /*************************天线发送给上位机的帧数据解析****************/
    //转换B0帧
    RSUERRORCODE TransB0(QRSUState &rsuState,const QByteArray message);
    //转换B2帧
    RSUERRORCODE TransB2(QFrame_B2 &rsuB2,const QByteArray message);
    //转换B3帧
    RSUERRORCODE TransB3(QFrame_B3 &rsuB3,const QByteArray message);
    //转换B4帧
    RSUERRORCODE TransB4(QFrame_B4 &rsuB4,const QByteArray message);
    //转换B5帧
    RSUERRORCODE TransB5(QFrame_B5 &rsuB5,const QByteArray message);
    //转换B6帧
    RSUERRORCODE TransB6(QFrame_B6 &rsuB6,const QByteArray message);
    /*************************上位机发送给天线的指令封装*****************/
    //转换C3帧
    bool TransC3(QFrame_C3Raw &raw,const quint32 OBUID,const TCardTollInfo& TollInfo,bool bOldCard);
    //转换C4帧
    bool TransC4(QFrame_C4Raw &raw,const QFrame_C4& c4);
    //转换C6帧
    bool TransC6(QFrame_C6Raw &raw, const quint32 OBUID,const quint32 ConsumeMoney,
    const TCardTollInfo& TollInfo,bool bOldCard);
    //日志调试信息的标记为的设置
    void SetDebugMessageFlag(bool flag);
    //日志调试信息的标记位的获取
    bool GetDebugMessageFlag();
private:
    //打印日志信息到控制台
    void LogMessageToConsle(QString message);
public:
    //获取卡类型名称
    QString GetOBUCardTypeName(quint8 CardType);
private:
    //卡文件转换对象
    TCardFileConverter sCardFileConverter;
    bool MessageFlag;
};
rsutransframe *GetRsuTransFrameInstance();
#endif // RSUTRANSFRAME_H
