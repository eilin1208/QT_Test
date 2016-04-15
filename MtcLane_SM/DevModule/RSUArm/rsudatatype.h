#ifndef RSUDATATYPE_H
#define RSUDATATYPE_H
/****************************************************************
**文件说明：本文件包含和RSU有关的数据类型的定义
****************************************************************/
#include "CommonModule/CardFileDef.h"
#include <QObject>
#pragma once
#pragma pack(push, 1)

//RSU相关代号
typedef enum
{
    rsuNoError=1,
    rsuLenghError,
    rsuFormatError,
    rsuNoCardInsert,
    rsuCardAbnormal,
    rsuCardLocked,
    rsuCardDisassembled,
    rsuCardPowerAbnormal,
    rsuRecivedB3CodeError,
    rsuRecivedB4CodeError,
    rsuCardTypeUnknown,
    rsuRecivedB5CodeError,
    rsuRecivedB6CodeError,
    rsuBCCNoPassed,
    rsuHeartBeat

}RSUERRORCODE;
Q_DECLARE_METATYPE(RSUERRORCODE);
//天线的设备状态
typedef enum
{
    RsuNoError=1,
    RsuInit,
    RsuNoPSAM,
    RsuCommError
}QRSUDevStatus;
//天线的工作状态
typedef enum
{
    RsuEvent_RcvMsg=1,      //接收到天线的数据了
    RsuEvent_HeartBeat,     //天线的心跳信号(天线正常工作)
    RsuEvent_OnPower,       //天线上电
    RsuEvent_RsuState,      //收到B0帧(天线的基本信息)
    RsuEvent_OBUBaseInfo,   //收到B2帧(OBU的基本信息)
    RsuEvent_OBUVehInfo,    //收到B3帧(OBU的车型信息)
    RsuEvent_IccInfo,       //收到B4帧(Ic卡信息)
    RsuEvent_TransResult,   //收到B5帧(交易结果)
    RsuEvent_FreeArea       //收到特殊帧(要免费的区域)
}QRSUWorkStatus;

//卡类型
typedef enum
{
    CPUCARD     = 0X00,
    MF1CARD     = 0X01,
    UNKNOWNCARD = 0XFF

}enumCardType;

//RSU帧信息
//RSU设备状态信息（B0）帧
struct QFrame_B0Raw
{
    quint8 RSUStatus;                       //Rsu状态0x00 正常 其他异常
    quint8 OLDRSUTerminalId[6];             //终端机编号1(MF1)
    quint8 NEWRSUTerminalId[6];             //终端机编号2(Pro)
    quint8 RSUAlgId;                        //算法标识ID
    quint8 RSUManulID;                      //RSU厂商代码16进制表示 0x01 埃特思 02金益03聚利04首信05东海08万集
    quint8 RSUID[3];                        //RSU编号,16进制表示
    quint16 RSUVersion;                     //软件版本,16进制表示
    quint8 ReServed[5];                     //保留
};
struct QRSUState
{
    quint8 status;                          //RSU主状态参数,00正常
    quint8 RSUTerminalID1[6];               //终端机编号6字节 quint81终端类型，quint82地区号(BCD)
    quint8 RSUTerminalID2[6];
    quint8 bRsuAlgID;                       //算法标识,16进制表示
    quint8 bRsuManuID;                      //厂商代码,16进制表示
    QString RsuID;                          //RSU编号,16进制表示
    quint16 RsuVersion;                     //软件版本号

public:
    void initQRSUState()
    {
        status = 0;
        memset(RSUTerminalID1, 0, sizeof(RSUTerminalID1[6]));
        memset(RSUTerminalID2, 0, sizeof(RSUTerminalID2[6]));
        bRsuAlgID   = 0;
        bRsuManuID  = 0;
        RsuID.clear();
        RsuVersion  = 0;

    }
    void CopyData(const QRSUState &rsuState)
    {
        status=rsuState.status;
        memcpy(RSUTerminalID1,rsuState.RSUTerminalID1,6);
        memcpy(RSUTerminalID2,rsuState.RSUTerminalID2,6);
        bRsuAlgID=rsuState.bRsuAlgID;
        bRsuManuID=rsuState.bRsuManuID;
        RsuID=rsuState.RsuID;
        RsuVersion=rsuState.RsuVersion;
    }
};
Q_DECLARE_METATYPE(QRSUState);
//OBU号信息(B2)帧
struct QFrame_B2Raw
{
    quint32 OBUID;                          //OBU号,高位在前
    quint8 ErrorCode;                       //取值00H,才有后续数据 0x08表示心跳数据
    char IssuerIdentifier[8];               //发行商编码
    quint8 AgreementType;                   //协议类型
    quint8 AgreementVer;                    //协议版本
    quint8 ContractSerialNumer[8];          //应用序列号BCD码
    char ContractSignedDate[4];             //启用日期
    char ContractExpiredDate[4];            //过期日期
    quint8 CollICCardID[10];                //绑定IC卡卡号
    quint8 EquitmentStatus;                 //OBU设备类型
    quint8 OBUStatus;                       //OBU状态
    quint8 ReServed;
};
struct QFrame_B2
{
    quint32 OBUID;                          //OBU号,高位在前
    quint8 ErrorCode;                       //错误代码00H,才有后续数据 0x08表示心跳数据
    char IssuerIdentifier[8];               //发行商编码
    char ContractSerialNumer[17];           //应用序列号BCD码
    QDateTime DateofIssue;                  //启用日期
    QDateTime DateofExpire;                 //过期日期
    quint8 EquitmentStatus;                 //OBU设备类型
    bool IsCardInsert;                      //是否插卡
    quint8 CardType;                        //卡类型0:CPU卡, 1:MF1卡 0xff, 未知
    bool IsCardNormal;                      //卡正常
    bool IsUnlocked;                        //OBU被锁
    bool IsUndisassembled;                  //OBU拆卸指示
    bool IsPowerNormal;                     //OBU低电平

public:
    void initQFrame_B2()
    {
        OBUID = 0;
        ErrorCode = 0;

        memset(IssuerIdentifier,0,sizeof(IssuerIdentifier));
        memset(ContractSerialNumer,0,sizeof(ContractSerialNumer));
        //启用日期
        DateofIssue.setDate(QDate(2012,10,11));
        //过期日期
        DateofExpire.setDate(QDate(2012,10,11));

        EquitmentStatus = 0;
        IsCardInsert    = false;
        CardType        = UNKNOWNCARD;
        IsCardNormal    = false;
        IsUnlocked      = false;
        IsUndisassembled= false;
        IsPowerNormal   = false;
    }
};
Q_DECLARE_METATYPE(QFrame_B2);
//OBU属性(B3)帧
struct QFrame_B3Raw
{
    quint32 OBUID;                          //OBU号，高位在前
    quint8 ErrorCode;                       //执行状态代码 00H 才有后续数据
    char VehPlateNumber[12];                //车牌号
    char VehPlateColor[2];                  //车牌颜色
    quint8 VehicalClass;                    //车辆类型
    quint8 VehicalUserType;                 //用户类型
    quint16 VehLength;                      //车辆长
    quint8 VehWidth;                        //车辆宽
    quint8 VehHeight;                       //车辆高
    quint8 VehWheels;                       //车轮数
    quint8 VehAxles;                        //车轴数
    quint16 VehWheelBases;                  //轴距,二进制表示,长度2字节单位为分米 0x28 表示轴距4米
    quint8 VehWeightLimits[3];              //车辆载重(货车)或座位数(客车)，二进制表示，单位为公斤/座
    char VehSpInfor[16];                    //车辆特征描述，字符用ASCII编码表示，汉字用机内码表示，如"奔驰307"
    char VehEngineNumber[16];               //车辆发动机号
};
struct QFrame_B3
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码，此处取值0x00
    QString VehPlateNumber;                 //车牌号
    QString VehPlateColor;                  //车牌色
    QString VehClass;                       //车辆类型
    QString VehUserType;                    //车辆用户类型
    quint16 VehLength;                      //车辆长
    quint8 VehWidt;                         //车辆宽
    quint8 VehHeight;                       //车辆高
    quint8 VehWheels;                       //车轮数
    quint8 VehAxles;                        //车轴数
    quint16 VehWheelBases;                  //轴距，二进制表示，长度为2个字节，单位为分米；如0x28，表示轴距为4米。//dafang 8->16
    QString VehWeightLimits;                //车辆载重(货车)或座位数(客车)，二进制表示，单位为公斤/座  //dafang qint8 ->QString
    QString VehSpInfor;                     //车辆特征描述，字符用ASCII编码表示，汉字用机内码表示，如"奔驰307"
    QString VehEngineNumber;                //车辆发动机号

public:
    void initQFrame_B3()
    {
        OBUID       = 0;
        ErrorCode   = 0;
        VehPlateNumber.clear();
        VehPlateColor.clear();
        VehClass.clear();
        VehUserType.clear();
        VehWidt     = 0;
        VehHeight   = 0;
        VehWheels   = 0;
        VehAxles    = 0;
        VehWheelBases=0;
        VehWeightLimits.clear();
        VehSpInfor.clear();
        VehEngineNumber.clear();
    }
};
Q_DECLARE_METATYPE(QFrame_B3);
//IC卡信息(B4)帧
struct QFrame_B4Raw   //yun nan ka
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码，此处取值0x00
    quint32 CardRestMoney;                  //卡余额，高位在前，低位在后；
    quint8 CardType;                        //卡片类型：0：国标卡；1：原云南CPU卡。
    TPro0015Raw FileData0015;               //卡片基本信息，0015文件内容，43字节
    TPro0019Raw FileData0019;               //过站信息 ，0019文件内容，43字节
    TPro0009Raw FileData0009;               //标识站信息，0009文件内容，9字节
};
struct QFrame_B4
{
    quint32 OBUID;                          //OBU编号
    quint8  ErrorCode;                      //执行状态代码，此处取值0x00
    quint32 CardRestMoney;                  //卡余额，高位在前，低位在后；
    TProCardBasicInfo CardBasicInfo;        //卡片基本信息，0015文件内容，
    TCardTollInfo CardTollInfo;             //过站信息 ，0019、0009文件内容

public:
    void initQFrame_B4()
    {
        OBUID       = 0;
        ErrorCode   = 0;
        CardRestMoney = 0;

        //卡片基本信息，0015文件内容  未初始化
        //过站信息 ，0019、0009文件内容  未初始化

    }
};
Q_DECLARE_METATYPE(QFrame_B4);
//成功交易结束(B5)帧
struct QFrame_B5Raw
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码
    quint8 PurchaseTime[7];                 //消费时间
    quint8 PSAMTerminalID[6];               //终端机交易序号
    quint32 PSAMTransSerial;                //PSAM卡交易序号
    quint16 ICCPayserial;                   //CPU卡脱机交易序号
    quint8 TransType;                       //交易类型
    quint32 CardRestMoney;                  //交易后卡余额，高位在前，低位在后；
    quint32 TAC;                            //TAC码
    quint32 WrFileTime;                     //写文件时间，UNIX格式（此时间为天线自己计时时间，只能参考，不能用于验证）
};
struct QFrame_B5
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码
    QDateTime PurchaseTime;                 //消费时间
    char PSAMTerminalID[12];                //终端机交易序号
    quint32 PSAMTransSerial;                //PSAM卡交易序号
    quint16 ICCPayserial;                   //CPU卡脱机交易序号
    quint8 TransType;                       //交易类型
    quint32 CardRestMoney;                  //交易后卡余额，高位在前，低位在后；
    quint32 TAC;                            //TAC码
    QDateTime WrFileTime;                   //写文件时间，UNIX格式（此时间为天线自己计时时间，只能参考，不能用于验证）
public:
    void initQFrame_B5()
    {
        OBUID       = 0;
        ErrorCode   = 0;
        PurchaseTime.setDate(QDate(2012,10,11));
        memset(PSAMTerminalID,0,sizeof(PSAMTerminalID));
        PSAMTransSerial = 0;
        ICCPayserial    = 0;
        TransType       = 0;
        CardRestMoney   = 0;
        TAC             = 0;
        WrFileTime.setDate(QDate(2012,10,11));
    }
};
Q_DECLARE_METATYPE(QFrame_B5);
//云通卡免费区间信息(B6)帧
struct QFrame_B6Raw
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码
    TPro000ARaw FileData0ARaw;              //免费信息  000A文件内容 83个字节
};
struct QFrame_B6
{
    quint32 OBUID;                          //OBU编号
    quint8 ErrorCode;                       //执行状态代码
    TPro000ARaw FileData0ARaw;              //免费信息  000A文件内容 83个字节

public:
    void initQFrame_B6()
    {
        OBUID       = 0;
        ErrorCode   = 0;
        memset(&FileData0ARaw, 0, sizeof(FileData0ARaw));
    }
};
Q_DECLARE_METATYPE(QFrame_B6);

//消费记录信息(B7)帧
struct QFrame_B7Raw
{
    //    quint32 OBUID;      //	OBU号,如12：保存为 00 00 00 0C      4字节16进制数
    //    quint8  ErrorCode;  //	错误代码，取值为00H，才有后续数据内容。
    //    quint8  RecordNum;  //	记录条数
    //    quint8  *Record;     //	交易记录文件23*RecordNum

};
//Q_DECLARE_METATYPE(QFrame_B7);

//卡片脱机交易序列号(B9)帧
struct QFrame_B9Raw
{
    quint32 OBUID;      //	OBU号,如12：保存为 00 00 00 0C      4字节16进制数
    quint8  ErrorCode;  //	错误代码，取值为00H，才有后续数据内容
    quint16 ICCPayserial;//	CPU卡脱机交易序号
};
//Q_DECLARE_METATYPE(QFrame_B9);
//写过站信息指令－C3
struct QFrame_C3
{
    quint32 OBUID;
    TCardTollInfo Entrydata;
    QDateTime PurchaseTime;
};
struct QFrame_C3Raw
{
    quint32 OBUID;                          //OBU编号
    quint8 station[40];                     //过站信息40字节
    quint8 PurchaseTime[7];                 //消费时间
};
//写标识站信息指令－C4
struct QFrame_C4Raw
{
    quint32 OBUID;
    TPro0009Raw FileData09Raw;              //标识站信息  0009文件  9个字节
};
struct QFrame_C4
{
    quint32 OBUID;
    TPro0009Raw FileData09Raw;              //标识站信息  0009文件  9个字节
};
//消费指令－C6
struct QFrame_C6
{
    quint32 OBUID;
    quint32 ConsumeMoney;
    TCardTollInfo ExitStation;
    QDateTime PurchaseTime;
};
struct QFrame_C6Raw
{
    quint32 OBUID;
    quint32 ConsumeMoney;
    quint8 ExitInfo[40];                    //过站信息40字节
    quint8 PurchaseTime[7];                 //消费时间
};

//取交易记录指令-C7
//struct QFrame_C7
//{
//    quint32 OBUID;
//    quint8  RecordNUM;  //需要读取的记录条数(小于10条)
//};
struct QFrame_C7Raw
{
    quint32 OBUID;
    quint8  RecordNUM;  //需要读取的记录条数(小于10条)
};

//取TAC码指令1-C8 (1)	如果为单天线系统，此帧命令用于从天线取回TAC码，如果为双天线系统，此帧命令用于从同一天线取回TAC码
//struct QFrame_C8
//{
//    quint32 OBUID;
//    quint16  CardPaySerial;//	卡交易序列号  为先前B9中返回的 CardPaySerial；
//};
struct QFrame_C8Raw
{
    quint32 OBUID;
    quint16  CardPaySerial;//	卡交易序列号  为先前B9中返回的 CardPaySerial；
};

//取TAC码指令2-C9 (1)	此命令专用于双天线系统，单天线无此命令
//struct QFrame_C9
//{
//    quint32 OBUID;
//    quint16  CardPaySerial;//	卡交易序列号  为先前B9中返回的 CardPaySerial；
//};
struct QFrame_C9Raw
{
    quint32 OBUID;
    quint16  CardPaySerial;//	卡交易序列号  为先前B9中返回的 CardPaySerial；
};
#pragma pack(pop)
#endif // RSUDATATYPE_H
