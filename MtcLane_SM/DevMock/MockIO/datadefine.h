#ifndef DATADEFINE_H
#define DATADEFINE_H
#include <QObject>
#include <QFile>
#include<QDateTime>
#include <QDebug>
#include <QMap>
#include <QVariant>
#include <QMap>
enum DEVTYPE
{
    DEVBEGIN,
    //这里写数据类型
    DEV_IO = 0x01,
    DEV_FD = 0x02,
    DEV_PRT = 0x03,
    DEV_VPR = 0x04,
    DEV_WT = 0x05,
    DEVEND
};

enum DIGITAL_IN
{
    DI_LoopTrig1,	//感应线圈
    DI_LoopTrig2,	//存在线圈
    DI_LoopFront,       //抓拍线圈
    DI_LoopBack,        //通过线圈
    DI_BarrierDown,
    DI_BarrierUp,
    DI_AlarmDetector
};

enum DIGITAL_OUT
{
    DO_CanopyLightGreen,
    DO_CanopyLightRed,
    DO_PassLightGreen,
    DO_PassLightRed,
    DO_BarrierDown,
    DO_BarrierUp,
    DO_AlarmLight,
    DO_AlarmSound
};

struct DataFrame
{
    //帧类型
    int frameType;
    //帧长度
    int frameLength;
    //帧内容
    char* frameBuf;
    //校验码
    quint8 bcd;
};
#endif // DATADEFINE_H
