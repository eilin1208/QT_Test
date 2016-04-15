/*********************************
//transBx 解析的包文体；不包括RSCTL和FrameType；
//message  不包括帧头帧尾，但包括RSCTL和FrameType
****************************************/

#include "rsutransframe.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/cardfileconverter.h"//cardfileconverter.h"
//#include "cardfileconverter.h"
#include <QDebug>
rsutransframe::rsutransframe()
{
    MessageFlag=false;
    SetDebugMessageFlag(true);
}
rsutransframe *GetRsuTransFrameInstance()
{
    static rsutransframe rsurtransObj;
    return &rsurtransObj;
}


RSUERRORCODE rsutransframe::TransB0(QRSUState &rsuState, const QByteArray message)
{
    int len = 0;
    QFrame_B0Raw raw;

    len=message.length()-3;
    rsuState.initQRSUState();

    if(len != sizeof(QFrame_B0Raw))
    {
        LogMessageToConsle(QObject::tr("B0帧数据的长度ERROR:QFrame_B0Raw:%1,QRSUState:%2")
                           .arg(sizeof(QFrame_B0Raw))
                           .arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(raw));
    }
    rsuState.status = raw.RSUStatus;
    memcpy(rsuState.RSUTerminalID1,raw.OLDRSUTerminalId,sizeof(raw.OLDRSUTerminalId));
    memcpy(rsuState.RSUTerminalID2,raw.NEWRSUTerminalId,sizeof(raw.NEWRSUTerminalId));
    rsuState.bRsuAlgID  = raw.RSUAlgId;
    rsuState.bRsuManuID = raw.RSUManulID;
    rsuState.RsuID = BCDToString(raw.RSUID,3);
    rsuState.RsuVersion=swapWORD(raw.RSUVersion);
    return rsuNoError;
}

RSUERRORCODE rsutransframe::TransB2(QFrame_B2 &rsuB2, const QByteArray message)
{
    int len;
    QFrame_B2Raw raw;

    len=message.length()-3;
    rsuB2.initQFrame_B2();

    if(len != sizeof(QFrame_B2Raw))
    {
        LogMessageToConsle(QObject::tr("B2帧数据的长度ERROR:QFrame_B0Raw:%1,QFrame_B2:%2")
                           .arg(sizeof(QFrame_B2Raw)).arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(QFrame_B2Raw));
    }

    rsuB2.OBUID= Swapquint32(raw.OBUID);
    rsuB2.ErrorCode= raw.ErrorCode;

    //发行商编号
    memcpy(rsuB2.IssuerIdentifier,raw.IssuerIdentifier,sizeof(raw.IssuerIdentifier));

    //应用序列号
    Raw2HexStr(rsuB2.ContractSerialNumer,raw.ContractSerialNumer,8);

    //启动时间
    bcd2systime(&(rsuB2.DateofIssue),raw.ContractSignedDate,4);

    //过期时间
    bcd2systime(&(rsuB2.DateofExpire),raw.ContractExpiredDate,4);

    //设备类型
    rsuB2.EquitmentStatus = raw.EquitmentStatus;

    //是否插卡
    if(raw.OBUStatus&0x80)//0：插卡 1：没插卡
    {
        rsuB2.IsCardInsert = false;
        LogMessageToConsle(QObject::tr("OBU没有插卡"));
        return rsuNoCardInsert;
    }
    else
    {
        rsuB2.IsCardInsert = true;

        //卡类型0:CPU卡, 1:MF1卡 0xff, 未知
        if(raw.OBUStatus&0x10)
        {
            rsuB2.CardType = MF1CARD;
        }
        else
        {
            rsuB2.CardType = CPUCARD;
        }

        //卡正常  正常（0），出错（1）
        if(raw.OBUStatus&0x08)
        {
            rsuB2.IsCardNormal = false;
            LogMessageToConsle(QObject::tr("B2帧解析:OBU卡异常"));
            return rsuCardAbnormal;
        }
        else
        {
            rsuB2.IsCardNormal = true;
        }

        //OBU被锁 未锁（0），被锁（1）
        if(raw.OBUStatus&0x04)
        {
            rsuB2.IsUnlocked = false;
            LogMessageToConsle(QObject::tr("B2帧解析:OBU被锁住"));
            return rsuCardLocked;
        }
        else
        {
            rsuB2.IsUnlocked = true;
        }

        //OBU拆卸指示 未被拆动（0），被拆动（1）
        if(raw.OBUStatus&0x02)
        {
            rsuB2.IsUndisassembled = false;
            LogMessageToConsle(QObject::tr("B2帧解析:OBU被拆卸"));
            return rsuCardDisassembled;
        }
        else
        {
            rsuB2.IsUndisassembled = true;
        }

        //OBU低电平  电池正常（0），电池电量低（1）
        if(raw.OBUStatus&0x01)
        {
            rsuB2.IsPowerNormal = false;
            LogMessageToConsle(QObject::tr("B2帧解析:OBU电池电量低"));
            return rsuCardPowerAbnormal;
        }
        else
        {
            rsuB2.IsPowerNormal = true;
        }
    }
    LogMessageToConsle(QObject::tr("B2帧解析:正确"));
    return rsuNoError;
}

RSUERRORCODE rsutransframe::TransB3(QFrame_B3 &rsuB3, const QByteArray message)
{
    int len;
    QFrame_B3Raw raw;

    len=message.length()-3;
    rsuB3.initQFrame_B3();

    if(len<6)
    {
        LogMessageToConsle(QObject::tr("B3帧数据的长度ERROR:QFrame_B3Raw:%1,QFrame_B3:%2")
                           .arg(sizeof(QFrame_B3Raw)).arg(len));
        return rsuLenghError;
    }

    if(len<sizeof(QFrame_B3Raw))
    {
        LogMessageToConsle(QObject::tr("B3帧数据的长度ERROR:QFrame_B3Raw:%1,QFrame_B3:%2")
                           .arg(sizeof(QFrame_B3Raw)).arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(QFrame_B3Raw));
    }

    if(raw.ErrorCode != 0x00)
    {
        LogMessageToConsle(QObject::tr("B3帧数据的CodeError"));
        return rsuRecivedB3CodeError;
    }

    rsuB3.OBUID=Swapquint32(raw.OBUID);
    rsuB3.ErrorCode=raw.ErrorCode;

    //车牌号
    rsuB3.VehPlateNumber += GB2312toUtf8(raw.VehPlateNumber);

    //车牌色
    rsuB3.VehPlateColor += GB2312toUtf8(raw.VehPlateColor);

    //车辆类型
    rsuB3.VehClass=QObject::tr("%1").arg(raw.VehicalClass);
    //车辆用户类型
    rsuB3.VehUserType=QString::number(raw.VehicalUserType,16);

    rsuB3.VehLength=Swapquint16(raw.VehLength);
    rsuB3.VehWidt=raw.VehWidth;
    rsuB3.VehHeight=raw.VehHeight;
    rsuB3.VehWheels=raw.VehWheels;
    rsuB3.VehAxles=raw.VehAxles;
    rsuB3.VehWheelBases=Swapquint16(raw.VehWheelBases);

    //车辆载重(货车)或座位数(客车)，二进制表示，单位为公斤/座
    rsuB3.VehWeightLimits = QString::fromLocal8Bit((char*)raw.VehWeightLimits,16);
    //车辆特征描述，字符用ASCII编码表示，汉字用机内码表示，如"奔驰307"
    rsuB3.VehSpInfor = QString::fromLocal8Bit(raw.VehSpInfor,16);
    //车辆发动机号
    rsuB3.VehEngineNumber=QString::fromLocal8Bit(raw.VehEngineNumber,16);
    LogMessageToConsle(QObject::tr("B3帧数据的解析：正确"));
    return rsuNoError;
}

RSUERRORCODE rsutransframe::TransB4(QFrame_B4 &rsuB4, const QByteArray message)
{
    int len;
    QFrame_B4Raw raw;

    len=message.length()-3;
    rsuB4.initQFrame_B4();
    int len1 = sizeof(QFrame_B4Raw);

    if(len != len1)
    {
        LogMessageToConsle(QObject::tr("B4帧数据的长度ERROR:QFrame_B4Raw:%1,QFrame_B4:%2")
                           .arg(sizeof(QFrame_B3Raw)).arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(QFrame_B4Raw));
    }

    if(raw.ErrorCode != 0x00)
    {
        LogMessageToConsle(QObject::tr("B4帧数据CodeError"));
        return rsuRecivedB4CodeError;
    }

    rsuB4.OBUID=Swapquint32(raw.OBUID);
    rsuB4.ErrorCode=raw.ErrorCode;
    rsuB4.CardRestMoney=Swapquint32(raw.CardRestMoney);

    //卡片基本信息，0015文件内容，
    if(raw.CardType == 0x00)//0：国标卡
    {
        sCardFileConverter.ProBasicRaw2ProCardBasicInfo(rsuB4.CardBasicInfo, raw.FileData0015);
        sCardFileConverter.Pro0012Raw2CardTollInfo(rsuB4.CardTollInfo, raw.FileData0019.raw0012, raw.FileData0009);
    }
    else if(raw.CardType == 0x01)//1: 原云南卡片
    {
//        POldPro0015Raw pOld0015;
//        pOld0015= (POldPro0015Raw)&raw.FileData0015;
//        sCardFileConverter.OldProBasicRaw2ProCardBasicInfo(rsuB4.CardBasicInfo, *pOld0015);//(TOldPro0015Raw)raw.FileData0015

//        POldPro0012Raw pOld0012;
//        pOld0012 = (POldPro0012Raw)&raw.FileData0019.raw0012;
//        sCardFileConverter.Old_Pro0012Raw2CardTollInfo(rsuB4.CardTollInfo, *pOld0012);
//        rsuB4.CardRestMoney = rsuB4.CardRestMoney*100;
    }
    else
    {
        LogMessageToConsle(QObject::tr("B4帧数据错误：卡类型未知"));
        return rsuCardTypeUnknown;
    }
    LogMessageToConsle(QObject::tr("B4帧数据解析：正确"));
    return rsuNoError;
}

RSUERRORCODE rsutransframe::TransB5(QFrame_B5 &rsuB5, const QByteArray message)
{
    int len;
    QFrame_B5Raw raw;

    len=message.length()-3;
    rsuB5.initQFrame_B5();

    if(len != sizeof(QFrame_B5Raw))
    {
        LogMessageToConsle(QObject::tr("B5帧数据的长度ERROR:QFrame_B5Raw:%1,QFrame_B5:%2")
                           .arg(sizeof(QFrame_B5Raw)).arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(QFrame_B5Raw));
    }
    if(raw.ErrorCode!=0x00)
    {
        LogMessageToConsle(QObject::tr("B5帧数据CodeError"));
        return rsuRecivedB5CodeError;
    }

    rsuB5.OBUID=Swapquint32(raw.OBUID);
    rsuB5.ErrorCode=raw.ErrorCode;

    //消费时间
    bcd2systime(&(rsuB5.PurchaseTime),(char *)(raw.PurchaseTime),7);

    //终端机交易序号
    Raw2HexStr(rsuB5.PSAMTerminalID, raw.PSAMTerminalID,6);

    //PSAM卡交易序号
    rsuB5.PSAMTransSerial=Swapquint32(raw.PSAMTransSerial);

    //CPU卡脱机交易序号
    rsuB5.ICCPayserial=Swapquint16(raw.ICCPayserial);

    //交易类型
    rsuB5.TransType=raw.TransType;

    //交易后卡余额，高位在前，低位在后；
    rsuB5.CardRestMoney=Swapquint32(raw.CardRestMoney);

    //TAC码
    rsuB5.TAC=Swapquint32(raw.TAC);

    //写文件时间，UNIX格式（此时间为天线自己计时时间，只能参考，不能用于验证）????
    UnixTimeToCTime2000(raw.WrFileTime,rsuB5.WrFileTime);
    LogMessageToConsle(QObject::tr("B4帧数据解析：正确"));
    return rsuNoError;
}

RSUERRORCODE rsutransframe::TransB6(QFrame_B6 &rsuB6, const QByteArray message)
{
    int len;
    QFrame_B6Raw raw;

    len=message.length()-3;
    rsuB6.initQFrame_B6();

    if(len != sizeof(QFrame_B6Raw))
    {
        LogMessageToConsle(QObject::tr("B6帧数据的长度ERROR:QFrame_B6Raw:%1,QFrame_B6:%2")
                           .arg(sizeof(QFrame_B6Raw)).arg(len));
        return rsuLenghError;
    }
    else
    {
        memcpy(&raw,message.right(message.length()-2).data(),sizeof(QFrame_B6Raw));
    }

    if(raw.ErrorCode != 0x00)
    {
        LogMessageToConsle(QObject::tr("B6帧数据CodeError"));
        return rsuRecivedB6CodeError;
    }

    rsuB6.OBUID=Swapquint32(raw.OBUID);
    rsuB6.ErrorCode=raw.ErrorCode;
    memcpy(&rsuB6.FileData0ARaw, &raw.FileData0ARaw, sizeof(raw.FileData0ARaw));
    LogMessageToConsle(QObject::tr("B6帧数据解析：正确"));
    return rsuNoError;
}

bool rsutransframe::TransC3(QFrame_C3Raw &raw, const quint32 OBUID,const TCardTollInfo& TollInfo, bool bOldCard)
{
    TPro0012Raw raw0012;
    POldPro0012Raw pOldRaw0012;
    bool flag=false;
    if(bOldCard)
    {
//        pOldRaw0012 = (POldPro0012Raw)&raw0012;
//        sCardFileConverter.Old_CardTollInfo2Pro0012Raw(*pOldRaw0012,TollInfo);
    }
    else
    {
        sCardFileConverter.CardTollInfo2Pro0012Raw(raw0012,TollInfo);
    }
    //清空数据，进行赋值
    memset(&raw,0,sizeof(raw));
    raw.OBUID=Swapquint32(OBUID);
    memcpy(raw.station,&raw0012,40);
    //时间提取
    systime2bcd((char*)raw.PurchaseTime,7,TollInfo.PassTime);
    flag=true;
    return flag;
}

bool rsutransframe::TransC4(QFrame_C4Raw &raw, const QFrame_C4& c4)
{
    bool flag=false;
    memset(&raw,0,sizeof(raw));
    raw.OBUID=Swapquint32(c4.OBUID);
    memcpy(&(raw.FileData09Raw),&(c4.FileData09Raw),sizeof(raw.FileData09Raw));
    flag=true;
    return flag;
}

bool rsutransframe::TransC6(QFrame_C6Raw& raw, const quint32 OBUID, const quint32 ConsumeMoney, const TCardTollInfo& TollInfo, bool bOldCard)
{
    TPro0012Raw raw0012;
    POldPro0012Raw pOldRaw0012;
    quint32 Money;
    bool flag=false;
    if(bOldCard)
    {
//        pOldRaw0012=(POldPro0012Raw)&raw0012;
//        sCardFileConverter.Old_CardTollInfo2Pro0012Raw((*pOldRaw0012),TollInfo);
//        Money=ConsumeMoney/100;
    }
    else
    {
        sCardFileConverter.CardTollInfo2Pro0012Raw(raw0012,TollInfo);
        Money=ConsumeMoney;
    }
    //清空数据进行赋值
    memset(&raw,0,sizeof(raw));
    raw.OBUID=Swapquint32(OBUID);
    raw.ConsumeMoney=Swapquint32(Money);
    memcpy(raw.ExitInfo,&raw0012,sizeof(raw.ExitInfo));
    //时间提取
    systime2bcd((char*)raw.PurchaseTime,7,TollInfo.PassTime);
    flag=true;
    return flag;
}

QString rsutransframe::GetOBUCardTypeName(quint8 CardType)
{
    QString temp_CardType;
    switch (CardType)
    {
    case 22:
        temp_CardType="储值卡";
        break;
    case 23:
        temp_CardType="记账卡";
        break;
    case 26:
        temp_CardType="全省免交";
        break;
    case 27:
        temp_CardType="全省包交";
        break;
    case 28:
        temp_CardType="路段免交储值卡";
        break;
    case 29:
        temp_CardType="路段包交储值卡";
        break;
    case 30:
        temp_CardType="路段免交记帐卡";
        break;
    case 51:
        temp_CardType="身份卡";
        break;
    default:
        temp_CardType="未知";
    }
    return temp_CardType;
}

void rsutransframe::LogMessageToConsle(QString message)
{
    if(MessageFlag)
        qDebug()<<"rsutransframe Debug Message:"<<message;
}

void rsutransframe::SetDebugMessageFlag(bool flag)
{
    flag=MessageFlag;
}

bool rsutransframe::GetDebugMessageFlag()
{
    return MessageFlag;
}
