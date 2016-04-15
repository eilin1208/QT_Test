#include "tollitem.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "ParamModule/qbaserateparamfile.h"
#include "ParamModule/qallrateparamfile.h"
#include "CommonModule/ilogmsg.h"
#include "devicefactory.h"
#include "PacketSendModule/msgsendfunc.h"
#include "PacketSendModule/sqlitedatasaver.h"
#include "shiftsum.h"
#include "PacketSendModule/monclient.h"
#include "CardReaderArm/qcardreader.h"
#include "laneinfo.h"
#include "MtcKey/MtcKeyDef.h"
#include "formrecordtable.h"
#include "formmainwindow.h"
#include "formvpr.h"
#include "XMLBakup/xmlbakupwriter.h"
#include "ZLib/include/zlib.h"
TollItem::TollItem(QObject *parent) :
    QObject(parent)
{
    FYWDataToSave.clear();
    //初始化报文
    ClearSavedInfo();
    ClearSendMsg(FMsgToSend);
    ClearSendMsg(FLastMsgToSend);
    ClearSendMsg(m_HdgMsgToSend);
    //初始化图像信息
    ClearImageInfo();
    //设备状态
    memset(&FDevStatus,'0',sizeof(FDevStatus));
}
//填充基本信息 填充报文基本信息
void TollItem::FillBasicInfo(const QDateTime &OccurTime, TMsgToSend &msg, bool bPeekSN)
{
    QString tmpstr,sExtParaVer,sCommParaVer,sKeyVer;
    int nFileNum;
    QSysParaDicParamFile *sysParam;
    QSysParamInfo SysInfo;
    QCardReader* pCardReader = NULL;
    //业务开始时间
    memcpy(msg.LaneInfo.OccurTime,m_dOccTime.toString("yyyyMMddhhmmss").toLocal8Bit().data(),sizeof(msg.LaneInfo.OccurTime));
    sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    SysInfo = sysParam->GetSysParamInfo();
    //压线圈时间
    DateTimeToChar14(FTiggerTime, msg.LaneInfo.TriggerTime);
    //第一次车型输入时间
    DateTimeToChar14(FVTInputTime, msg.LaneInfo.VTInputTime);
    //业务结束时间
    DateTimeToChar14(OccurTime, msg.LaneInfo.EndWasteTime);

    //收费站编码
    IntToArChar(msg.LaneInfo.StationID,6,SysInfo.m_nStationID);
    //车道编码
    IntToArChar(msg.LaneInfo.LaneID,3,getLaneInfo()->getLaneCode());
    //车道业务序列号
    if(bPeekSN)
    {
        //取当前流水
        msg.LaneInfo.WasteSN = getLaneInfo()->ReadTransID();
    }
    else
    {
        //取当前流水后加一
        msg.LaneInfo.WasteSN = getLaneInfo()->GetTransID();
    }
    //所属省级中心
    memcpy(msg.LaneInfo.BL_ProvID,getLaneInfo()->getOrgInfo().sBL_ProvID.toLocal8Bit().data(),sizeof(msg.LaneInfo.BL_ProvID));
    //所属区域中心编号
    memcpy(msg.LaneInfo.BL_NetWorkID,getLaneInfo()->getOrgInfo().sBL_NetID.toLocal8Bit().data(),sizeof(msg.LaneInfo.BL_NetWorkID));
    //所属路段
    memcpy(msg.LaneInfo.BL_RoadID,getLaneInfo()->getOrgInfo().sBL_Road.toLocal8Bit().data(),sizeof(msg.LaneInfo.BL_RoadID));
    //所属分中心
    IntToArChar(msg.LaneInfo.BL_SubCenter,6,SysInfo.m_nBLSubCenter);

    //车道类型
    msg.LaneInfo.OrgType = Tran_LaneType((LaneType)getLaneInfo()->getLaneType());

    //车道程序版本号
    tmpstr = QString::number(getLaneInfo()->GetAppVer());
    memcpy(msg.LaneInfo.LaneAppVer,tmpstr.toLocal8Bit().data(),qMin(sizeof(tmpstr),sizeof(msg.LaneInfo.LaneAppVer)));
    //操作员工号
    memcpy(msg.LaneInfo.OperNo,QString::number(getLaneInfo()->getOper().dwOper).toLocal8Bit().data(),9);
    //操作员姓名
    tmpstr = getLaneInfo()->getOper().sOperName;
    //Utf8toGB2312(tmpstr.toLocal8Bit().data(),tmpstr.length());
    memcpy(msg.LaneInfo.OperName,Utf8toGB2312(tmpstr).data(), 10);
    //上班时间  YYYYMMDDhhmmss
    if(getLaneInfo()->getLogTime().isValid())
    {
        memcpy(msg.LaneInfo.LoginTime,getLaneInfo()->getLogTime().toString("yyyyMMddhhmmss").toLocal8Bit().data()
               ,sizeof(msg.LaneInfo.LoginTime));
    }
    //上班流水号
    msg.LaneInfo.LoginWasteSN = getLaneInfo()->getLogWaste();
    //逻辑日期   YYYYMMDD
    memcpy(msg.LaneInfo.Lchar,getLaneInfo()->getShiftInfo().iWorkDate.toString("yyyyMMdd").toLocal8Bit().data()
           ,sizeof(msg.LaneInfo.Lchar));
    //统计班次
    msg.LaneInfo.ShiftNo = getLaneInfo()->getShiftInfo().wShift;
    //操作员班组
    msg.LaneInfo.GroupID = 0;
    //车道状态
    msg.LaneInfo.LaneState = getLaneInfo()->getShiftState();
    pCardReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    nFileNum = 0;
    //参数版本号
    if(getLaneInfo()->getLaneType() != LaneType_MtcIn)
    {
        getParamMgr()->GetFileInfoStr(cfOrgCode,cfAllRate,sExtParaVer,nFileNum,false);
    }
    getParamMgr()->GetFileInfoStr(cfAreaCode,cfPSamWhite,sCommParaVer,nFileNum,false);
    getParamMgr()->GetFileInfoStr(cfLaneKey,sKeyVer,false);
    tmpstr = sExtParaVer+sCommParaVer+sKeyVer;
    memcpy(msg.LaneInfo.ParaVer,tmpstr.toLocal8Bit().data(),qMin(sizeof(msg.LaneInfo.ParaVer),(quint32)tmpstr.length()));
}
//填充车辆基本信息
void TollItem::FillVehInfo(TMsgToSend &msg)
{
    //除判车种
    msg.VehInfo.PVT = VehType2Byte(m_VehInfo.PVehType);
    //最终车种
    msg.VehInfo.VT = VehType2Byte(m_VehInfo.VehType);
    //初判车型
    msg.VehInfo.PVC = VehClass2Byte(m_VehInfo.PVehClass);
    //自动识别车型
    msg.VehInfo.AutoVC = VehClass2Byte(m_VehInfo.AutoVehClass);
    //最终车型
    msg.VehInfo.VC = VehClass2Byte(m_VehInfo.VehClass);
//    //qDebug()<<"m_VehInfo.VehClass"<<m_VehInfo.VehClass;
    //自动识别车牌颜色
    msg.VehInfo.AutoVLPC = m_VehInfo.nAutoVehPlateColor;
    memcpy(msg.VehInfo.AutoVLP,m_VehInfo.szAutoVehPlate,qMin(sizeof(m_VehInfo.szAutoVehPlate),sizeof(msg.VehInfo.AutoVLP)));
    //最终使用车牌颜色
    msg.VehInfo.VLPC = m_VehInfo.nVehPlateColor;
    //最终使用车牌
    memcpy(msg.VehInfo.VLP,m_VehInfo.szVehPlate,qMin(sizeof(m_VehInfo.szVehPlate),sizeof(msg.VehInfo.VLP)));
    //过车数量
    msg.VehInfo.VCnt = 0;
    //感应车流
    msg.VehInfo.InductCnt = 0;
    //车速（公里/小时）
    msg.VehInfo.VSpeed = m_VehInfo.dwVSpeed;
}
//填充卡基本信息
void TollItem::FillICCardInfo(TMsgToSend &msg)
{
    memset(&msg.ICCInfo,0,sizeof(msg.ICCInfo));
    msg.ICCInfo.CardType = m_nCardType;
    switch(FPhysicalCardType)
    {
    case TYPE_NULL:
        break;
    case TYPE_M1_S50_BLOCK:
    case TYPE_M1_S50_MAD:
        IntToArChar(msg.ICCInfo.CardNet,sizeof(msg.ICCInfo.CardNet),FTollCardBasicInfo.M1BasicInfo.wNetworkId);
        IntToArChar(msg.ICCInfo.CardID,9,FTollCardBasicInfo.M1BasicInfo.dwCardNum);
        msg.ICCInfo.CardSN = FTollCardBasicInfo.M1BasicInfo.dwCardId;
        msg.ICCInfo.CardCnt = 1;
        break;
    case TYPE_PRO:
        if(!m_ProCardBasicInfo.bOldCard)
        {
            IntToArChar(msg.ICCInfo.CardNet,sizeof(msg.ICCInfo.CardNet),m_ProCardBasicInfo.wNetworkId);
        }
        memcpy(msg.ICCInfo.CardID,m_ProCardBasicInfo.szCardNo,16);
        msg.ICCInfo.CardSN = m_ProCardBasicInfo.dwCardId;
        msg.ICCInfo.CardBal = FdwBalance;
        msg.ICCInfo.CardCnt = 1;
        break;
    default:
        break;
    }
}
//填充支付卡信息
void TollItem::FillPayCardInfo(TMsgToSend &msg)
{
    memset(&msg.PayCardInfo,0,sizeof(msg.PayCardInfo));
    msg.PayCardInfo.PayCardVLPC = -1;
    msg.PayCardInfo.PayCardType = 0xFF;
    if(CARD_TYPE_STORE_CARD != m_ProCardBasicInfo.bType &&CARD_TYPE_TALLY_CARD != m_ProCardBasicInfo.bType
            &&CARD_TYPE_ALLFREE_CARD != m_ProCardBasicInfo.bType&&CARD_TYPE_ALL_BAOJIAO_CARD != m_ProCardBasicInfo.bType
            &&CARD_TYPE_ROAD_FREE_STORE != m_ProCardBasicInfo.bType&&CARD_TYPE_ROAD_BAOJIAO_STORE != m_ProCardBasicInfo.bType
            &&CARD_TYPE_ROAD_FREE_TALLY != m_ProCardBasicInfo.bType&&CARD_TYPE_ROAD_BAOJIAO_TALLY != m_ProCardBasicInfo.bType)
    {
        return;
    }
    if(FPhysicalCardType != TYPE_PRO)
    {
        return;
    }
    //支付卡类型
    memcpy(msg.PayCardInfo.PayCardID,"0000",4);
    msg.PayCardInfo.PayCardType = m_ProCardBasicInfo.bType;
    if(!m_ProCardBasicInfo.bOldCard)
    {
        //支付卡网络号
        IntToArChar(msg.PayCardInfo.PayCardNet,4,m_ProCardBasicInfo.wNetworkId);
        //支付卡卡号中前4位是网络号
        IntToArChar(msg.PayCardInfo.PayCardID,4,m_ProCardBasicInfo.wNetworkId%10000);
    }
    //支付卡卡号(卡号填后16位)
    memcpy(&msg.PayCardInfo.PayCardID[4],m_ProCardBasicInfo.szCardNo,16);
    //支付卡物理序号
    msg.PayCardInfo.PayCardSN = m_ProCardBasicInfo.dwCardId;
    //支付卡用户类型
    msg.PayCardInfo.PayCardUserType = m_ProCardBasicInfo.bUserType;
    //支付卡交易序列号
    msg.PayCardInfo.PayCardTranSN = 0;
    //PSam交易序列号
    msg.PayCardInfo.PSAMTradeSN = 0;
    //卡内车牌
    memcpy(msg.PayCardInfo.PayCardVLP,m_ProCardBasicInfo.szVehPlate,sizeof(msg.PayCardInfo.PayCardVLP));
    //卡内车牌颜色
    msg.PayCardInfo.PayCardVLPC = m_ProCardBasicInfo.bVehPlateColor;
    //卡内车型
    msg.PayCardInfo.PayCardVehClass = m_ProCardBasicInfo.bVehClass;
    //Tac码
    msg.PayCardInfo.TAC = 0;
    //PSAM终端机编号
    memcpy(msg.PayCardInfo.PSamTerm,"",sizeof(msg.PayCardInfo.PSamTerm));
    //支付前余额
    msg.PayCardInfo.PayCardBalBefore = FdwBalance;
    //支付卡余额（分）
    msg.PayCardInfo.PayCardBalAfter = FdwBalance;
    // 交易类型
    msg.PayCardInfo.TransType = 0;
    //密钥版本
    msg.PayCardInfo.KeyVer = 0;
}
//填充OBU信息
void TollItem::FillOBUInfo(TMsgToSend &msg)
{
    memset(&msg.OBUInfo,0,sizeof(msg.OBUInfo));
}
//填充卡箱信息
void TollItem::FillCardBoxInfo(TMsgToSend &msg)
{
    memset(&msg.CardBoxInfo,0,sizeof(msg.CardBoxInfo));
    //卡盒编号
    QString tempID = QString::number(getDataFileMgr()->getCardBoxInfo().CardBoxID);
    memcpy(msg.CardBoxInfo.CardBoxID,tempID.toLocal8Bit().data(),qMin(tempID.length(),(int)sizeof(msg.CardBoxInfo.CardBoxID)));
    //盒内卡数
    msg.CardBoxInfo.CardCount = getDataFileMgr()->getCardBoxInfo().nCardCount;
}
//按键信息
void TollItem::FillKeyInfo(TMsgToSend &msg)
{
    memset(&msg.KeyInfo,0,sizeof(msg.KeyInfo));
    //按键信息
    int tmpkey = 99;
    MtcKeyProcessResultRecorder::instance()->getAndEraseKeyPressHistory(msg.KeyInfo.KeyValue,tmpkey);
    msg.KeyInfo.KeyNum=(quint16)tmpkey;
    LogMsg("lane",tr("[FillKeyInfo]按键个数:%1长度:%2").arg(msg.KeyInfo.KeyNum).arg(strlen(msg.KeyInfo.KeyValue)));
}
//填充交易状态信息
void TollItem::FillDealInfo(TMsgToSend &msg)
{
    memset(msg.DealInfo.SpInfo,'0',sizeof(msg.DealInfo.SpInfo));
    // 3.设备状态
    memcpy(msg.DealInfo.DevInfo,FDevStatus,sizeof(msg.DealInfo.DevInfo));
    //闯关被拦
    if(m_nViolateHeldBack == LST_TYPE_VIOLATE_HELDBACK)
    {
        msg.DealInfo.SpInfo[DS_SECG6] = '1';
    }
    //锁杆
    if(getDeviceFactory()->getIOBoard()->IsLockBar())
    {
        msg.DealInfo.SpInfo[DS_LOCK] = '1';
    }

    //车辆灰名单
    if(m_bVehGray)
    {
        msg.DealInfo.SpInfo[DS_VEHG] = '1';
    }
    //车辆黑名单
    if(m_bVehBlack)
    {
        msg.DealInfo.SpInfo[DS_VEHB] = '1';
    }
}
//授权信息 填充授权信息
void TollItem::FillAuthInfo(TMsgToSend &msg)
{
    memset(&msg.AuthInfo,0,sizeof(msg.AuthInfo));
    //授权类型
    msg.AuthInfo.AuthType = FAuthInfo.AuthType;
    //授权工号
    memcpy(msg.AuthInfo.AuthID,FAuthInfo.AuthID,sizeof(msg.AuthInfo.AuthID));
    //授权姓名
    memcpy(msg.AuthInfo.AuthName,FAuthInfo.AuthName,sizeof(msg.AuthInfo.AuthName));
    //授权时间
    DateTimeToChar14(FAuthInfo.AuthTime, msg.AuthInfo.AuthTime);
}
//计重信息
void TollItem::FillWeightInfo(TMsgToSend &msg)
{
    memset(&msg.WeightInfo,0,sizeof(msg.WeightInfo));
}
//填充图片信息
void TollItem::FillImageInfo(TMsgToSend &msg, int nPicType)
{
    if(nPicType == 0)
    {
        //抓拍图像时间
        memcpy(msg.CapImage.PhotoTime,FCaptureTime.toString("yyyyMMddhhmmss").toLocal8Bit().data()
               ,sizeof(msg.CapImage.PhotoTime));
        //抓拍图像
        strncpy(msg.CapImage.ImageFileName,FCapImageFile.toLocal8Bit().data(),sizeof(msg.CapImage));
        LogMsg("lane",tr("[FillImageInfo]抓拍图像：[%1]").arg(QString::fromLocal8Bit(msg.CapImage.ImageFileName)));
    }
    else
    {
        //车牌抓拍时间
        memcpy(msg.VLPImage.PhotoTime,FVLPTime.toString("yyyyMMddhhmmss").toLocal8Bit().data(),sizeof(msg.VLPImage.PhotoTime));
        //车牌图像
        strncpy(msg.VLPImage.ImageFileName,FVLPImageFile.toLocal8Bit().data(),sizeof(msg.VLPImage.ImageFileName));
        LogMsg("lane",tr("[FillImageInfo]车牌图像：[%1]").arg(QString::fromLocal8Bit(msg.VLPImage.ImageFileName)));
    }
}
//填充冲证报文
void TollItem::FillHedgeMsg(TMsgToSend &msg)
{
    ClearSendMsg(m_HdgMsgToSend);
    m_HdgMsgToSend = msg;
    //车流量
    m_HdgMsgToSend.VehInfo.VCnt = -m_HdgMsgToSend.VehInfo.VCnt;
    m_HdgMsgToSend.VehInfo.InductCnt = -m_HdgMsgToSend.VehInfo.InductCnt;
    //通行费
    m_HdgMsgToSend.TollInfo.TollMoney = -m_HdgMsgToSend.TollInfo.TollMoney;
    m_HdgMsgToSend.TollInfo.CardCost = -m_HdgMsgToSend.TollInfo.CardCost;
    m_HdgMsgToSend.TollInfo.OverWeightMoney = -m_HdgMsgToSend.TollInfo.OverWeightMoney;
    m_HdgMsgToSend.TollInfo.FreeMoney = - m_HdgMsgToSend.TollInfo.FreeMoney;
    m_HdgMsgToSend.TollInfo.RebateMoney = - m_HdgMsgToSend.TollInfo.RebateMoney;
    m_HdgMsgToSend.TollInfo.CashMoney = - m_HdgMsgToSend.TollInfo.CashMoney;
    m_HdgMsgToSend.TollInfo.CashCardMoney = - m_HdgMsgToSend.TollInfo.CashCardMoney;
    m_HdgMsgToSend.TollInfo.LastMoney = - m_HdgMsgToSend.TollInfo.LastMoney;
    m_HdgMsgToSend.TollInfo.LastCardMoney = - m_HdgMsgToSend.TollInfo.LastCardMoney;
    //打印票据数规定为0
    m_HdgMsgToSend.InvPrintInfo.InvCnt = 0;
    sprintf(m_HdgMsgToSend.BasicInfo.msgHead.cMsgType,"%d",LST_TYPE_HEDGE);
}
//填充单一位元
void TollItem::FillSingleMsg(int nPos, TMsgToSend &Msg, bool bPeekSN)
{
    switch(nPos)
    {
    case 1:
        FillBasicInfo(QDateTime::currentDateTime(),Msg,bPeekSN);
        break;
    case 2:
        FillVehInfo(Msg);
        break;
    case 3:
        FillICCardInfo(Msg);
        break;
    case 4:
        FillPayCardInfo(Msg);
        break;
    case 5:
        FillOBUInfo(Msg);
        break;
    case 6:
        FillCardBoxInfo(Msg);
        break;
    case 7:
        FillKeyInfo(Msg);
        break;
    case 8:
        FillDealInfo(Msg);
        break;
    case 9:
        FillAuthInfo(Msg);
        break;

    case 31:
        FillImageInfo(Msg,0);
        break;
    case 32:
        FillImageInfo(Msg,1);
        break;
    }
}
//根据位元表填MsgToSend各位元
void TollItem::FillMsgToSend(int MsgID, int MsgType, TMsgToSend &Msg, bool bPeekSN)
{
    char WeiYuan[4];
    int i;
    getMsgSendFun()->GetWeiYuan(MsgID,WeiYuan);
    sprintf(Msg.BasicInfo.msgHead.cMsgID,"%2d",MsgID);
    sprintf(Msg.BasicInfo.msgHead.cMsgType,"%2d",MsgType);
    for(i = 1;i<=32;i++)
    {
        if(getMsgSendFun()->Testbyte(WeiYuan,i))
        {
            FillSingleMsg(i,Msg,bPeekSN);
        }
    }
}

//将MsgToSend 按位元压缩后存入数据库
bool TollItem::SaveMsg(int nSendType, int MsgID, int MsgType, TMsgToSend &msg, bool RemoveTmpData, void *PSynData, int nSynDataLen)
{
    TDataToSave DataToSave;
    quint64 DataID;
    bool result;
    QSysParaDicParamFile *sysParam;
    QSysParamInfo SysInfo;
    sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    SysInfo = sysParam->GetSysParamInfo();
    int nLenMsg;
    getMsgSendFun()->GetWeiYuan(MsgID,msg.BasicInfo.wy);
    sprintf(msg.BasicInfo.msgHead.cMsgID,"%02d",MsgID);
    sprintf(msg.BasicInfo.msgHead.cMsgType,"%02d",MsgType);
    //发送机构
    IntToArChar(msg.BasicInfo.msgHead.Sender,sizeof(msg.BasicInfo.msgHead.Sender),SysInfo.m_nStationID*1000 + getLaneInfo()->getLaneCode());
    //接收机构
    IntToArChar(msg.BasicInfo.msgHead.Receiver,sizeof(msg.BasicInfo.msgHead.Receiver),SysInfo.m_nStationID*1000);
    sprintf(&msg.BasicInfo.msgHead.ZipType,"%d",0);
    msg.BasicInfo.WasteID = MsgID+MsgType;
    nLenMsg = sizeof(msg)-sizeof(msg.BasicInfo.msgHead);
    //计算MD5校验码
    nLenMsg = sizeof(msg)-sizeof(msg.BasicInfo.msgHead);
    QString tmpStr = GetMD5ForBuffer((char*)&msg.BasicInfo.WasteID,nLenMsg).toLocal8Bit().data();
    qMemCopy(msg.BasicInfo.msgHead.VerifyCode, tmpStr.toLocal8Bit().data(), tmpStr.length());
    //计算报文长度
    msg.BasicInfo.msgHead.TotalLength = getMsgSendFun()->GetTotalSize(msg);
    memset(&DataToSave,0,sizeof(DataToSave));
    DataToSave.OccurTime =  DateTimeToUnixTime1970(QDateTime::currentDateTime());
    DataToSave.MsgID = MsgID;
    DataToSave.MsgType = MsgType;
    DataToSave.pMsg = (char*)&msg;
    DataToSave.nMsgLen  = sizeof(msg);
    //将当前时间赋给上次发送的业务的时间
    if(SEND_SUPERVISOR_ONLY == nSendType)
    {
        getMon()->SendData(msg);
        result = true;
    }
    else
    {

        //压缩文件
        int zlibLength = 10240;
        compress((Bytef*)m_zlibBuf, (uLongf*)&zlibLength, (Bytef*)DataToSave.pMsg, DataToSave.nMsgLen);
        DataToSave.pMsg = m_zlibBuf;
        DataToSave.nMsgLen = zlibLength;
        getXMLBakupWriter()->bakup(DataToSave);
        result = getDataSaver()->SaveData(DataID,&DataToSave,RemoveTmpData,PSynData,nSynDataLen);
        if(SEND_BOTH == nSendType)
        {
            getMon()->SendData(msg);
        }
    }
    ////保存数据异常时，弹出错误提示

    if(!result)
    {
        LogMsg("Lane","硬盘故障无法继续操作，请立刻通知系统管理员！");
    }
    return result;
}

//保存防死机流水
bool TollItem::SaveTmpMsg(int MsgID, int MsgType, TMsgToSend &msg, void *PSynData, int nSynDataLen)
{
    TDataToSave DataToSave;
    bool result;
    int nLenMsg;
    QSysParaDicParamFile *sysParam;
    QSysParamInfo SysInfo;
    sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    SysInfo = sysParam->GetSysParamInfo();
    getMsgSendFun()->GetWeiYuan(MsgID,msg.BasicInfo.wy);
    sprintf(msg.BasicInfo.msgHead.cMsgID,"%d",MsgID);
    sprintf(msg.BasicInfo.msgHead.cMsgType,"%d",MsgType);
    //发送机构
    IntToArChar(msg.BasicInfo.msgHead.Sender,sizeof(msg.BasicInfo.msgHead.Sender),SysInfo.m_nStationID*1000 + getLaneInfo()->getLaneCode());
    //接收机构
    IntToArChar(msg.BasicInfo.msgHead.Receiver,sizeof(msg.BasicInfo.msgHead.Receiver),SysInfo.m_nStationID*1000);
    sprintf(&msg.BasicInfo.msgHead.ZipType,"%d",0);
    DateTimeToChar14(QDateTime::currentDateTime(),msg.BasicInfo.msgHead.SendTime);
    //计算MD5校验码
    nLenMsg = sizeof(msg)-sizeof(msg.BasicInfo.msgHead);
    QString tmpStr = GetMD5ForBuffer((char*)&msg.BasicInfo.WasteID,nLenMsg).toLocal8Bit().data();
    qMemCopy(msg.BasicInfo.msgHead.VerifyCode, tmpStr.toLocal8Bit().data(), tmpStr.length());
    msg.BasicInfo.WasteID = MsgID+MsgType;
    //计算报文长度
    msg.BasicInfo.msgHead.TotalLength = getMsgSendFun()->GetTotalSize(msg);
    memset(&DataToSave,0,sizeof(DataToSave));
    DataToSave.OccurTime =  DateTimeToUnixTime1970(QDateTime::currentDateTime());
    DataToSave.MsgID = MsgID;
    DataToSave.MsgType = MsgType;
    DataToSave.pMsg = (char*)&msg;
    DataToSave.nMsgLen  = sizeof(msg);
    result = getDataSaver()->SaveTempData(&DataToSave,PSynData,nSynDataLen);
    //保存数据异常时，弹出错误提示
    if(!result)
    {
        LogMsg("Lane","硬盘故障无法继续操作，请立刻通知系统管理员！");
    }
    return result;
}
//清空报文内容 清除保存的报文信息
void TollItem::ClearSendMsg(TMsgToSend &msg)
{
    memset(&msg,0,sizeof(msg));
    memset(msg.DealInfo.SpInfo,0,sizeof(msg.DealInfo.SpInfo));
    memset(msg.DealInfo.DevInfo,0,sizeof(msg.DealInfo.DevInfo));

    msg.PayCardInfo.PayCardVLPC = -1;
    msg.PayCardInfo.PayCardType = 0xFF;
    msg.ICCInfo.CardType = 0xFF;
    msg.VehInfo.AutoVLPC = -1;
    msg.VehInfo.VLPC = -1;
    msg.VehEnInfo.EnVLPC = -1;
}

void TollItem::setVTInputTime(const QDateTime &sTime)
{
    FVTInputTime = sTime;
}
//清除保存的时间
void TollItem::ClearSaveTime()
{

}
//清除上一辆车的备份报文
void TollItem::ClearLastSendMsg()
{
    ClearSendMsg(FLastMsgToSend);
}
//清除图像信息
void TollItem::ClearImageInfo()
{
    //抓拍时间
    //抓拍图像文件名
    FCapImageFile = "";
    //车牌图像抓拍时间
    //车牌图像文件名
    FVLPImageFile = "";
}
//清除卡片信息
void TollItem::ClearCardInfo()
{
    FPhysicalCardType = TYPE_NULL;
    FdwBalance = 0;
    m_nCardType = -1;
    FCardTollInfo.clear();
    FCardTollInfo.bVehPlateColor = 0xFF;
    m_ProCardBasicInfo.clear();
    FTollCardBasicInfo.bCRC = false;
    FTollCardBasicInfo.bOldCard = false;
    FTollCardBasicInfo.M1BasicInfo.clear();
    m_ProCardBasicInfo.bVehPlateColor = 0xFF;
}
//清除收费员输入的信息
void TollItem::ClearSavedInput()
{
    m_VehInfo.Clear();
}
//清除保存的信息
void TollItem::ClearSavedInfo()
{
    //清除保存的输入信息
    ClearSavedInput();
    //清除保存的时间
    ClearSaveTime();
    //清除卡片信息
    ClearCardInfo();
    //清空图像信息
    ClearImageInfo();
    //业务类型
    FnYwType = LST_TYPE_NORMAL;
    //违章标志
    m_nViolateHeldBack = 0;
    //车辆黑名单
    m_bVehBlack = false;
    //车辆灰名单
    m_bVehGray = false;
    //卡黑名单
    m_bCardBlack = false;
    //清除授权信息
    FAuthInfo.clear();
}
//设置计重信息
bool TollItem::SetVehAxisInfo()
{
    return  false;
}
//返回当前报文信息
TMsgToSend TollItem::GetMsgToSend()
{
    return FMsgToSend;
}

void TollItem::SetViolateHeldBack(int nBack)
{
    m_nViolateHeldBack = nBack;
}

void TollItem::SetOccurTime(const QDateTime &time)
{
    m_dOccTime = time;
}
//主界面显示业务流水
void TollItem::AddHistWaste(TMsgToSend &msg)
{
    QString sTemp;
    sTemp = QString::fromLocal8Bit(msg.LaneInfo.OccurTime,14);
    FHistRec.WasteTime = QDateTime::fromString(sTemp, "yyyyMMddhhmmss");
    if(!FHistRec.WasteTime.isValid())
    {
        LogMsg("lane", tr("时间转换错误"));
        return;
    }
    if(getPhysicalCardType() == TYPE_PRO)
    {
        sTemp = QString::fromLocal8Bit(msg.ICCInfo.CardID, qMin((int)strlen(msg.ICCInfo.CardID),16));
    }
    else
    {
        sTemp = msg.ICCInfo.CardID;
    }
    if(!sTemp.isNull())
    {
        FHistRec.CardID = QString::number(sTemp.toLongLong());
    }
    FHistRec.VehClass = GetVehClassName_LPCSTR((CVehClass)msg.VehInfo.VC);
    FHistRec.VehPlate = GetVehColorQString(msg.VehInfo.VLPC)+GB2312toUtf8(msg.VehInfo.VLP);
    FHistRec.EnStation = QString::fromLocal8Bit(msg.VehEnInfo.EnStation, 6);
    FHistRec.Money = msg.TollInfo.LastMoney+msg.TollInfo.LastCardMoney;
    FHistRec.PayCardBal = msg.PayCardInfo.PayCardBalAfter;
    int nFinal = getPhysicalCardType() == TYPE_PRO? (msg.PayCardInfo.PayCardBalBefore - FHistRec.PayCardBal): FHistRec.Money;
    //liujian
    //现金
    if(msg.TollInfo.PayType == 0)
    {
        nFinal = FHistRec.Money;
    }
    //非现金
    if(msg.TollInfo.PayType == 1)
    {
        nFinal = msg.PayCardInfo.PayCardBalBefore - FHistRec.PayCardBal;
    }
    //两者混用
    if(msg.TollInfo.PayType == 2)
    {
        nFinal = msg.PayCardInfo.PayCardBalBefore - FHistRec.PayCardBal + FHistRec.Money;
        LogMsg("lane",tr("启用现金非现金混合支付方式%1").arg(msg.TollInfo.PaySN));
    }
    //end
    FormRecordTable* pRecord = NULL;
    bool bRet = getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    if(bRet)
    {

        if(getLaneInfo()->isEntryLane())
        {
            pRecord->insertEntryInfo(FHistRec.WasteTime,FHistRec.CardID,
                                     FHistRec.VehClass,FHistRec.VehPlate);
        }
        else
        {
            //liujian
            pRecord->insertExitInfo(FHistRec.WasteTime, FHistRec.CardID,
                                    FHistRec.VehClass, FHistRec.VehPlate, FHistRec.EnStation,
                                    (msg.TollInfo.TollMoney + msg.TollInfo.CardCost)/100.0,
                                    nFinal / 100.0, FHistRec.PayCardBal / 100.0);
        }
    }
    FormVpr* pVpr = NULL;
    bRet = getWidgetByType(emFormVpr, (QWidget**)&pVpr);
    if(bRet)
    {
        pVpr->clearImage();
    }
}
//保存PRO卡基本信息
void TollItem::SetProCardInfo(TProCardBasicInfo &AProCardInfo)
{
    //Pro卡基本信息
    m_ProCardBasicInfo.bOldCard = AProCardInfo.bOldCard;
    m_ProCardBasicInfo.dwCardId = AProCardInfo.dwCardId;
    memcpy(m_ProCardBasicInfo.szIssueOrgId,AProCardInfo.szIssueOrgId,sizeof(m_ProCardBasicInfo.szIssueOrgId));
    m_ProCardBasicInfo.bType = AProCardInfo.bType;
    m_ProCardBasicInfo.bVersion = AProCardInfo.bVersion;
    m_ProCardBasicInfo.wNetworkId = AProCardInfo.wNetworkId;
    memcpy(m_ProCardBasicInfo.szCardNo,AProCardInfo.szCardNo,sizeof(m_ProCardBasicInfo.szCardNo));
    m_ProCardBasicInfo.StartTime = AProCardInfo.StartTime;
    m_ProCardBasicInfo.ExpireTime = AProCardInfo.ExpireTime;
    memcpy(m_ProCardBasicInfo.szVehPlate,AProCardInfo.szVehPlate,sizeof(m_ProCardBasicInfo.szVehPlate));
    m_ProCardBasicInfo.bUserType = AProCardInfo.bUserType;
    m_ProCardBasicInfo.bVehPlateColor = AProCardInfo.bVehPlateColor;
    m_ProCardBasicInfo.bVehClass = AProCardInfo.bVehClass;
    m_ProCardBasicInfo.bVehTollType = AProCardInfo.bVehTollType;
    m_ProCardBasicInfo.bDiscount = AProCardInfo.bDiscount;
    m_ProCardBasicInfo.bFreeRegionType = AProCardInfo.bFreeRegionType;
    m_ProCardBasicInfo.wFreeRegionNum = AProCardInfo.wFreeRegionNum;
    memcpy(m_ProCardBasicInfo.FreeRegion,AProCardInfo.FreeRegion,sizeof(m_ProCardBasicInfo.FreeRegion));
    setCardType(AProCardInfo.bType);
}
//保存卡中的通行信息
void TollItem::SetCardTollInfo(TCardTollInfo &ACardTollInfo)
{
    FCardTollInfo.wNetworkId = ACardTollInfo.wNetworkId;
    FCardTollInfo.dwStaId = ACardTollInfo.dwStaId;
    FCardTollInfo.bLaneId =ACardTollInfo.bLaneId;
    FCardTollInfo.bPassStatus = ACardTollInfo.bPassStatus;
    FCardTollInfo.bMaintenance = ACardTollInfo.bMaintenance;
    FCardTollInfo.PassTime = ACardTollInfo.PassTime;
    FCardTollInfo.bRecIndex = ACardTollInfo.bRecIndex;
    FCardTollInfo.dwOpId = ACardTollInfo.dwOpId;
    FCardTollInfo.bShiftId = ACardTollInfo.bShiftId;
    FCardTollInfo.bVehClass = ACardTollInfo.bVehClass;
    FCardTollInfo.bVehTollType = ACardTollInfo.bVehTollType;
    memcpy(FCardTollInfo.baPsamTermId,ACardTollInfo.baPsamTermId,sizeof(FCardTollInfo.baPsamTermId));
    FCardTollInfo.dwWriteCount = ACardTollInfo.dwWriteCount;
    FCardTollInfo.dwCardBoxId = ACardTollInfo.dwCardBoxId;
    memcpy(FCardTollInfo.szVehPlate,ACardTollInfo.szVehPlate,sizeof(FCardTollInfo.szVehPlate));
    FCardTollInfo.bVehPlateColor = ACardTollInfo.bVehPlateColor;
    FCardTollInfo.bFlagStaCount = ACardTollInfo.bFlagStaCount;
    memcpy(FCardTollInfo.baFlagStaInfo,ACardTollInfo.baFlagStaInfo,sizeof(FCardTollInfo.baFlagStaInfo));
    FCardTollInfo.bCRC = ACardTollInfo.bCRC;
}
//设置授权信息
void TollItem::SetAuthInfo(TAuthorizeInfo &AuthInfo)
{
    memcpy(FAuthInfo.AuthID, AuthInfo.AuthID, sizeof(AuthInfo.AuthID));
    memcpy(FAuthInfo.AuthName, AuthInfo.AuthName, sizeof(AuthInfo.AuthName));
    FAuthInfo.AuthType = AuthInfo.AuthType;
    FAuthInfo.AuthTime = AuthInfo.AuthTime;
    LogMsg("lane",tr("'授权类型%1,授权工号%2,授权姓名%3").arg(FAuthInfo.AuthType).arg(FAuthInfo.AuthID).arg(FAuthInfo.AuthName));
}
//保存M1卡基本信息
void TollItem::SetTollCardInfo(TTollCardBasicInfo &ATollCardInfo)
{ 
    FTollCardBasicInfo.M1BasicInfo.dwCardId = ATollCardInfo.M1BasicInfo.dwCardId;
    FTollCardBasicInfo.M1BasicInfo.bVersion = ATollCardInfo.M1BasicInfo.bVersion;
    FTollCardBasicInfo.M1BasicInfo.wNetworkId = ATollCardInfo.M1BasicInfo.wNetworkId;
    FTollCardBasicInfo.M1BasicInfo.bType = ATollCardInfo.M1BasicInfo.bType;
    FTollCardBasicInfo.M1BasicInfo.dwCardNum = ATollCardInfo.M1BasicInfo.dwCardNum;
    FTollCardBasicInfo.M1BasicInfo.StartTime = ATollCardInfo.M1BasicInfo.StartTime;
    FTollCardBasicInfo.M1BasicInfo.ExpireTime = ATollCardInfo.M1BasicInfo.ExpireTime;
    FTollCardBasicInfo.M1BasicInfo.bIssueKeyVer = ATollCardInfo.M1BasicInfo.bIssueKeyVer;
    FTollCardBasicInfo.M1BasicInfo.szIssueSAM = ATollCardInfo.M1BasicInfo.szIssueSAM;
    FTollCardBasicInfo.M1BasicInfo.dwIssueOrg = ATollCardInfo.M1BasicInfo.dwIssueOrg;
    FTollCardBasicInfo.M1BasicInfo.dwIssueOp = ATollCardInfo.M1BasicInfo.dwIssueOp;
    FTollCardBasicInfo.bCRC = ATollCardInfo.bCRC;
    setCardType(ATollCardInfo.M1BasicInfo.bType);

}
//设置设备状态
void TollItem::SetDevStatus(const TDevType ADevType, char AStatus)
{
    //liujian 出口没有自动卡机
    FDevStatus[DT_CARDMGR1] = DV_OK;
    FDevStatus[(int)ADevType] = AStatus;
    m_dOccTime = QDateTime::currentDateTime();
    SendLaneEvent(LET_DevChange);
}
//保存设置设备当前状态变化
void TollItem::SaveDevStatus(int nType, char bStatus)
{
    char DevStatus;
    if (bStatus == 0)
    {
        DevStatus = DV_OK;
    }
    else
    {
        DevStatus = DV_FAILURE;
    }
    switch(nType)
    {
    case DEV_CardReader1:
        FDevStatus[DT_CARDREADER] = DevStatus;
        break;
    case DEV_CardReader2:
        FDevStatus[DT_CARDREADER1] = DevStatus;
        break;
    case DEV_CardReader3:
        FDevStatus[DT_CARDREADER2] = DevStatus;
        break;
    case DEV_IOCard:
        FDevStatus[DT_CCU] = DevStatus;
        break;
    case DEV_SmallPrinter:
        FDevStatus[DT_PRINTER] = DevStatus;
        break;
    case DEV_CardManageMachine:
        FDevStatus[DT_AUTOCARDMGR] = DevStatus;
        break;
    case   DEV_VideoCard:
        FDevStatus[DT_VideoCard] = DevStatus;
        break;
    case  DEV_VehPlateRecognizer:
        FDevStatus[DT_PLATERECOG] = DevStatus;
        break;
    case DEV_FareDisplayer:
        FDevStatus[DT_FEEDISPLAY] = DevStatus;
        break;
    case  DEV_VDM :
        FDevStatus[DT_ADDCHAR] = DevStatus;
        break;
    }
    //发送车道事件报文 设备变化
    //liujian 出口没有自动卡机
    FDevStatus[DT_CARDMGR1] = DV_NONE;
    m_dOccTime = QDateTime::currentDateTime();
    SendLaneEvent(LET_DevChange);
}
//保存IO设备当前状态
void TollItem::SetIOStatus(int nType, int IoIndex, char bStatus)
{
    char DevStatus;

    if(bStatus == 0)
    {
        DevStatus = DV_FAILURE;
    }
    else
    {
        DevStatus = DV_OK;
    }
    if(nType == 0)
    {
        switch(DIGITAL_IN(IoIndex))
        {
        case DI_LoopTrig1:
            FDevStatus[DT_TRIGLOOP1] = DevStatus;
            break;
        case DI_LoopTrig2:
            FDevStatus[DT_TRIGLOOP2] = DevStatus;
            break;
        case DI_LoopFront:
            FDevStatus[DT_FRONTLOOP] = DevStatus;
            break;
        case DI_LoopBack:
            FDevStatus[DT_BACKLOOP] = DevStatus;
            break;
        case DI_BarrierDown:
            if(!getDeviceFactory()->getIOBoard()->GetDIPortStatus(DI_BarrierDown))
                break;
        case DI_BarrierUp:
            if(!getDeviceFactory()->getIOBoard()->GetDIPortStatus(DI_BarrierUp))
                break;
        case DI_AlarmDetector:
            FDevStatus[DT_ALARMDETECTOR] = DevStatus;
            break;
        default:
            break;
        }
    }
    else if(nType == 1)
    {
        switch(DIGITAL_OUT(IoIndex))
        {
        case DO_CanopyLightGreen:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_CanopyLightGreen))
                break;
            else
            {
                FDevStatus[DT_CANOPYLIGHT] = DV_OK;
                break;
            }
        case DO_CanopyLightRed:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_CanopyLightRed))
                break;
            else
            {
                FDevStatus[DT_CANOPYLIGHT] = DV_FAILURE;
                break;
            }
        case DO_PassLightGreen:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_PassLightGreen))
                break;
            else
            {
                FDevStatus[DT_PASSLIGHT] = DV_OK;
                break;
            }
        case DO_PassLightRed:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_PassLightRed))
                break;
            else
            {
                FDevStatus[DT_PASSLIGHT] = DV_FAILURE;
                break;
            }
        case DO_BarrierDown:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_BarrierDown))
                break;
            else
            {
                FDevStatus[DT_AUTOBAR] = DV_FAILURE;
                break;
            }
        case DO_BarrierUp:
            if(!getDeviceFactory()->getIOBoard()->GetDOPortStatus(DO_BarrierUp))
                break;
            else
            {
                FDevStatus[DT_AUTOBAR] = DV_OK;
                break;
            }
        case DO_AlarmLight:
            FDevStatus[DT_ALARMLIGHT] = DevStatus;
            break;
        case DO_AlarmSound:
            FDevStatus[DT_ALARMSOUND] = DevStatus;
            break;
        default:
            break;
        }
    }
    //liujian 出口没有自动卡机
    FDevStatus[DT_CARDMGR1] = DV_NONE;
    m_dOccTime = QDateTime::currentDateTime();
    SendLaneEvent(LET_DevChange);
}
//根据抓拍时间等，返回图像应该保存的相对目录和文件名（文件名根据图像时间及类型生成）
QString TollItem::FormatPicFileName(const QDateTime& CapTime, int nPicType, QString &sFileName)
{
    QString sRaletivePath;
    sFileName = tr("%1_%2.jpg").arg(CapTime.toString("yyMMddhhmmss")).arg(nPicType);
    sRaletivePath = CapTime.toString("MMdd");
    return sRaletivePath;
}
//保存图像信息
void TollItem::SetPicPath(QString szPicPath, const QDateTime& CapTime, int nPicType)
{
    if(nPicType == 0)
    {
        FCapImageFile = szPicPath;
        FCaptureTime = CapTime;
    }
    else
    {
        FVLPImageFile = szPicPath;
        FVLPTime = CapTime;
    }
}
//大工班切换
void TollItem::NotifyWorkShiftChange()
{
    if(getShiftSum()->HaveBigShiftSumInfo())
    {
        SendBigShiftSum();
    }
    getShiftSum()->StartNewBigShift(getLaneInfo()->getShiftInfo().wShift,getLaneInfo()->getShiftInfo().iWorkDate,QDateTime::currentDateTime());
}
//获取上次存报文时间
QDateTime TollItem::GetLastSendTime()
{
    return FLastSendTime;
}

void TollItem::Complete(const QDateTime& OccutTime, int nAllowPassCount, bool bFinished)
{
    TMsgShiftSum MsgShiftSum;
    int MsgID,MsgType;
    ClearSendMsg(FMsgToSend);
    FillYwWaste(OccutTime);

    FnYwType = LST_TYPE_NORMAL;
    if(m_nViolateHeldBack == LST_TYPE_VIOLATE_LOGOUT || m_nViolateHeldBack == LST_TYPE_VIOLATE)
    {
        FMsgToSend.DealInfo.SpInfo[DS_SECG1] = '1';
    }
    if(nAllowPassCount == 0 || !bFinished)
    {
        FMsgToSend.VehInfo.VCnt = 0;
    }

    //添加小班汇总
    getShiftSum()->Sum(FMsgToSend);
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    MsgID = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgID,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgID));
    MsgType = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgType,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgType));

    if(!SaveTmpMsg(MsgID,MsgType,FMsgToSend,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("lane","临时数据保存失败");
    }
    //保存上次发送的数据
    AddHistWaste(FMsgToSend);
    static int i =1;
    //对于重新处理或分段付款等操作立即发送报文
    if(nAllowPassCount == 0 || !bFinished)
    {
        SendYewuWasteMsg(0);
    }
}
//********** 报文发送相关函数 **************
void TollItem::SendStartMsg()
{
    int BigShiftNo;
    //如未正常下班程序关闭，程序启动后补发下班和小班汇总报文
    if(getShiftSum()->HaveSmallShiftSumInfo())
    {
        getShiftSum()->SaveCardBox();
        m_dOccTime = QDateTime::currentDateTime();
        SendLogoutMsg(getShiftSum()->GetSmallShiftSum());
    }
    //如系统保存的工班与现有工班不同，发送工班合计数
    BigShiftNo = QString::fromLocal8Bit(getShiftSum()->GetBigShiftSum().ShiftID,2).toInt();
    QDate time = QDate::fromString(QString::fromLocal8Bit(getShiftSum()->GetBigShiftSum().Lchar, 8), "yyyyMMdd");
    bool ret = getShiftSum()->IsBigShiftSame(BigShiftNo,time);
    if(!getShiftSum()->HaveBigShiftSumInfo())
    {
        getLaneInfo()->getShiftInfo().iWorkDate = time;
        getLaneInfo()->getShiftInfo().wShift = BigShiftNo;
        //XK:这里由于在启动之前已经确定设置了大工班，因此不再设置
    }
    else
        if(!ret)
        {
            NotifyWorkShiftChange();
        }
    m_dOccTime = QDateTime::currentDateTime();
    SendAppMsg(true);
    LogMsg("lane",tr("报文保存到数据库SendStartMsg完成"));
}
//发送程序启动/关闭报文
void TollItem::SendAppMsg(bool bAppStart)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 4;
    if(bAppStart)
    {
        //启动  报文类型
        MsgType = 1;
    }
    else
    {
        //关闭  报文类型
        MsgType = 2;
    }
    //根据位元表填MsgToSend各位元
    FillMsgToSend(MsgID,MsgType,msg);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        if(bAppStart)
        {
            LogMsg("lane", tr("发送程序启动报文失败"));
        }
        else
        {
            LogMsg("lane", tr("发送程序关闭报文失败"));
        }
    }
    LogMsg("lane",tr("报文保存到数据库SendAppMsg完成"));
}
//发送上班报文
void TollItem::SendLoginMsg()
{
    TMsgToSend msg;
    TMsgShiftSum ShiftSum;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 3;
    MsgType = 1;
    FillMsgToSend(MsgID,MsgType,msg,true);
    getShiftSum()->SaveCardBox();
    getShiftSum()->StartNewSmallShift(msg);
    getShiftSum()->GetSmallShiftInfo(ShiftSum);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg,true,&ShiftSum,sizeof(ShiftSum)))
    {
        LogMsg("lane", tr("保存上班报文失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendLoginMsg完成"));
}
//发送下班报文(同时发送小班汇总) overload;
void TollItem::SendLogoutMsg()
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 3;
    MsgType = 2;
    m_dOccTime = QDateTime::currentDateTime();
    FillMsgToSend(MsgID,MsgType,msg,true);
    msg.LaneInfo.WasteSN = getLaneInfo()->ReadTransID() - 1;
    msg.LaneInfo.LaneState = lsUnlogin;
    if (!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        LogMsg("lane", tr("保存下班报文失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendLogoutMsg完成"));

    //发送小工班汇总数据
    SendSmallShiftSum(msg.LaneInfo.OccurTime);
}
//补发下班报文 overload;
void TollItem::SendLogoutMsg(const TMsgShiftSum& ShiftSum)
{
    TMsgToSend msg;
    QString tmpStr;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 3;
    MsgType =2;
    FillMsgToSend(MsgID,MsgType,msg,true);
    msg.LaneInfo.WasteSN = getLaneInfo()->ReadTransID() - 1;
    msg.LaneInfo.LaneState = lsUnlogin;
    //根据班次汇总信息修改上班信息
    tmpStr = ShiftSum.ShiftID;
    msg.LaneInfo.ShiftNo = tmpStr.toInt();
    memcpy(msg.LaneInfo.Lchar,ShiftSum.Lchar,sizeof(msg.LaneInfo.Lchar));
    memcpy(msg.LaneInfo.LoginTime,ShiftSum.StartTime,sizeof(msg.LaneInfo.LoginTime));
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        LogMsg("lane", tr("保存补发下班报文失败"));
    }
    LogMsg("lane",tr("报文保存到数据库补发SendLogoutMsg完成"));
    SendSmallShiftSum(msg.LaneInfo.OccurTime);
}
//发送小班合计
void TollItem::SendSmallShiftSum(const char sEndTime[])
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 5;
    MsgType = 0;
    switch(getLaneInfo()->getLaneType())
    {

    case LaneType_MtcIn:
    case LaneType_EtcIn:
        MsgType = 1;
        break;
    case LaneType_MtcOut:
    case LaneType_EtcOut:
        MsgType = 2;
        break;
    default:
        break;
    }
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime,msg,true);
    msg.LaneInfo.LaneState = lsUnlogin;
    getShiftSum()->SetSmallShiftEndTime(sEndTime);
    getShiftSum()->GetSmallShiftInfo(msg.ShiftSum);
    getShiftSum()->GetShiftCardBoxInfo(msg.CardBoxInfo);

    msg.LaneInfo.WasteSN = getLaneInfo()->ReadTransID() -1;
    if(!SaveMsg(SEND_DATABASE_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("Lane","保存小班汇总报文失败");
    }
    LogMsg("lane",tr("报文保存到数据库SendSmallShiftSum完成"));
    //清除小工班汇总
    getShiftSum()->ClearSmallShiftSum();
}
//发送大班合计
void TollItem::SendBigShiftSum()
{
    TMsgToSend msg;
    int MsgID,MsgType;
    QString tmpStr;
    ClearSendMsg(msg);
    MsgID = 5;
    MsgType = 0;
    switch(getLaneInfo()->getLaneType())
    {
    case LaneType_MtcIn:
    case LaneType_EtcIn:
        MsgType = 3;
        break;
    case LaneType_MtcOut:
    case LaneType_EtcOut:
        MsgType = 4;
        break;
    }

    m_dOccTime =  QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime,msg,true);
    memset(&msg.LaneInfo.OperNo,0,sizeof(msg.LaneInfo.OperNo));
    memset(&msg.LaneInfo.OperName,0,sizeof(msg.LaneInfo.OperNo));
    memset(&msg.LaneInfo.LoginTime,0,sizeof(msg.LaneInfo.LoginTime));
    msg.LaneInfo.LoginWasteSN = 0;
    getShiftSum()->SetBigShiftEndTime(m_dOccTime);
    getShiftSum()->GetBigShiftInfo(msg.ShiftSum);
    tmpStr = GB2312toUtf8(msg.ShiftSum.Lchar) + "-" +GB2312toUtf8(msg.ShiftSum.ShiftID);
    msg.LaneInfo.WasteSN = getLaneInfo()->ReadTransID();
    if(!SaveMsg(SEND_DATABASE_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("Lane","保大班汇总报文失败");
    }
    LogMsg("lane",tr("报文保存到数据库SendBigShiftSum完成"));
    //清除大工班汇总数据
    getShiftSum()->ClearBigShiftSum();
}
//发送冲减报文  入口倒车   出口冲减
void TollItem::SendHedgeMsg()
{
    int MsgID,MsgType;
    TMsgShiftSum MsgShiftSum;
    switch(getLaneInfo()->getLaneType())
    {
    case LaneType_MtcIn:
    case LaneType_EtcIn:
        MsgID = 1;
        break;
    case LaneType_MtcOut:
    case LaneType_EtcOut:
        MsgID = 2;
        break;
    default:
        LogMsg("lane", tr("车道类型错误:%1").arg(getLaneInfo()->getLaneType()));
        break;
    }
    MsgType = LST_TYPE_HEDGE;
    m_HdgMsgToSend.LaneInfo.WasteSN = getLaneInfo()->GetTransID();
    //添加小班汇总
    getShiftSum()->Sum(m_HdgMsgToSend);
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,m_HdgMsgToSend,true,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("Lane","保存冲减报文失败");
    }
    LogMsg("lane",tr("报文保存到数据库SendHedgeMsg完成"));
}
//发送车队过车报文
void TollItem::SendMotorCarPassMsg(int nInductCount)
{
    TMsgToSend msg;
    TMsgShiftSum MsgShiftSum;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    switch(getLaneInfo()->getLaneType())
    {
    case LaneType_MtcIn:
    case LaneType_EtcIn:
        MsgID = 1;
        break;
    case LaneType_MtcOut:
    case LaneType_EtcOut:
        MsgID = 2;
        break;
    default:
        LogMsg("lane", tr("车道类型错误:%1").arg(getLaneInfo()->getLaneType()));
    }
    MsgType = LST_TYPE_MOTORPASS;
    //出口时需要删除计重信息
    if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
    {
        SetVehAxisInfo();
        //TODO:记重设备..
    }
    // 清空车牌图像
    getDeviceFactory()->getVpr()->ClearVPRResult();
    FillMsgToSend(MsgID,MsgType,msg);
    msg.VehInfo.InductCnt = nInductCount;
    msg.VehInfo.VCnt = nInductCount;
    msg.VehInfo.PVT = VehType2Byte(VT_Motorcade);
    msg.VehInfo.VT = VehType2Byte(VT_Motorcade);
    msg.ICCInfo.CardType = 0xFF;
    msg.PayCardInfo.PayCardType = 0xFF;
    //添加小班汇总
    getShiftSum()->Sum(msg);
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg,true,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("Lane","保存车队过车报文失败");
    }
    LogMsg("lane",tr("报文保存到数据库SendMotorCarPassMsg完成"));
    AddHistWaste(msg);
}
//发送闯关报文
void TollItem::SendRushMsg(int rtype)
{
    int MsgID,MsgType;
    TMsgShiftSum MsgShiftSum;
    TMsgToSend Msg;
    ClearSendMsg(Msg);
    MsgID = 0;
    if(getLaneInfo()->getLaneType() == LaneType_MtcIn)
    {
        MsgID = 1;
    }
    else if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
    {
        MsgID = 2;
    }
    MsgType = rtype;
    FillMsgToSend(MsgID,MsgType,Msg);
    //清除通行费
    Msg.TollInfo.CashCardMoney = 0;
    Msg.TollInfo.CashMoney = 0;
    Msg.TollInfo.LastMoney = 0;
    Msg.TollInfo.LastCardMoney = 0;
    switch(MsgType)
    {
    case LST_TYPE_VIOLATE_LOGOUT:
        Msg.VehInfo.VCnt = 1;
        Msg.VehInfo.InductCnt = 1;
        Msg.DealInfo.SpInfo[DS_SECG2] = '1';
        break;
    case LST_TYPE_VIOLATE:
        Msg.VehInfo.VCnt = 1;
        Msg.VehInfo.InductCnt = 1;

        Msg.DealInfo.SpInfo[DS_SECG1] = '1';
        break;
    case LST_TYPE_VIOLATE_FALSEALARM:
        Msg.VehInfo.VCnt = 0;
        Msg.VehInfo.InductCnt = 0;

        Msg.DealInfo.SpInfo[DS_SECG3] = '1';
        break;
    case  LST_TYPE_VIOLATE_HELDBACK:
        Msg.VehInfo.VCnt = 0;
        Msg.VehInfo.InductCnt = 0;

        Msg.DealInfo.SpInfo[DS_SECG6] = '1';
        break;
    default:
        break;
    }
    Msg.ICCInfo.CardType = 0xFF;
    //添加小班汇总
    getShiftSum()->Sum(Msg);
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,Msg,true,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("Lane","保存收费业务报文失败");
    }
    LogMsg("lane",tr("报文保存到数据库SendRushMsg完成"));
    AddHistWaste(Msg);
}
//卡箱操作报文 1--上箱   2--下箱  3--强制下箱
void TollItem::SendCardBox(char AOperType)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 24;
    MsgType = AOperType;
    //填充车道基本信息以及卡箱信息
    QDateTime ntime = QDateTime::currentDateTime();
    FillBasicInfo(ntime,msg,true);
    FillCardBoxInfo(msg);
    if(!SaveMsg(SEND_BOTH,MsgID,MsgType,msg))
    {
        LogMsg("lane", tr("保存卡箱操作报文失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendCardBox完成"));
}
//发送参数启用报文
void TollItem::SendParamLoadMsg(int nFiled)
{
    TMsgToSend Msg;
    int nMsgID,nMsgType;
    QBaseParamFile* pParamFile = NULL;
    ClearSendMsg(Msg);
    nMsgID = 7;
    nMsgType = 51;

    FillMsgToSend(nMsgID,nMsgType,Msg,true);
    pParamFile = getParamMgr()->GetParamFileById(nFiled);
    Msg.ParamUseInfo.ParaID = nFiled;
    SParamFileHead cfgHead, newCfgHead;
    pParamFile->GetCfgFileHead(cfgHead);
    pParamFile->GetCfgFileHead(newCfgHead, true);
    Msg.ParamUseInfo.RecordCnt = cfgHead.dwRecordCount;
    SParamFileHead filehead;
    if(pParamFile->GetCfgFileHead(filehead,false))
    {
        DateTimeToChar14(filehead.lVersion,Msg.ParamUseInfo.Ver);
        DateTimeToChar14(filehead.tUseTime,Msg.ParamUseInfo.UseTime);
    }else
    {
        //QDebug()<<"格式转换失败";
    }
    Msg.ParamUseInfo.NewRecordCnt = newCfgHead.dwRecordCount;
    if(pParamFile->GetCfgFileHead(filehead,true))
    {
        DateTimeToChar14(filehead.lVersion,Msg.ParamUseInfo.NewVer);
        DateTimeToChar14(filehead.tUseTime,Msg.ParamUseInfo.NewUseTime);
    }else
    {
        //QDebug()<<"格式转换失败";
    }
    if(!SaveMsg(SEND_BOTH,nMsgID,nMsgType,Msg))
    {
        LogMsg("lane","保存参数启用报文失败,[pParamFile.GetFileName])");
    }
    LogMsg("lane",tr("报文保存到数据库SendParamLoadMsg完成"));
}
//发送Pro卡刷卡事件（只发送监控）出口时使用
void TollItem::SendProCardInfo(TProCardBasicInfo &ProCardBasicInfo, quint32 dwBalance)
{
    TMsgToSend Msg;
    int MsgID,MsgType;
    ClearSendMsg(Msg);
    MsgID=25;
    MsgType = 1;
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime, Msg, true);
    if(CARD_TYPE_STORE_CARD == ProCardBasicInfo.bType)
    {
        Msg.ICCInfo.CardType = CT_StoreCard;
    }
    else if(CARD_TYPE_TALLY_CARD == ProCardBasicInfo.bType)
    {
        Msg.ICCInfo.CardType = CT_TallyCard;
    }
    Msg.ICCInfo.CardCnt = 1;
    IntToArChar(Msg.ICCInfo.CardNet,sizeof(Msg.ICCInfo.CardNet),
                ProCardBasicInfo.wNetworkId);
    memcpy(Msg.ICCInfo.CardID,ProCardBasicInfo.szCardNo,sizeof(Msg.ICCInfo.CardID));
    Msg.ICCInfo.CardSN = ProCardBasicInfo.dwCardId;
    Msg.ICCInfo.CardBal = dwBalance;

    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,Msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendProCardInfo失败"));

    }
    LogMsg("lane",tr("报文保存到数据库SendProCardInfo完成"));
}

void TollItem::SendProCardInfo(TProCardBasicInfo &ProCardBasicInfo, quint32 dwBalance, CVehEntryInfo& pEntryInfo)
{
    TMsgToSend Msg;
    int MsgID,MsgType;
    ClearSendMsg(Msg);
    MsgID=25;
    MsgType = 1;
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime, Msg, true);
    if(CARD_TYPE_STORE_CARD == ProCardBasicInfo.bType)
    {
        Msg.ICCInfo.CardType = CT_StoreCard;
    }
    else if(CARD_TYPE_TALLY_CARD == ProCardBasicInfo.bType)
    {
        Msg.ICCInfo.CardType = CT_TallyCard;
    }
    Msg.ICCInfo.CardCnt = 1;
    IntToArChar(Msg.ICCInfo.CardNet,sizeof(Msg.ICCInfo.CardNet),
                ProCardBasicInfo.wNetworkId);
    memcpy(Msg.ICCInfo.CardID,ProCardBasicInfo.szCardNo,sizeof(Msg.ICCInfo.CardID));
    Msg.ICCInfo.CardBal = dwBalance/100.0;

    //查询入口站
    IntToArChar(Msg.VehEnInfo.QueryEnStation,sizeof(Msg.VehEnInfo.QueryEnStation),pEntryInfo.dwQryStationID);

    //入口站号
    IntToArChar(Msg.VehEnInfo.EnStation,sizeof(Msg.VehEnInfo.EnStation),pEntryInfo.dwEnStationID);
    //入口车道编号
    IntToArChar(Msg.VehEnInfo.EnLane,sizeof(Msg.VehEnInfo.EnLane),pEntryInfo.nLaneID);
    //入口时间
    DateTimeToChar14(pEntryInfo.EnTime,Msg.VehEnInfo.EnTime);
    //入口序列号
    Msg.VehEnInfo.EnWasteSN = pEntryInfo.bEnWasteSN;
    //入口车道机构类型
    Msg.VehEnInfo.EnLaneType = pEntryInfo.bEnLaneType;
    //入口收费员工号
    IntToArChar(Msg.VehEnInfo.EnOper,sizeof(Msg.VehEnInfo.EnOper),pEntryInfo.dwEnOper);
    //入口车型
    Msg.VehEnInfo.EnVC = pEntryInfo.bEnVC;
    //入口车种
    Msg.VehEnInfo.EnVT = pEntryInfo.bEnVT;
    //入口车牌颜色
    Msg.VehEnInfo.EnVLPC = pEntryInfo.bEnVLPC;
    //入口车牌
    qMemCopy(Msg.VehEnInfo.EnVLP,pEntryInfo.szEnVLP,sizeof(Msg.VehEnInfo.EnVLP));
    //入口PSAM卡终端机号
    qMemCopy(Msg.VehEnInfo.EnPSAMTerm,pEntryInfo.szEnPSamTerm,sizeof(Msg.VehEnInfo.EnPSAMTerm));

    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,Msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendProCardInfo失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendProCardInfo完成"));
}

//监控回控信息
bool TollItem::SendRequestMonInfo(TMonReqID AMonReqID)
{
    TMsgToSend msg;
    int nMsgId,nMsgType;
    ClearSendMsg(msg);
    nMsgId = 27;
    nMsgType = 1;
    if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
    {
        SetVehAxisInfo();
    }
    m_dOccTime = QDateTime::currentDateTime();
    FillMsgToSend(nMsgId,nMsgType,msg,true);
    //填充请求回控信息
    memset(&msg.RequestMonInfo,0,sizeof(msg.RequestMonInfo));
    msg.RequestMonInfo.EventCode = AMonReqID;
    msg.RequestMonInfo.ResponseType = 1;
    msg.RequestMonInfo.SpEventSet[AMonReqID] = '1';
    //单独发送监控
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,nMsgId,nMsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendRequestMonInfo失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendRequestMonInfo完成"));
    return true;
}
//网络状态报文
void TollItem::SendNetStatus()
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 30;
    MsgType = 0;
    FillMsgToSend(MsgID,MsgType,msg,true);
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendNetStatus失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendNetStatus完成"));
}
//车辆信息输入变化报文
void TollItem::SendVehInputChange(TVehChangeType AVehChange)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 21;
    MsgType  = AVehChange;
    m_dOccTime = QDateTime::currentDateTime();
    FVTInputTime = QDateTime::currentDateTime();
    //填充报文基本信息
    FillBasicInfo(m_dOccTime,msg,true);
    msg.VehInfo.PVC = VehClass2Byte(m_VehInfo.PVehClass);
    msg.VehInfo.AutoVC = VehClass2Byte(m_VehInfo.AutoVehClass);
    msg.VehInfo.VC = VehClass2Byte(m_VehInfo.VehClass);
    msg.VehInfo.PVT = VehType2Byte(m_VehInfo.PVehType);
    msg.VehInfo.VT = VehType2Byte(m_VehInfo.VehType);
    memcpy(msg.VehInfo.AutoVLP,m_VehInfo.szAutoVehPlate,qMin(sizeof(m_VehInfo.szAutoVehPlate),sizeof(msg.VehInfo.AutoVLP)));
    msg.VehInfo.AutoVLPC = m_VehInfo.nAutoVehPlateColor;
    memcpy(msg.VehInfo.VLP,m_VehInfo.szVehPlate,qMin(sizeof(m_VehInfo.szVehPlate),sizeof(msg.VehInfo.VLP)));
    msg.VehInfo.VLPC = m_VehInfo.nVehPlateColor;
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendVehInputChange失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendVehInputChange完成"));
}
//车道事件报文
void TollItem::SendLaneEvent(TLaneEventType AEvent)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 7;
    MsgType = AEvent;
    //根据位元表填MsgToSend各位元   流水不增加
    FillMsgToSend(MsgID,MsgType,msg,true);
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendLaneEvent失败"));

    }
    LogMsg("lane",tr("报文保存到数据库SendLaneEvent完成"));
}
//发送车队开始报文
void TollItem::SendMotorBeginMsg()
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    MsgID = 0;
    switch(getLaneInfo()->getLaneType())
    {
    case LaneType_MtcIn:
        MsgID = 1;
        break;
    case LaneType_MtcOut:
        MsgID = 2;
        break;
    }
    MsgType = LST_TYPE_MOTORBEGIN;
    FillMsgToSend(MsgID,MsgType,msg);
    msg.VehInfo.PVT = VehType2Byte(VT_Motorcade);
    msg.VehInfo.VT = VehType2Byte(VT_Motorcade);
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendMotorBeginMsg失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendMotorBeginMsg完成"));
}

void TollItem::SendImageMsg(const QString& imageFile)
{
    TMsgToSend msg;
    int msgId = 28;
    int msgType = 1;
    ClearSendMsg(msg);
    FillMsgToSend(msgId, msgType, msg);
    QFile image(imageFile);

    //如果图片文件存在
    if(image.open(QIODevice::ReadOnly))
    {
        //设置抓拍图片信息
        strncpy(msg.CapImage.ImageFileName, imageFile.toLocal8Bit().data(), 128);
        strncpy(msg.CapImage.PhotoTime, QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
        msg.CapImage.ImageSize = image.size();
        if(!SaveMsg(SEND_SUPERVISOR_ONLY, msgId, msgType, msg))
        {
            LogMsg("lane", tr("[SendImageMsg]发送图片抓拍报文失败"));
        }
        else
        {
            LogMsg("lane", tr("[SendImageMsg]发送图片抓拍报文成功"));
        }
    }
    else
    {
        LogMsg("lane", tr("[SendImageMsg]图像文件[%1]打开失败，停止发送抓拍图片").arg(imageFile));
    }

}
//发送车队结束报文
void TollItem::SendMotorEndMsg(int CarCount)
{
    TMsgToSend msg;
    int MsgID,MsgType;
    ClearSendMsg(msg);
    switch(getLaneInfo()->getLaneType())
    {
    case LaneType_MtcIn:
        MsgType = 1;
        break;
    case LaneType_MtcOut:
        MsgType = 2;
        break;
    default:
        LogMsg("lane", tr("车道类型错误：%1").arg(getLaneInfo()->getLaneType()));
        break;
    }
    MsgType = LST_TYPE_MOTOREND;
    FillMsgToSend(MsgID,MsgType,msg);
    msg.VehInfo.InductCnt = CarCount;
    msg.VehInfo.VCnt = CarCount;
    msg.VehInfo.PVT = VehType2Byte(VT_Motorcade);
    msg.VehInfo.VT = VehType2Byte(VT_Motorcade);
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendMotorEndMsg失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendMotorEndMsg完成"));
}

void TollItem::setCardType(qint16 type)
{
    m_nCardType = type;
}

int TollItem::getCardType()
{
    return m_nCardType;
}

void TollItem::SendTollCardInfo(TTollCardBasicInfo &TollCardBasicInfo)
{
    TMsgToSend Msg;
    int MsgID,MsgType;
    ClearSendMsg(Msg);
    MsgID = 25;
    MsgType = 1;
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime,Msg,true);
    Msg.ICCInfo.CardType = 0;
    Msg.ICCInfo.CardCnt = 1;
    IntToArChar(Msg.ICCInfo.CardNet,sizeof(Msg.ICCInfo.CardNet),TollCardBasicInfo.M1BasicInfo.wNetworkId);
    IntToArChar(Msg.ICCInfo.CardID,sizeof(Msg.ICCInfo.CardID),TollCardBasicInfo.M1BasicInfo.dwCardNum);
    Msg.ICCInfo.CardSN = TollCardBasicInfo.M1BasicInfo.dwCardId;
    //单独发送监控报文
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,MsgID,MsgType,Msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendTollCardInfo失败"));
    }
    LogMsg("lane",tr("报文保存到数据库SendTollCardInfo完成"));
}
void TollItem::SendTollCardInfo(TTollCardBasicInfo &TollCardBasicInfo,CVehEntryInfo &pEntryInfo)
{
    TMsgToSend msg;
    int msgID,msgType;
    ClearSendMsg(msg);
    msgID =25;
    msgType = 1;
    m_dOccTime = QDateTime::currentDateTime();
    FillBasicInfo(m_dOccTime,msg,true);
    msg.ICCInfo.CardType = 0;
    msg.ICCInfo.CardCnt = 1;
    IntToArChar(msg.ICCInfo.CardNet,sizeof(msg.ICCInfo.CardNet),TollCardBasicInfo.M1BasicInfo.wNetworkId);
    IntToArChar(msg.ICCInfo.CardID,sizeof(msg.ICCInfo.CardID),TollCardBasicInfo.M1BasicInfo.dwCardNum);
    msg.ICCInfo.CardSN = TollCardBasicInfo.M1BasicInfo.dwCardId;
    IntToArChar(msg.VehEnInfo.QueryEnStation,sizeof(msg.VehEnInfo.QueryEnStation),pEntryInfo.dwQryStationID);
    IntToArChar(msg.VehEnInfo.EnStation,sizeof(msg.VehEnInfo.EnStation),pEntryInfo.dwEnStationID);
    IntToArChar(msg.VehEnInfo.EnLane,sizeof(msg.VehEnInfo.EnLane),pEntryInfo.nLaneID);
    DateTimeToChar14(pEntryInfo.EnTime,msg.VehEnInfo.EnTime);
    msg.VehEnInfo.EnWasteSN = pEntryInfo.bEnWasteSN;
    msg.VehEnInfo.EnLaneType = pEntryInfo.bEnLaneType;
    IntToArChar(msg.VehEnInfo.EnOper,sizeof(msg.VehEnInfo.EnOper),pEntryInfo.dwEnOper);
    msg.VehEnInfo.EnVC = pEntryInfo.bEnVC;
    msg.VehEnInfo.EnVT = pEntryInfo.bEnVT;
    msg.VehEnInfo.EnVLPC = pEntryInfo.bEnVLPC;
    memcpy(msg.VehEnInfo.EnVLP,pEntryInfo.szEnVLP,sizeof(msg.VehEnInfo.EnVLP));
    memcpy(msg.VehEnInfo.EnPSAMTerm,pEntryInfo.szEnPSamTerm,sizeof(msg.VehEnInfo.EnPSAMTerm));
    if(!SaveMsg(SEND_SUPERVISOR_ONLY,msgID,msgType,msg))
    {
        LogMsg("lane",tr("报文保存到数据库SendTollCardInfo失败"));

    }
    LogMsg("lane",tr("报文保存到数据库SendTollCardInfo完成"));
}

void TollItem::Clear()
{
    ClearImageInfo();
    ClearSendMsg(FMsgToSend);
    ClearSendMsg(m_HdgMsgToSend);
    FYWDataToSave.clear();
    m_ProCardBasicInfo.clear();
    FCardTollInfo.clear();
    FTollCardBasicInfo.bCRC = false;
    FTollCardBasicInfo.bOldCard = false;
    FTollCardBasicInfo.M1BasicInfo.clear();
    FdwBalance = 0;
    BasicMaterialCardType = 0;
    FPhysicalCardType = TYPE_NULL;
    m_nCardType = -1;
    FnYwType = LST_TYPE_NORMAL;
    m_nViolateHeldBack = 0;
    m_bVehBlack = false;
    m_bVehGray = false;
    m_bCardBlack = false;
    memset(FDevStatus,0,sizeof(FDevStatus));
    FAuthInfo.clear();
}

