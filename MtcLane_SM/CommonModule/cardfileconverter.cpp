#include "cardfileconverter.h"
#include <memory>
#include <QChar>
#include "laneutils_new.h"
#include "ilogmsg.h"

bool  TCardFileConverter::CardTollInfo2Pro0012Raw(TPro0012Raw  &Pro0012Raw , const TCardTollInfo&  CardTollInfo)
{
    static int i = 0;
    ////qDebug()<<QObject::tr("第")<<i++<<QObject::tr("次用到")<<"CardTollInfo2Pro0012Raw()";
    quint32 dwTemp=0 ;
    qMemSet(&Pro0012Raw,0,sizeof(Pro0012Raw));
    //	2	入口路网号，路网号=5301
    ll2bcd((char*)&Pro0012Raw.wNetworkId, 2, CardTollInfo.wNetworkId);
    //	5	入口收费站编码

    ll2bcd((char*)Pro0012Raw.baStaId, 3, CardTollInfo.dwStaId);
    //	6	入口车道
    Pro0012Raw.bLaneId=CardTollInfo.bLaneId ;
    //	10	入/出时间，采用UNIX时间，从2000年01月01日零时开始
    Pro0012Raw.dwPassTime=DateTimeToUnixTime2000(CardTollInfo.PassTime) ;
    //	11	入口车型
    Pro0012Raw.bVehClass=CardTollInfo.bVehClass ;
    //	12	通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    Pro0012Raw.bPassStatus=CardTollInfo.bPassStatus ;
    //	18	入口PSAM卡号
    qMemCopy(Pro0012Raw.baPsamTermId,CardTollInfo.baPsamTermId,sizeof(Pro0012Raw.baPsamTermId));
    //	19	入口流水记录号
    Pro0012Raw.bRecIndex=CardTollInfo.bRecIndex ;
    //	23	操作员号
    Pro0012Raw.dwOpId= CardTollInfo.dwOpId;
    //	24	入口班次
    Pro0012Raw.bShiftId= CardTollInfo.bShiftId ;
    //	36	车牌号码，无信息字节均为0x00
    qMemCopy(Pro0012Raw.szVehPlate,CardTollInfo.szVehPlate,12);
    //	37	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色；0xFF-未输入颜色
    Pro0012Raw.bVehPlateColor= CardTollInfo.bVehPlateColor;
    return 1;
}
bool TCardFileConverter::CardTollInfo2TollCardTollInfoRaw(TTollCardTollInfoRaw &TollCardTollInfoRaw,const TCardTollInfo& CardTollInfo )
{
    // cRc暂时设置成0
    qMemSet(&TollCardTollInfoRaw, 0, sizeof(TTollCardTollInfoRaw));
    qMemCopy(TollCardTollInfoRaw.baStaId, &CardTollInfo.dwStaId, 3);
    TollCardTollInfoRaw.bLaneId=CardTollInfo.bLaneId;
    TollCardTollInfoRaw.dwPassTime = SystemTime2CardTime(CardTollInfo.PassTime);
    TollCardTollInfoRaw.bRecIndex=CardTollInfo.bRecIndex;
    TollCardTollInfoRaw.dwOpId=CardTollInfo.dwOpId;
    TollCardTollInfoRaw.bVehClass=CardTollInfo.bVehClass;
    TollCardTollInfoRaw.bVehTollType=CardTollInfo.bVehTollType;
    TollCardTollInfoRaw.bShiftID = CardTollInfo.bShiftId;
    qMemCopy(TollCardTollInfoRaw.baPsamTermId, CardTollInfo.baPsamTermId, sizeof(CardTollInfo.baPsamTermId));
    TollCardTollInfoRaw.dwWriteCount=CardTollInfo.dwWriteCount;
    TollCardTollInfoRaw.dwCardBoxId=CardTollInfo.dwCardBoxId;
    TollCardTollInfoRaw.bVehPlateColor=CardTollInfo.bVehPlateColor;
    qMemCopy(TollCardTollInfoRaw.szVehPlate, CardTollInfo.szVehPlate,sizeof(TollCardTollInfoRaw.szVehPlate));
    MaintenanceAndbPassStatus_file2card(TollCardTollInfoRaw.bMaintenanceAndbPassStatus,  CardTollInfo.bMaintenance, CardTollInfo.bPassStatus);
    return true;
}
bool TCardFileConverter::flagStaInfo_file2card(QByteArray&  bFlagStaInfoRaw,const quint8 bFlagStaCount, const QByteArray& baFlagStaInfo)
{
    int  nullbytecount = 0;
    int i,j;
    quint8 head, rear;

    //第i个字节上的剩余位数
    j=-1;
    for( i=0 ;i<=8;i++)
    {
        nullbytecount = (i+1)*8 %7;
        if (nullbytecount==1)
        {
            j++;
        }
        if (nullbytecount==0)
        {
            nullbytecount=7;
        }
        head=baFlagStaInfo[j] << nullbytecount;
        if(j==9)
        {
            rear=0x00;
        }
        else
        {
            rear=baFlagStaInfo[j+1]>> (7-nullbytecount);
        }
        bFlagStaInfoRaw[i]=head | rear;
        j++;
    }
    return true;
}
void TCardFileConverter:: flagStaInfo_card2file(quint8 & bFlagStaCount, QByteArray  &baFlagStaInfo, const QByteArray& bFlagStaInfoRaw)
{
    int  stateCount;
    quint8  tmp,head,rear;
    int   flag, i, j;
    head= 0;
    stateCount= 0;
    j= -1;
    for ( i=0;i<= 9; i++)
    {
        flag= (i+1)*7% 8;
        if(i==0)
        {
            head= 0;
            j++;
        }
        if (i==8)
        {
            j--;
        }
        else
        {
            head = bFlagStaInfoRaw[j-1] << flag;
        }

        rear= bFlagStaInfoRaw[j] >> (8-flag);
        tmp =head | rear;
        baFlagStaInfo[i]= tmp & 0x7f;
        if (baFlagStaInfo[i]!=(char)0)
        {
            stateCount++;
        }
        j++;
    }
    bFlagStaCount =quint8(stateCount);
}

void TCardFileConverter:: MaintenanceAndbPassStatus_card2file(quint8 &bMaintenance, quint8 &bPassStatus, quint8  bMaintenanceAndbPassStatus)
{
    bMaintenance = (bMaintenanceAndbPassStatus & 0xf0) >> 4;
    bPassStatus= bMaintenanceAndbPassStatus & 0x0f;

}
void TCardFileConverter:: MaintenanceAndbPassStatus_file2card(quint8 &MaintenanceAndbPassStatus,const quint8 bMaintenance, quint8 bPassStatus)

{
    MaintenanceAndbPassStatus = ((bMaintenance & 0x0f) << 4) | (bPassStatus &0x0f);
}

bool TCardFileConverter::OpCardBasicInfoRaw2OpCardBasicInfo(TOpCardBasicInfo & OpCardBasicInfo, quint32 dwCardSerialId,const  TOpCardBasicInfoRaw& OpCardBasicInfoRaw)
{
    OpCardBasicInfo.M1BasicInfo.dwCardNum = OpCardBasicInfoRaw.M1BasicInfo.dwCardNum;
    OpCardBasicInfo.M1BasicInfo.wNetworkId = OpCardBasicInfoRaw.M1BasicInfo.wNetworkId;
    OpCardBasicInfo.M1BasicInfo.bVersion = OpCardBasicInfoRaw.M1BasicInfo.bVersion;
    OpCardBasicInfo.M1BasicInfo.bType = OpCardBasicInfoRaw.M1BasicInfo.bType;
    OpCardBasicInfo.M1BasicInfo.dwIssueOp = OpCardBasicInfoRaw.dwIssueOp;
    OpCardBasicInfo.M1BasicInfo.dwIssueOrg = OpCardBasicInfoRaw.dwIssueOrg;
    OpCardBasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    if(! CardTime2SystemTime(OpCardBasicInfoRaw.M1BasicInfo.dwStartTime, OpCardBasicInfo.M1BasicInfo.StartTime))
    {
        return false;
    }
    if (! CardTime2SystemTime(OpCardBasicInfoRaw.M1BasicInfo.dwExpireTime,OpCardBasicInfo.M1BasicInfo.ExpireTime))
    {
        return false;
    }
    return true;
}
//参  数: var OpCardBasicInfo: TOpCardBasicInfo;
//        dwCardSerialId: DWORD;
//        const OpCardBasicInfoRaw: TOpCardBasicInfoRaw;
//        const OpCardInfoRaw: TOpCardInfoRaw
//  返回值: Boolean
//  说  明: 身份卡基本信息转换（原始->逻辑）
bool TCardFileConverter::OpCardBasicInfoRaw2OpCardBasicInfo(TOpCardBasicInfo & OpCardBasicInfo, quint32 dwCardSerialId,const TOpCardBasicInfoRaw& OpCardBasicInfoRaw,const TOpCardInfoRaw& OpCardInfoRaw)
{
    //wumax == qMemSet(&OpCardBasicInfo,0,sizeof(OpCardBasicInfo));
    OpCardBasicInfo.clear();
    if (! OpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo, dwCardSerialId, OpCardBasicInfoRaw))
    {
        return false;

    }
    OpCardBasicInfo.szOpName = OpCardInfoRaw.szOpName;
    OpCardBasicInfo.dwOpId = OpCardInfoRaw.dwOpId;
    OpCardBasicInfo.szID = OpCardInfoRaw.szID;
    OpCardBasicInfo.bCRC= true;
    return true;
}
bool TCardFileConverter::Pro0012Raw2CardTollInfo(TCardTollInfo& CardTollInfo, const TPro0012Raw&  Pro0012Raw,const TPro0009Raw&  Pro0009Raw)
{
    quint32 dwTemp;
    quint8 *pB;
    CardTollInfo.clear();
    //	入口路网号，路网号=5301
    //liujian
    CardTollInfo.wNetworkId = bcd2dword((char*)&Pro0012Raw.wNetworkId,2);
    //	入口收费站编码
    CardTollInfo.dwStaId=bcd2dword((char*)Pro0012Raw.baStaId, 3);
    //	入口车道
    CardTollInfo.bLaneId= Pro0012Raw.bLaneId;
    //	通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    CardTollInfo. bPassStatus = Pro0012Raw.bPassStatus;
    //  维护发卡标识（0-正常发卡、1-维护发卡）
    //	入口时间
    CardTollInfo.PassTime=UnixTimeToDateTime2000(Pro0012Raw.dwPassTime) ;
    //	入口流水记录号
    CardTollInfo. bRecIndex=Pro0012Raw.bRecIndex ;
    //	入口操作员号
    CardTollInfo. dwOpId=Pro0012Raw.dwOpId ;
    //	入口班次
    CardTollInfo. bShiftId=Pro0012Raw.bShiftId;
    //	入口车型
    CardTollInfo.bVehClass=Pro0012Raw.bVehClass ;
    //	入口车类
    //	入口PSAM卡号
    qMemCopy(CardTollInfo.baPsamTermId,Pro0012Raw.baPsamTermId,6);

    //	车牌号码，无信息字节均为0x00
    qMemCopy(CardTollInfo.szVehPlate,Pro0012Raw.szVehPlate,sizeof(Pro0012Raw.szVehPlate));
    //	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色；0xFF-未输入颜色
    CardTollInfo. bVehPlateColor=Pro0012Raw.bVehPlateColor ;

    //	CRC校验
    CardTollInfo.bCRC=true ;
    return true;
}
bool  TCardFileConverter::OldProBasicRaw2ProCardBasicInfo(TProCardBasicInfo & ProCardBasicInfo,quint32  dwCardSerialId, const TOldPro0015Raw& Pro0015Raw)
{
    bool A= OldProBasicRaw2ProCardBasicInfo(ProCardBasicInfo, Pro0015Raw);
    ProCardBasicInfo.dwCardId = dwCardSerialId;
    return A;
}
bool TCardFileConverter::ProBasicRaw2ProCardBasicInfo(TProCardBasicInfo &ProCardBasicInfo,quint32 dwCardSerialId,const TPro0015Raw& Pro0015Raw, const TPro000ARaw& Pro000ARaw)
{
    bool rlt = ProBasicRaw2ProCardBasicInfo(ProCardBasicInfo, Pro0015Raw);
    if(!rlt)
    {
        return false;
    }
    //	卡片物理序号
    ProCardBasicInfo.dwCardId = dwCardSerialId;
    if (Pro000ARaw.bFreeType != 0xFF)
    {
        //	免费区间类型
        ProCardBasicInfo.bFreeRegionType= Pro000ARaw.bFreeType;
        //	免费区间数量
        ProCardBasicInfo.wFreeRegionNum= Pro000ARaw.wFreeRegionNum;
        //	免费区间（按位存储）
        qMemCopy(ProCardBasicInfo.FreeRegion,Pro000ARaw.baFreeRegion,sizeof(Pro000ARaw.baFreeRegion));
    }
    return true;
}
bool TCardFileConverter::ProCardBasicInfo2ProBasicRaw(TPro0015Raw &Pro0015Raw,TPro000ARaw & Pro000ARaw,const TProCardBasicInfo& ProCardBasicInfo)
{
    qMemSet(&Pro0015Raw, 0,sizeof(Pro0015Raw));
    qMemCopy(Pro0015Raw.IssueOrg,ProCardBasicInfo.szIssueOrgId,sizeof(Pro0015Raw.IssueOrg));//	8	发卡方唯一标识
    Pro0015Raw.bType= ProCardBasicInfo.bType; ;							//	9	卡片类型，22-储值卡（鲁通B卡）；23-记账卡（鲁通A卡）
    Pro0015Raw.bVersion=ProCardBasicInfo.bVersion ;							//	10	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    //	12	卡片网络编号
    Pro0015Raw.Network= Word2Bcd(ProCardBasicInfo.wNetworkId);
    Hex2Raw(Pro0015Raw.CardNo, ProCardBasicInfo.szCardNo, 16);						//	20	CPU卡片内部编号
    systime2bcd(Pro0015Raw.StartTime, 4, ProCardBasicInfo.StartTime);  	//	24	启用时间，格式：CCYYMMDD
    systime2bcd(Pro0015Raw.ExpireTime, 4, ProCardBasicInfo.ExpireTime);  	//	24	启用时间，格式：CCYYMMDD
    //	40	车牌号码
    qMemCopy(Pro0015Raw.szVehPlate,ProCardBasicInfo.szVehPlate,12);
    Pro0015Raw.bUserType=ProCardBasicInfo.bUserType ;						//	41	用户类型 0-普通用户; 6-全免鲁通C卡（国标为公务车用户）; 7-区间免费鲁通C卡
    Pro0015Raw. bDiscount=ProCardBasicInfo.bDiscount;				//	42	折扣率
    Pro0015Raw.bVehtypeAndPlateColor= ((ProCardBasicInfo.bVehPlateColor << 4) & 0xF0)
            | ( ProCardBasicInfo.bVehClass & 0x0F );	        //	43	车型/车牌颜色 低4位存车型（1 - 客1、2 - 客2、3 - 客3、4 - 客4、10 - 货车、11 - 货1、12 - 货2、13 - 货3、14 - 货4、15 - 货5、5 - 集1、6 - 集2，7～9保留），高4位存车牌色（ 0-蓝; 1–黄; 2–黑; 3–白）

    qMemSet(&Pro000ARaw, 0,sizeof(Pro000ARaw));
    //	免费区间类型
    Pro000ARaw.bFreeType= ProCardBasicInfo.bFreeRegionType;
    //	免费区间数量
    Pro000ARaw.wFreeRegionNum= ProCardBasicInfo.wFreeRegionNum;
    //	免费区间（按位存储）
    qMemCopy(Pro000ARaw.baFreeRegion,ProCardBasicInfo.FreeRegion,sizeof(Pro000ARaw.baFreeRegion));
    return true;
}
//{-------------------------------------------------------------------------------
//  参  数: var TollCardBasicInfo: TTollCardBasicInfo;  通行卡基本信息
//          dwCardSerialId: DWORD;                       卡物理序号
//          const TollCardBasicInfoRaw:   通行卡基本信息文件（原始）
//  返回值: boolean
//  说  明: 通行卡基本信息转换（原始->逻辑）
//-------------------------------------------------------------------------------}
bool TCardFileConverter::TollCardBasicInfoRaw2TollCardBasicInfo(
        TTollCardBasicInfo &TollCardBasicInfo, quint32 dwCardSerialId,
        const  TTollCardBasicInfoRaw& TollCardBasicInfoRaw)
{
    TollCardBasicInfo.bCRC= true;
    TollCardBasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    TollCardBasicInfo.M1BasicInfo.bVersion = TollCardBasicInfoRaw.M1BasicInfo.bVersion;
    TollCardBasicInfo.M1BasicInfo.wNetworkId = ORG_NETWORKID;
    TollCardBasicInfo.M1BasicInfo.bType = TollCardBasicInfoRaw.M1BasicInfo.bType;
    TollCardBasicInfo.M1BasicInfo.dwCardNum = TollCardBasicInfoRaw.M1BasicInfo.dwCardNum;
    if(! CardTime2SystemTime(TollCardBasicInfoRaw.M1BasicInfo.dwStartTime,
                             TollCardBasicInfo.M1BasicInfo.StartTime))
    {
        return false;
    }
    if(! CardTime2SystemTime(TollCardBasicInfoRaw.M1BasicInfo.dwExpireTime,
                             TollCardBasicInfo.M1BasicInfo.ExpireTime))
    {
        return false;
    }
    TollCardBasicInfo.M1BasicInfo.bIssueKeyVer = TollCardBasicInfoRaw.bIssueKeyVer;
    TollCardBasicInfo.M1BasicInfo.szIssueSAM = BCDToString(TollCardBasicInfoRaw.dwIssueSAM, 6);
    TollCardBasicInfo.M1BasicInfo.dwIssueOrg = TollCardBasicInfoRaw.dwIssueOrg;
    TollCardBasicInfo.M1BasicInfo.dwIssueOp = TollCardBasicInfoRaw.dwIssueOp;
    return true;
}

bool TCardFileConverter::TollCardTollInfoRaw2CardTollInfo(TCardTollInfo& CardTollInfo,const TTollCardTollInfoRaw& TollCardTollInfoRaw )

{
    qMemCopy(&CardTollInfo.dwStaId,TollCardTollInfoRaw.baStaId,3);
    CardTollInfo.wNetworkId = ORG_NETWORKID;
    CardTollInfo.bLaneId = TollCardTollInfoRaw.bLaneId;
    if(! CardTime2SystemTime(TollCardTollInfoRaw.dwPassTime,CardTollInfo.PassTime))
    {
        return false;
    }
    CardTollInfo.bRecIndex = TollCardTollInfoRaw.bRecIndex;
    CardTollInfo.bVehTollType = TollCardTollInfoRaw.bVehTollType;
    CardTollInfo.dwOpId = TollCardTollInfoRaw.dwOpId;
    CardTollInfo.bVehClass = TollCardTollInfoRaw.bVehClass;
    qMemCopy(CardTollInfo.baPsamTermId,TollCardTollInfoRaw.baPsamTermId,6);
    CardTollInfo.dwWriteCount = TollCardTollInfoRaw.dwWriteCount;
    qMemCopy(CardTollInfo.szVehPlate,TollCardTollInfoRaw.szVehPlate,12);
    CardTollInfo.dwCardBoxId = TollCardTollInfoRaw.dwCardBoxId;
    CardTollInfo.bVehPlateColor = TollCardTollInfoRaw.bVehPlateColor;
    qMemCopy(CardTollInfo.baFlagStaInfo,TollCardTollInfoRaw.baFlagStaInfo,9);
    CardTollInfo.bShiftId = TollCardTollInfoRaw.bShiftID;
    CardTollInfo.bCRC= (qLoByte(TollCardTollInfoRaw.wCRC) != 0);
    MaintenanceAndbPassStatus_card2file(CardTollInfo.bMaintenance,
                                        CardTollInfo.bPassStatus,TollCardTollInfoRaw.bMaintenanceAndbPassStatus);
    return true;
}
bool TCardFileConverter::BoxCardAppInfoRaw2BoxCardAppInfo(TBoxCardAppInfo & AppInfo,quint32  dwCardNum,const  TBoxCardAppInfoRaw& AppInfoRaw)
{
    AppInfo.dwPersonizeOrg = AppInfoRaw.dwPersonizeOrg;
    if(! CardTime2SystemTime(AppInfoRaw.dwPersonizeTime,
                             AppInfo.PersonizeTime))
    {
        return false;
    }
    AppInfo.dwBoxID = dwCardNum;
    AppInfo.dwCount = AppInfoRaw.dwCount;
    return true;
}
bool  TCardFileConverter::BoxCardBasicInfoRaw2BoxCardBasicInfo(TBoxCardBasicInfo & BasicInfo, quint32 dwCardSerialId,const TBoxCardBasicInfoRaw& BasicInfoRaw)
{
    BasicInfo.bCRC = true;
    BasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    BasicInfo.M1BasicInfo.bVersion = BasicInfoRaw.M1BasicInfo.bVersion;
    BasicInfo.M1BasicInfo.wNetworkId= ORG_NETWORKID;//Bcd2Word(BasicInfoRaw.M1BasicInfo.wNetworkId);
    BasicInfo.M1BasicInfo.bType = BasicInfoRaw.M1BasicInfo.bType;
    BasicInfo.M1BasicInfo.dwCardNum = BasicInfoRaw.M1BasicInfo.dwCardNum;
    if(! CardTime2SystemTime(BasicInfoRaw.M1BasicInfo.dwStartTime,
                             BasicInfo.M1BasicInfo.StartTime))
    {
        return false;
    }
    if(! CardTime2SystemTime(BasicInfoRaw.M1BasicInfo.dwExpireTime,
                             BasicInfo.M1BasicInfo.ExpireTime))
    {
        return false;

    }
    BasicInfo.M1BasicInfo.bIssueKeyVer = BasicInfoRaw.bIssueKeyVer;
    BasicInfo.M1BasicInfo.szIssueSAM = BCDToString(BasicInfoRaw.dwIssueSAM, 6);
    BasicInfo.M1BasicInfo.dwIssueOrg = BasicInfoRaw.dwIssueOrg;
    BasicInfo.M1BasicInfo.dwIssueOp = BasicInfoRaw.dwIssueOp;
    return true;
}
bool TCardFileConverter::BoxCardAppInfo2BoxCardAppInfoRaw(TBoxCardAppInfoRaw &AppInfoRaw,const TBoxCardAppInfo& AppInfo)
{
    AppInfoRaw.dwPersonizeOrg = AppInfo.dwPersonizeOrg;
    AppInfoRaw.dwPersonizeTime = SystemTime2CardTime(AppInfo.PersonizeTime);
    AppInfoRaw.dwBoxID = AppInfo.dwBoxID;
    AppInfoRaw.dwCount = AppInfo.dwCount;
    return true;
}
//{-------------------------------------------------------------------------------
//  参  数: var OpCardInfoRaw: TOpCardInfoRaw;
//        const OpCardBasicInfo: TDLLOpCardBasicInfo
//  返回值: boolean
//  说  明: 身份卡基本信息转换（逻辑->原始）02 由于01无权限写故未添加  --用于DLL，逻辑结构不同
//-------------------------------------------------------------------------------}
bool TCardFileConverter::DLLOpCardBasicInfoO2CardBasicInfoRaw( TOpCardInfoRaw& OpCardInfoRaw, const TDLLOpCardBasicInfo& OpCardBasicInfo)
{
    OpCardInfoRaw.dwOpId = OpCardBasicInfo.dwOpId;                       //	4	操作员工号
    qMemCopy(OpCardInfoRaw.szOpName,
             OpCardBasicInfo.szOpName,qMin(sizeof(OpCardInfoRaw.szOpName),
                                           sizeof(OpCardBasicInfo.szOpName)));
    qMemCopy(OpCardInfoRaw.szID,OpCardBasicInfo.szID,
             qMin(sizeof(OpCardInfoRaw.szID), sizeof(OpCardBasicInfo.szID)));
    return true;
}

bool TCardFileConverter::OpCardAppInfoRaw2OpCardAppInfo(TOpCardAppInfo &OpCardAppInfo, const TOpCardAppInfoRaw& OpCardAppInfoRaw)
{
    int i = 0;
    OpCardAppInfo.dwOrg = OpCardAppInfoRaw.dwOrg;
    OpCardAppInfo.bAuthorization = OpCardAppInfoRaw.bAuthorization;
    OpCardAppInfo.bWorkStatus = OpCardAppInfoRaw.bWorkStatus;
    OpCardAppInfo.dwPersonizeOrg = OpCardAppInfoRaw.dwPersonizeOrg;

    for(i = 0; i < 16; i++)
    {
        OpCardAppInfo.szPassword = OpCardAppInfo.szPassword +
                QObject::tr("%1").arg(OpCardAppInfoRaw.baPassword[i],2,16,QLatin1Char('0'));

    }
    if(!CardTime2SystemTime(OpCardAppInfoRaw.dwPersonizeTime, OpCardAppInfo.PersonizeTime))
    {
        return false;
    }
    OpCardAppInfo.bCRC = true;
    return true;
}
//{-------------------------------------------------------------------------------
//  参  数: var OpCardBasicInfo: TDLLOpCardBasicInfo;
//          dwCardSerialId: DWORD;
//          const OpCardBasicInfoRaw : TOpCardBasicInfoRaw;
//          const OpCardInfoRaw : TOpCardInfoRaw
//  返回值: Boolean
//  说  明: 用于DLL，逻辑结构不同
//-------------------------------------------------------------------------------}
bool TCardFileConverter::DLLOpCardBasicInfoRaw2OpCardBasicInfo(TDLLOpCardBasicInfo & OpCardBasicInfo,quint32  dwCardSerialId,const  TOpCardBasicInfoRaw& OpCardBasicInfoRaw ,const  TOpCardInfoRaw& OpCardInfoRaw)
{
    qMemCopy(&OpCardBasicInfo,0,sizeof(OpCardBasicInfo));
    if (! DLLOpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo, dwCardSerialId, OpCardBasicInfoRaw))
    {
        return false;

    }
    qMemCopy(OpCardBasicInfo.szOpName,OpCardInfoRaw.szOpName,sizeof(OpCardInfoRaw.szOpName));

    OpCardBasicInfo.dwOpId = OpCardInfoRaw.dwOpId;
    qMemCopy(OpCardBasicInfo.szID,OpCardInfoRaw.szID,sizeof(OpCardInfoRaw.szID));

    OpCardBasicInfo.bCRC= true;
    return true;
}
bool TCardFileConverter::DLLOpCardBasicInfoRaw2OpCardBasicInfo(
        TDLLOpCardBasicInfo& OpCardBasicInfo, quint32 dwCardSerialId,
        const TOpCardBasicInfoRaw& OpCardBasicInfoRaw)
{
    QDateTime dtTemp;
    QString sTemp;
    OpCardBasicInfo.clear();
    OpCardBasicInfo.M1BasicInfo.dwCardNum = OpCardBasicInfoRaw.M1BasicInfo.dwCardNum;
    //liujian
    OpCardBasicInfo.M1BasicInfo.wNetworkId = bcd2dword((char*)&OpCardBasicInfoRaw.M1BasicInfo.wNetworkId,2);
    OpCardBasicInfo.M1BasicInfo.bVersion = OpCardBasicInfoRaw.M1BasicInfo.bVersion;
    OpCardBasicInfo.M1BasicInfo.bType = OpCardBasicInfoRaw.M1BasicInfo.bType;
    OpCardBasicInfo.M1BasicInfo.dwIssueOp = OpCardBasicInfoRaw.dwIssueOp;
    OpCardBasicInfo.M1BasicInfo.dwIssueOrg = OpCardBasicInfoRaw.dwIssueOrg;
    OpCardBasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    if(! CardTime2SystemTime(OpCardBasicInfoRaw.M1BasicInfo.dwStartTime, dtTemp))
    {
        return false;

    }
    QDateTime2SysTime(dtTemp, &OpCardBasicInfo.M1BasicInfo.StartTime);

    if (! CardTime2SystemTime(OpCardBasicInfoRaw.M1BasicInfo.dwExpireTime, dtTemp))
    {
        return false;

    }
    QDateTime2SysTime(dtTemp, &OpCardBasicInfo.M1BasicInfo.ExpireTime);

    sTemp = BCDToString(OpCardBasicInfoRaw.szIssueSAM,6);
    qMemCopy(OpCardBasicInfo.M1BasicInfo.szIssueSAM,sTemp.toLocal8Bit().data(),qMin(sizeof(OpCardBasicInfo.M1BasicInfo.szIssueSAM),(unsigned int)sTemp.length()));
    return true;
}
//{-------------------------------------------------------------------------------
//  参  数: var TollCardBasicInfo: TTollCardBasicInfo;  旧通行卡基本信息
//          dwCardSerialId: DWORD;                      旧通卡卡物理序号
//          const TollCardBasicInfoRaw: TOldTollCardBasicInfoRaw  旧通行卡基本信息文件（原始）
//  返回值: boolean
//  说  明: 旧通行卡基本信息转换(原始->逻辑)
//-------------------------------------------------------------------------------}
bool TCardFileConverter::Old_TollCardBasicInfoRaw2TollCardBasicInfo(
        TTollCardBasicInfo& TollCardBasicInfo,quint32 dwCardSerialId,
        const TOldTollCardBasicInfoRaw& TollCardBasicInfoRaw)
{
    TollCardBasicInfo.bOldCard = true;
    TollCardBasicInfo.bCRC = true;
    TollCardBasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    TollCardBasicInfo.M1BasicInfo.bVersion = TollCardBasicInfoRaw.M1BasicInfo.bVersion;
    TollCardBasicInfo.M1BasicInfo.wNetworkId =swapWORD(TollCardBasicInfoRaw.M1BasicInfo.wNetworkId);
    if (TollCardBasicInfo.M1BasicInfo.wNetworkId !=OLD_ORG_NETWORKID)
    {
        return false;
    }
    TollCardBasicInfo.M1BasicInfo.wNetworkId = ORG_NETWORKID;
    TollCardBasicInfo.M1BasicInfo.bType = TollCardBasicInfoRaw.M1BasicInfo.bType;
    TollCardBasicInfo.M1BasicInfo.dwCardNum =swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwCardNum);
    if(! CardTime2SystemTime(swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwStartTime),
                             TollCardBasicInfo.M1BasicInfo.StartTime))
    {
        return false;

    }
    if(! CardTime2SystemTime(swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwExpireTime),
                             TollCardBasicInfo.M1BasicInfo.ExpireTime))
    {
        return false;

    }
    TollCardBasicInfo.M1BasicInfo.bIssueKeyVer = TollCardBasicInfoRaw.bIssueKeyVer;
    TollCardBasicInfo.M1BasicInfo.szIssueSAM = QObject::tr("%1").arg(swapDword(TollCardBasicInfoRaw.dwIssueSAM));
    return true;
}

bool TCardFileConverter::Old_BoxCardBasicInfoRaw2BoxCardBasicInfo(
        TBoxCardBasicInfo & BasicInfo,quint32 dwCardSerialId,
        const TOldBoxCardBasicInfoRaw& BasicInfoRaw)
{
    bool bRet = Old_TollCardBasicInfoRaw2TollCardBasicInfo(BasicInfo, dwCardSerialId, BasicInfoRaw);
    if (BasicInfo.M1BasicInfo.bType == CARD_TYPE_OLD_BOX_CARD)
        BasicInfo.M1BasicInfo.bType = CARD_TYPE_BOX_CARD;
    return bRet;
}
bool TCardFileConverter::Old_BoxCardAppInfoRaw2BoxCardAppInfo(
        TBoxCardAppInfo & AppInfo,quint32 dwCardNum,
        const TOldTollCardTollInfo2001Raw& AppInfoRaw)
{
    AppInfo.dwBoxID = dwCardNum;
    AppInfo.dwCount = swapDword(AppInfoRaw.Counter);
    AppInfo.bCRC= true;
    return  true;
}

bool TCardFileConverter::Old_MonthCardBasicInfoRaw2MonthCardBasicInfo(
        TMonthCardBasicInfo &TollCardBasicInfo,quint32 dwCardSerialId,
        const TOldTollCardBasicInfoRaw& TollCardBasicInfoRaw )
{
    TollCardBasicInfo.bCRC = true;
    TollCardBasicInfo.M1BasicInfo.dwCardId = dwCardSerialId;
    TollCardBasicInfo.M1BasicInfo.bVersion = TollCardBasicInfoRaw.M1BasicInfo.bVersion;
    TollCardBasicInfo.M1BasicInfo.wNetworkId = swapWORD(TollCardBasicInfoRaw.M1BasicInfo.wNetworkId);
    if (TollCardBasicInfo.M1BasicInfo.wNetworkId != OLD_ORG_NETWORKID)
    {
        LogMsg("lane",QObject::tr("旧包交卡网络编号不合法:%d").arg(TollCardBasicInfo.M1BasicInfo.wNetworkId));
        TollCardBasicInfo.M1BasicInfo.wNetworkId = ORG_NETWORKID;
        TollCardBasicInfo.M1BasicInfo.bType = TollCardBasicInfoRaw.M1BasicInfo.bType;
        TollCardBasicInfo.M1BasicInfo.dwCardNum = swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwCardNum);
        if(!CardTime2SystemTime(swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwStartTime),
                                TollCardBasicInfo.M1BasicInfo.StartTime))
        {
            return false;
        }
        if(!CardTime2SystemTime(swapDword(TollCardBasicInfoRaw.M1BasicInfo.dwExpireTime),
                                TollCardBasicInfo.M1BasicInfo.ExpireTime))
        {
            return false;
        }
        TollCardBasicInfo.M1BasicInfo.bIssueKeyVer = TollCardBasicInfoRaw.bIssueKeyVer;
        TollCardBasicInfo.M1BasicInfo.szIssueSAM = QObject::tr("%d").arg(swapDword(TollCardBasicInfoRaw.dwIssueSAM));
        return true;
    }
    else
        return true;
}
bool TCardFileConverter::OldProBasicRaw2ProCardBasicInfo(TProCardBasicInfo& ProCardBasicInfo,const TOldPro0015Raw& Pro0015Raw )
{
    qint64 dwTemp;
    ProCardBasicInfo.bOldCard = true;
    //	卡片物理序号（ETC无法获取）
    //dwCardId:= dwCardSerialId;
    //	发卡方唯一标识
    qMemCopy(ProCardBasicInfo.szIssueOrgId,Pro0015Raw.IssueOrg,sizeof(Pro0015Raw.IssueOrg));
    //	卡片类型，22-储值卡；23-记账卡
    ProCardBasicInfo. bType=Pro0015Raw.bType;
    //	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    ProCardBasicInfo.  bVersion=Pro0015Raw.bVersion ;
    //	卡片网络编号
    ProCardBasicInfo.wNetworkId = swapWORD(Pro0015Raw.Network);
    if (ProCardBasicInfo.wNetworkId != OLD_ORG_NETWORKID)
    {
        LogMsg("lane",QObject::tr("旧非现金卡基本信息网络编号错误：%1").arg(ProCardBasicInfo.wNetworkId));
    }
    ProCardBasicInfo. wNetworkId = ORG_NETWORKID;
    //	CPU卡片内部编号,老卡用了4位，整形存放，前面存入网络编号
    dwTemp =swapDword(Pro0015Raw.CardNo);
    qMemCopy(ProCardBasicInfo.szCardNo, &dwTemp, 8);
    // IntToArChar(szCardNo[0], 8, dwTemp);
    //	启用时
    ProCardBasicInfo. StartTime=UnixTimeToDateTime1970(Pro0015Raw.StartTime);
    //	到期时间
    ProCardBasicInfo. ExpireTime=UnixTimeToDateTime1970(Pro0015Raw.ExpireTime);
    //	车牌号码
    qMemCopy(ProCardBasicInfo.szVehPlate,Pro0015Raw.szVehPlate ,sizeof(Pro0015Raw.szVehPlate));
    //	用户类型 0-普通用户; 6-全免卡（国标为公务车用户）; 7-区间免费
    ProCardBasicInfo. bUserType=Pro0015Raw.bUserType;
    // 旧CPU卡中记账卡为全免公务卡，不判断有效期
    if (ProCardBasicInfo.bType == CARD_TYPE_TALLY_CARD)
    {
        //  免费范围用于公务卡。0－省内，1－指定路网或路段，专用于公务卡，不用可填0xff
        if (ProCardBasicInfo.bUserType== 0 )
        {
            ProCardBasicInfo. bUserType= CARD_UserType_FullFree;

        }
        ProCardBasicInfo. StartTime= QDateTime(QDate(2000,1,1), QTime(0,0,0));
        ProCardBasicInfo.   ExpireTime= QDateTime(QDate(2300, 1, 1),QTime(0,0,0));
    }
    //	车牌颜色，1-蓝色；2-黄色；3-黑色；4-白色
    ProCardBasicInfo.  bVehPlateColor = 0;

    //	车型
    ProCardBasicInfo.  bVehClass= 0 ;

    //	免费车类，1-公务；2-联合收割；3-防汛；4-防火；5-救灾；6-临时免费；7-电煤
    ProCardBasicInfo.  bVehTollType= 0 ;

    //	免费区间类型
    ProCardBasicInfo. bFreeRegionType= 0;

    //	免费区间数量
    ProCardBasicInfo. wFreeRegionNum= 0;

    //	免费区间（按位存储）
    return true;
}
bool TCardFileConverter::ProBasicRaw2ProCardBasicInfo(
        TProCardBasicInfo &ProCardBasicInfo,
        const TPro0015Raw& Pro0015Raw )
{
    ProCardBasicInfo.bOldCard = false;
    //	卡片物理序号(ETC无法获取)

    //	发卡方唯一标识
    qMemCopy(ProCardBasicInfo.szIssueOrgId,Pro0015Raw.IssueOrg,sizeof(Pro0015Raw.IssueOrg));
    //	卡片类型，22-储值卡；23-记账卡
    ProCardBasicInfo.bType=Pro0015Raw.bType;
    //	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    ProCardBasicInfo.bVersion=Pro0015Raw.bVersion ;
    //	卡片网络编号
    //ProCardBasicInfo.wNetworkId = Bcd2Word(Pro0015Raw.Network);
    ProCardBasicInfo.wNetworkId = bcd2dword((char*)&Pro0015Raw.Network,2);
    //	CPU卡片内部编号
    Raw2HexStr(ProCardBasicInfo.szCardNo, Pro0015Raw.CardNo, sizeof(Pro0015Raw.CardNo));
    //	启用时间
    bcd2systime(&ProCardBasicInfo.StartTime,Pro0015Raw.StartTime,4);
    //	到期时间
    bcd2systime(&ProCardBasicInfo.ExpireTime,Pro0015Raw.ExpireTime,4);
    //	车牌号码
    qMemCopy(ProCardBasicInfo.szVehPlate,Pro0015Raw.szVehPlate,sizeof(Pro0015Raw.szVehPlate));
    //	用户类型 0-普通用户; 6-全免卡（国标为公务车用户）; 7-区间免费
    ProCardBasicInfo.bUserType=Pro0015Raw.bUserType;
    // 折扣率
    ProCardBasicInfo.bDiscount = Pro0015Raw.bDiscount;
    //	车型
    ProCardBasicInfo.bVehClass=Pro0015Raw.bVehtypeAndPlateColor & 0x0F;
    //	车牌颜色，1-蓝色；2-黄色；3-黑色；4-白色
    ProCardBasicInfo.bVehPlateColor=(Pro0015Raw.bVehtypeAndPlateColor >> 4) & 0x0F;
    return true;
}

bool TCardFileConverter::Old_ProCardBasicInfo2ProBasicRaw( TOldPro0015Raw &Pro0015Raw,const TProCardBasicInfo& ProCardBasicInfo)
{
    quint64 dwTemp;
    //	8	发卡方唯一标识
    qMemCopy(Pro0015Raw.IssueOrg,ProCardBasicInfo.szIssueOrgId,sizeof(Pro0015Raw.IssueOrg));
    Pro0015Raw. bType= ProCardBasicInfo.bType;							//	9	卡片类型，22-储值卡（鲁通B卡）；23-记账卡（鲁通A卡）
    Pro0015Raw. bVersion=ProCardBasicInfo.bVersion ;							//	10	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    Pro0015Raw.  Network=swapWORD(OLD_ORG_NETWORKID);					//	12	卡片网络编号
    dwTemp = ArCharToLong(ProCardBasicInfo.szCardNo, sizeof(ProCardBasicInfo.szCardNo));
    Pro0015Raw. CardNo=swapDword(dwTemp);						//	20	CPU卡片内部编号   老卡用了4位，整形存放
    Pro0015Raw.  StartTime=DateTimeToUnixTime1970(ProCardBasicInfo.StartTime);					//	24	启用时间，Unix时间
    Pro0015Raw.ExpireTime=DateTimeToUnixTime1970(ProCardBasicInfo.ExpireTime);					//	28	到期时间，Unix时间

    qMemCopy(Pro0015Raw.szVehPlate,ProCardBasicInfo.szVehPlate,sizeof(Pro0015Raw.szVehPlate));
    Pro0015Raw. bUserType=0 ;						//	41	用户类型
    Pro0015Raw. bDiscount=0;				//	42	折扣率
    Pro0015Raw. bFCI=0;	        //	43	00
    return true;

}

bool TCardFileConverter::OpCardAppInfo2OpCardAppInfoRaw(TOpCardAppInfoRaw &OpCardAppInfoRaw, const TOpCardAppInfo& OpCardAppInfo)
{
    int tmpInt, i;
    QString tmpStr;
    OpCardAppInfoRaw.bDataFileVersion =  OpCardAppInfo.bDataFileVersion;             //  1   数据文件版本号
    OpCardAppInfoRaw.dwOrg =OpCardAppInfo.dwOrg;						            //	5	所在机构，身份卡首次制作时写入0x00，下级可以更改
    OpCardAppInfoRaw.bAuthorization=OpCardAppInfo.bAuthorization ;				        //	6	操作权限
    OpCardAppInfoRaw.bWorkStatus = OpCardAppInfo.bWorkStatus;					          //	7	工作状态
    OpCardAppInfoRaw.dwPersonizeOrg = OpCardAppInfo.dwPersonizeOrg;			          //	11	个性化机构编码，保留
    OpCardAppInfoRaw.dwPersonizeTime = SystemTime2CardTime(OpCardAppInfo.PersonizeTime);  //	15	个性化时间，保留
    for (i=0; i <= 15;i++)
    {
        tmpStr = OpCardAppInfo.szPassword.mid(2*i+1, 2);
        bool ok = false;
        tmpInt = tmpStr.toInt(&ok,16);
        if (!ok)
        {
            // Logger.Error('转换操作员密码发生错误，无法转换成数字:'+tmpStr);
            return false;
        }
        OpCardAppInfoRaw.baPassword[i] = quint8(tmpInt);
    }
    return true;
}

bool TCardFileConverter::OpCardBasicInfoO2CardBasicInfoRaw(TOpCardInfoRaw &OpCardInfoRaw, const TOpCardBasicInfo& OpCardBasicInfo)
{
    return true;
}
