#include "msgsendfunc.h"
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
MsgSendFunc::MsgSendFunc(QObject *parent) :
    QObject(parent)
{
}
//计算报文长度
quint32 MsgSendFunc::GetTotalSize(TMsgToSend &Msg)
{
    quint32 Total = 0;
    QString strFileName = "";
    //报文头基本信息 --
    if(Testbyte(Msg.BasicInfo.wy,0) == 1)
    {
        Total = Total + sizeof(Msg.BasicInfo);
    }
    //1	车道基本信息--
    if(Testbyte(Msg.BasicInfo.wy,1) == 1)
    {
        Total = Total + sizeof(Msg.LaneInfo);
    }
    //2	车辆信息--
    if(Testbyte(Msg.BasicInfo.wy,2) == 1)
    {
        Total = Total + sizeof(Msg.VehInfo);
    }
    //3	IC卡信息结构--
    if(Testbyte(Msg.BasicInfo.wy,3) == 1)
    {
        Total = Total + sizeof(Msg.ICCInfo);
    }
    //4	支付卡信息
    if(Testbyte(Msg.BasicInfo.wy,4) == 1)
    {
        Total = Total + sizeof(Msg.PayCardInfo);
    }
    //5	OBU信息
    if(Testbyte(Msg.BasicInfo.wy,5) == 1)
    {
        Total = Total + sizeof(Msg.OBUInfo);
    }
    //6	卡箱信息
    if(Testbyte(Msg.BasicInfo.wy,6) == 1)
    {
        Total = Total + sizeof(Msg.CardBoxInfo);
    }
    //7	按键信息
    if(Testbyte(Msg.BasicInfo.wy,7) == 1)
    {
        Total = Total + (Msg.KeyInfo.KeyNum * 8) + sizeof(Msg.KeyInfo.KeyNum);
    }
    //8	交易状态信息
    if(Testbyte(Msg.BasicInfo.wy,8) == 1)
    {
        Total = Total + sizeof(Msg.DealInfo);
    }
    //9	授权信息
    if(Testbyte(Msg.BasicInfo.wy,9) == 1)
    {
        Total = Total + sizeof(Msg.AuthInfo);
    }
    //10	车辆入口信息
    if(Testbyte(Msg.BasicInfo.wy,10) == 1)
    {
        Total = Total + sizeof(Msg.VehEnInfo);
    }
    //11	计重信息
    if(Testbyte(Msg.BasicInfo.wy,11) == 1)
    {
        Total = Total + sizeof(Msg.WeightInfo);
    }
    //12	计费基础信息
    if(Testbyte(Msg.BasicInfo.wy,12) == 1)
    {
        Total = Total + sizeof(Msg.TollBaseInfo) - sizeof(Msg.TollBaseInfo.FreeArea);
        Total = Total + (Msg.TollBaseInfo.FreeAreaCnt * 15);
    }
    //13	通行费信息
    if(Testbyte(Msg.BasicInfo.wy,13) == 1)
    {
        Total = Total + sizeof(Msg.TollInfo);
    }
    //14	拆分信息
    if(Testbyte(Msg.BasicInfo.wy,14) == 1)
    {
        Total = Total + Msg.SplitInfo.SpliteNum * 18 + sizeof(Msg.SplitInfo.SpliteNum);
        Total = Total + Msg.SplitInfo.CellNum * 8 + sizeof(Msg.SplitInfo.CellNum);
        Total = Total + Msg.SplitInfo.StationNum * 6 + sizeof(Msg.SplitInfo.StationNum);
    }
    //15	票据修改信息
    if(Testbyte(Msg.BasicInfo.wy,15) == 1)
    {
        Total = Total + sizeof(Msg.InvModefyInfo);
    }
    //16	票据打印信息
    if(Testbyte(Msg.BasicInfo.wy,16) == 1)
    {
        Total = Total + sizeof(Msg.InvPrintInfo);
    }
    //17	工班汇总信息
    if(Testbyte(Msg.BasicInfo.wy,17) == 1)
    {
        Total = Total + sizeof(Msg.ShiftSum);
    }
    //18	参数启用信息
    if(Testbyte(Msg.BasicInfo.wy,18) == 1)
    {
        Total = Total + sizeof(Msg.ParamUseInfo);
    }
    //19 请求监控回控信息
    if(Testbyte(Msg.BasicInfo.wy,19) == 1)
    {
        Total = Total + sizeof(Msg.RequestMonInfo);
    }
    //31	抓拍图像信息
    if(Testbyte(Msg.BasicInfo.wy,31) == 1)
    {
        strFileName = QString::fromLocal8Bit(Msg.CapImage.ImageFileName);
        LogMsg("lane",tr("[getTotalsize]抓拍图像：[%1]").arg(strFileName));
        if(!QFile().exists(strFileName) || strFileName =="")
        {
            LogMsg("lane",tr("GetTotalSize不存在的抓拍图像文件%1").arg(strFileName));
            Msg.CapImage.ImageSize = 0;
            memset(Msg.CapImage.ImageFileName,0,sizeof(Msg.CapImage.ImageFileName));
            Total = Total + sizeof(Msg.CapImage.ImageFileName) + sizeof(Msg.CapImage.PhotoTime) + 4;
        }
        else
        {
            QByteArray bytes;
            QFile file(strFileName);
            file.open(QIODevice::ReadOnly);
            bytes =  file.readAll();
            Total = Total + sizeof(Msg.CapImage.ImageFileName) + bytes.size() + sizeof(Msg.CapImage.PhotoTime) + 4;
            Msg.CapImage.ImageSize = bytes.size();
            bytes.clear();
            file.close();
            LogMsg("lane",tr("GetTotalSize抓拍图像文件%1大小%2").arg(strFileName).arg(bytes.size()));
        }

    }
    //32	车牌图像信息
    if(Testbyte(Msg.BasicInfo.wy,32) == 1)
    {
        strFileName = QString::fromLocal8Bit(Msg.VLPImage.ImageFileName);
        LogMsg("lane",tr("[getTotalsize]车牌图像：[%1]").arg(strFileName));
        if(!QFile().exists(strFileName) || strFileName == "")
        {
            LogMsg("lane",tr("GetTotalSize不存在的车牌图像文件%1").arg(strFileName));
            Msg.VLPImage.ImageSize = 0;
            memset(Msg.VLPImage.ImageFileName,0,sizeof(Msg.VLPImage.ImageFileName));
            Total = Total + sizeof(Msg.VLPImage.ImageFileName) + sizeof(Msg.VLPImage.PhotoTime) + 4;
        }else
        {
            QByteArray bytes;
            QFile file(strFileName);
            file.open(QIODevice::ReadOnly);
            bytes =  file.readAll();
            Total = Total + sizeof(Msg.VLPImage.ImageFileName) + bytes.size() + sizeof(Msg.VLPImage.PhotoTime) + 4;
            Msg.VLPImage.ImageSize = bytes.size();
            bytes.clear();
            file.close();
            LogMsg("lane",tr("GetTotalSize图像文件%1大小%2").arg(strFileName).arg(bytes.size()));
        }
    }
    LogMsg("lane",tr("计算报文长度GetTotalSize：%1").arg(Total));
    return Total;
}
//计算位元中指定位置的是否为1
int MsgSendFunc::Testbyte(const char* weiyuanbiao, int position)
{
    quint8 tmp_byte = 0;
    quint8 Shang = 0;
    quint8 YuShu = 0;
    quint8 Standard = 0;
    int tmpPosition = position-1;
    if(position == 0)
    {
        return 1;
    }
    tmpPosition = position - 1;
    YuShu = tmpPosition % 8;
    Shang = tmpPosition / 8;
    tmp_byte = (weiyuanbiao[Shang]);
    Standard = 0x01;
    Standard = Standard << (7-YuShu);
    if((Standard & tmp_byte) > 0)
    {
        return 1;
    }
    else
        return 0;
}
//打包报文
bool MsgSendFunc::PackMsg(const TMsgToSend* Msg, char* MsgBuff, int &nBufflen)
{
    int TotalLen = 0;
    QString strFileName = "";
    quint32 imgSize = 0;
    int tmpTotalLen = 0;
    quint16 wKeyNum = 0;
    TMsgToSend tmpMsg;
    //重新计算报文长度
    memcpy(&tmpMsg,Msg,sizeof(tmpMsg));
    tmpMsg.BasicInfo.msgHead.TotalLength = GetTotalSize(tmpMsg);
    if(Testbyte(tmpMsg.BasicInfo.wy,0) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.BasicInfo,sizeof(tmpMsg.BasicInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.BasicInfo);
    }
    //1 车道信息
    if(Testbyte(tmpMsg.BasicInfo.wy,1) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.LaneInfo,sizeof(tmpMsg.LaneInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.LaneInfo);
    }
    //2	车辆信息
    if(Testbyte(tmpMsg.BasicInfo.wy,2) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.VehInfo,sizeof(tmpMsg.VehInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.VehInfo);
    }
    //3	IC卡信息结构
    if(Testbyte(tmpMsg.BasicInfo.wy,3) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.ICCInfo,sizeof(tmpMsg.ICCInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.ICCInfo);
    }
    //4	支付卡信息
    if(Testbyte(tmpMsg.BasicInfo.wy,4) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.PayCardInfo,sizeof(tmpMsg.PayCardInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.PayCardInfo);
    }
    //5	OBU信息
    if(Testbyte(tmpMsg.BasicInfo.wy,5) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.OBUInfo,sizeof(tmpMsg.OBUInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.OBUInfo);
    }
    //6	卡箱信息
    if(Testbyte(tmpMsg.BasicInfo.wy,6) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.CardBoxInfo,sizeof(tmpMsg.CardBoxInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.CardBoxInfo);
    }
    //7	按键信息
    if(Testbyte(tmpMsg.BasicInfo.wy,7) == 1)
    {
        wKeyNum = tmpMsg.KeyInfo.KeyNum;
        LogMsg("lane",tr("按键个数%1").arg(wKeyNum));
        tmpTotalLen = qMin(wKeyNum*8,(signed)(sizeof(tmpMsg.KeyInfo.KeyValue)));
        tmpTotalLen = tmpTotalLen + sizeof(tmpMsg.KeyInfo.KeyNum);
        memcpy(&MsgBuff[TotalLen],&tmpMsg.KeyInfo,tmpTotalLen);
        TotalLen = TotalLen + tmpTotalLen;
    }
    //8	交易状态信息
    if(Testbyte(tmpMsg.BasicInfo.wy,8) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.DealInfo,sizeof(tmpMsg.DealInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.DealInfo);
    }
    //9	授权信息
    if(Testbyte(tmpMsg.BasicInfo.wy,9) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.AuthInfo,sizeof(tmpMsg.AuthInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.AuthInfo);
    }
    //10	车辆入口信息
    if(Testbyte(tmpMsg.BasicInfo.wy,10) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.VehEnInfo,sizeof(tmpMsg.VehEnInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.VehEnInfo);
    }
    //11	计重信息
    if(Testbyte(tmpMsg.BasicInfo.wy,11) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.WeightInfo,sizeof(tmpMsg.WeightInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.WeightInfo);
    }
    //12	计费基础信息
    if(Testbyte(tmpMsg.BasicInfo.wy,12) == 1)
    {
        tmpTotalLen = sizeof(tmpMsg.TollBaseInfo) - sizeof(tmpMsg.TollBaseInfo.FreeArea);
        tmpTotalLen = tmpTotalLen + (tmpMsg.TollBaseInfo.FreeAreaCnt * 15);
        memcpy(&MsgBuff[TotalLen],&tmpMsg.TollBaseInfo,tmpTotalLen);
        TotalLen = TotalLen + tmpTotalLen;
    }
    //13	通行费信息
    if(Testbyte(tmpMsg.BasicInfo.wy,13) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.TollInfo,sizeof(tmpMsg.TollInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.TollInfo);
    }
    //14	拆分信息
    if(Testbyte(tmpMsg.BasicInfo.wy,14) == 1)
    {
        wKeyNum = tmpMsg.SplitInfo.SpliteNum;
        LogMsg("Lane",tr("拆分次数：%1").arg(wKeyNum));
        tmpTotalLen = qMin(wKeyNum*18,(signed)(sizeof(tmpMsg.SplitInfo.SplitInfo)));
        tmpTotalLen = tmpTotalLen + sizeof(tmpMsg.SplitInfo.SpliteNum);
        memcpy(&MsgBuff[TotalLen],&tmpMsg.SplitInfo.SpliteNum,tmpTotalLen);
        TotalLen = TotalLen + tmpTotalLen;

        wKeyNum = tmpMsg.SplitInfo.CellNum;
        LogMsg("Lane",tr("收费单元数：%1").arg(wKeyNum));
        tmpTotalLen = qMin(wKeyNum*8,(signed)(sizeof(tmpMsg.SplitInfo.CellInfo)));
        tmpTotalLen = tmpTotalLen + sizeof(tmpMsg.SplitInfo.CellNum);
        memcpy(&MsgBuff[TotalLen],&tmpMsg.SplitInfo.CellNum,tmpTotalLen);
        TotalLen = TotalLen + tmpTotalLen;

        wKeyNum = tmpMsg.SplitInfo.StationNum;
        LogMsg("Lane",tr("经过收费站数:%1").arg(wKeyNum));
        tmpTotalLen = qMin(wKeyNum*6,(signed)(sizeof(tmpMsg.SplitInfo.PathInfo)));
        tmpTotalLen = tmpTotalLen + sizeof(tmpMsg.SplitInfo.StationNum);
        memcpy(&MsgBuff[TotalLen],&tmpMsg.SplitInfo.StationNum,tmpTotalLen);
        TotalLen = TotalLen + tmpTotalLen;
    }
    //15	票据修改信息
    if(Testbyte(tmpMsg.BasicInfo.wy,15) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.InvModefyInfo,sizeof(tmpMsg.InvModefyInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.InvModefyInfo);
    }
    //16	票据打印信息
    if(Testbyte(tmpMsg.BasicInfo.wy,16) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.InvPrintInfo,sizeof(tmpMsg.InvPrintInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.InvPrintInfo);
    }
    //17	工班汇总信息
    if(Testbyte(tmpMsg.BasicInfo.wy,17) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.ShiftSum,sizeof(tmpMsg.ShiftSum));
        TotalLen = TotalLen + sizeof(tmpMsg.ShiftSum);
    }
    //18	参数启用信息
    if(Testbyte(tmpMsg.BasicInfo.wy,18) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.ParamUseInfo,sizeof(tmpMsg.ParamUseInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.ParamUseInfo);
    }
    //19 请求监控回控信息
    if(Testbyte(tmpMsg.BasicInfo.wy,19) == 1)
    {
        memcpy(&MsgBuff[TotalLen],&tmpMsg.RequestMonInfo,sizeof(tmpMsg.RequestMonInfo));
        TotalLen = TotalLen + sizeof(tmpMsg.RequestMonInfo);
    }

    //31
    if(Testbyte(tmpMsg.BasicInfo.wy,31) == 1)
    {
        tmpTotalLen = TotalLen;
        strFileName = QString::fromLocal8Bit(tmpMsg.CapImage.ImageFileName);
        LogMsg("lane",tr("[packMsg]抓拍图像：[%1]").arg(strFileName));
        if(!QFile().exists(strFileName) || strFileName =="")
        {
            LogMsg("lane",tr("packMsg不存在的抓拍图像文件%1").arg(strFileName));
            memset(&MsgBuff[TotalLen],0,sizeof(tmpMsg.CapImage) - 4);
            TotalLen = tmpTotalLen + sizeof(tmpMsg.CapImage) - 4;
        }else
        {
            QByteArray bytes;
            QFile file(strFileName);
            file.open(QIODevice::ReadOnly);
            bytes =  file.readAll();
            memcpy(&MsgBuff[TotalLen],tmpMsg.CapImage.PhotoTime,sizeof(tmpMsg.CapImage.PhotoTime));
            TotalLen = TotalLen + sizeof(tmpMsg.CapImage.PhotoTime);
            memcpy(&MsgBuff[TotalLen],tmpMsg.CapImage.ImageFileName,sizeof(tmpMsg.CapImage.ImageFileName));
            TotalLen = TotalLen + sizeof(tmpMsg.CapImage.ImageFileName);
            //图像内容
            imgSize = bytes.size();

            memcpy(MsgBuff + TotalLen, &imgSize, sizeof(imgSize));
            TotalLen = TotalLen + sizeof(imgSize);
            memcpy(&MsgBuff[TotalLen],bytes.data(),bytes.size());
            TotalLen = TotalLen + bytes.size();
            file.close();
            //liujian 照片打包完后删除本地文件
            LogMsg("lane",tr("packMsg抓拍图像文件%1大小%2").arg(strFileName).arg(bytes.size()));
        }
    }
    //32	车牌图像信息
    if(Testbyte(tmpMsg.BasicInfo.wy,32) == 1)
    {
        tmpTotalLen = TotalLen;
        strFileName = QString::fromLocal8Bit(tmpMsg.VLPImage.ImageFileName);
        LogMsg("lane",tr("[packMsg]车牌图像[%1]：").arg(strFileName));
        if(!QFile().exists(strFileName) || strFileName == "")
        {
            LogMsg("lane",tr("packMsg不存在的车牌图像文件%1").arg(strFileName));
            memset(&MsgBuff[TotalLen],0,sizeof(tmpMsg.VLPImage) - 4);
            TotalLen = TotalLen + sizeof(tmpMsg.VLPImage) - 4;
        }
        else
        {
            QByteArray bytes;
            QFile file(strFileName);
            file.open(QIODevice::ReadOnly);
            bytes =  file.readAll();
            memcpy(&MsgBuff[TotalLen],tmpMsg.VLPImage.PhotoTime,sizeof(tmpMsg.VLPImage.PhotoTime));
            TotalLen = TotalLen + sizeof(tmpMsg.VLPImage.PhotoTime);
            memcpy(&MsgBuff[TotalLen],tmpMsg.VLPImage.ImageFileName,sizeof(tmpMsg.VLPImage.ImageFileName));
            TotalLen = TotalLen + sizeof(tmpMsg.VLPImage.ImageFileName);
            //图像内容
            imgSize = bytes.size();

            memcpy(MsgBuff + TotalLen, &imgSize, sizeof(imgSize));
            TotalLen = TotalLen + sizeof(imgSize);
            memcpy(&MsgBuff[TotalLen],bytes.data(),bytes.size());
            TotalLen = TotalLen + bytes.size();
            file.close();

            LogMsg("lane",tr("packMsg车牌图像文件%1大小%2").arg(strFileName).arg(bytes.size()));
        }

    }
    nBufflen = TotalLen;
    return true;
}
//返回报文位元
void MsgSendFunc::GetWeiYuan(int nMsgID, char* weiyuanbiao)
{
    char tmpWeiYuan[4];
    switch(nMsgID)
    {
    case 1:
        tmpWeiYuan[0] = 0xFF;
        tmpWeiYuan[1] = 0x80;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x03;
        break;
    case 2:
        tmpWeiYuan[0] = 0xFF;
        tmpWeiYuan[1] = 0xFD;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x03;
        break;
    case 3:
        tmpWeiYuan[0] = 0x83;
        tmpWeiYuan[1] = 0;
        tmpWeiYuan[2] = 0;
        tmpWeiYuan[3] = 0;
        break;
    case 4:
        tmpWeiYuan[0] = 0x81;
        tmpWeiYuan[1] = 0;
        tmpWeiYuan[2] = 0;
        tmpWeiYuan[3] = 0;
        break;
    case 5:
        tmpWeiYuan[0] = 0x84;
        tmpWeiYuan[1] = 0x00;
        tmpWeiYuan[2] = 0x80;
        tmpWeiYuan[3] = 0x00;
        break;
        //ETC车道计重0x80200000
    case 6:
        tmpWeiYuan[0] = 0x80;
        tmpWeiYuan[1] = 0x20;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //车道事件 0xA9880000
    case 7:
        tmpWeiYuan[0] = 0xA9;
        tmpWeiYuan[1] = 0x88;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //车道基本信息 0x85020000
    case 20:
        tmpWeiYuan[0] = 0x85;
        tmpWeiYuan[1] = 0x02;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //车辆输入信息变化 0xC0800000
    case 21:
        tmpWeiYuan[0] = 0xC0;
        tmpWeiYuan[1] = 0x80;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //计重信息修改 0x80200000
    case 22:
        tmpWeiYuan[0] = 0x80;
        tmpWeiYuan[1] = 0x20;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
    case 23:
        tmpWeiYuan[0] = 0x80;
        tmpWeiYuan[1] = 0x02;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
    case 24:
        tmpWeiYuan[0] = 0x84;
        tmpWeiYuan[1] = 0x00;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //通行卡信息 0xA0400000
    case 25:
        tmpWeiYuan[0] = 0xA0;
        tmpWeiYuan[1] = 0x40;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //通行费信息 0x80080000
    case 26:
        tmpWeiYuan[0] = 0x80;
        tmpWeiYuan[1] = 0x48;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
        //监控回控 0xFDE22003
    case 27:
        tmpWeiYuan[0] = 0xFD;
        tmpWeiYuan[1] = 0xE2;
        tmpWeiYuan[2] = 0x20;
        tmpWeiYuan[3] = 0x03;
        break;
        //入口车辆放行 0xEF800000
//    case 28:
//        tmpWeiYuan[0] = 0xEF;
//        tmpWeiYuan[1] = 0x80;
//        tmpWeiYuan[2] = 0x00;
//        tmpWeiYuan[3] = 0x00;
//        break;
//        //出口车辆放行 0xFFFD0000
//    case 29:
//        tmpWeiYuan[0] = 0xFF;
//        tmpWeiYuan[1] = 0xFD;
//        tmpWeiYuan[2] = 0x00;
//        tmpWeiYuan[3] = 0x00;
//        break;
//    case 30:
//        tmpWeiYuan[0] = 0x81;
//        tmpWeiYuan[1] = 0x00;
//        tmpWeiYuan[2] = 0x00;
//        tmpWeiYuan[3] = 0x00;
//        break;
    case 28:
        tmpWeiYuan[0] = 0x81;
        tmpWeiYuan[1] = 0x20;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x03;
        break;
    case 30:
        tmpWeiYuan[0] = 0x81;
        tmpWeiYuan[1] = 0x00;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
    case 57:
        tmpWeiYuan[0] = 0x81;
        tmpWeiYuan[1] = 0x00;
        tmpWeiYuan[2] = 0x00;
        tmpWeiYuan[3] = 0x00;
        break;
    default:
        memset(tmpWeiYuan,0,sizeof(tmpWeiYuan));
    }
    memcpy(weiyuanbiao,tmpWeiYuan,4);
}
//打包参数请求报文
bool MsgSendFunc::PackParamReqMsg(int nMsgID, const TMsgReqParam msg, QByteArray pBuffer)
{
    bool Result = false;
    return Result;
}
// 计算速度，传入 行驶里程（米）、行驶时间（秒）
quint32 MsgSendFunc::CalcVCSpeed(quint32 nDiseMeter, quint32 nDriveTime)
{
    double CalcRet = 0;
    int Result = 0;
    if(nDriveTime == 0)
        return 0;
    //计算成  米/秒
    CalcRet = nDiseMeter/nDriveTime;
    //算成 米/小时
    CalcRet = CalcRet * 3600;
    //算成   千米/小时
    //CalcRet = CalcRet / 1000;
    LogMsg("lane",tr("行驶里程：%1,行驶时间：%2,车辆平均速度：%3").arg(nDiseMeter).arg(nDriveTime).arg(CalcRet));
    Result = qRound(CalcRet);
    return Result;
}
MsgSendFunc *getMsgSendFun()
{
    static MsgSendFunc msg;
    return &msg;
}
