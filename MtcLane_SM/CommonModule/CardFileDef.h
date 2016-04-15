#ifndef CARDFILEDEF_H
#define CARDFILEDEF_H
#include <QObject>
#include <QDateTime>
#include <QString>
#include "LaneType.h"


////////////卡类型定义 ////////
//通行卡
const int  CARD_TYPE_PASS_CARD = 0;
//标签卡
const int  CARD_TYPE_BOX_CARD = 6;
//储值卡
const int CARD_TYPE_STORE_CARD = 22;
//记账卡
const int CARD_TYPE_TALLY_CARD = 23;
//全省免交
const int CARD_TYPE_ALLFREE_CARD = 26;
//全省包交
const int CARD_TYPE_ALL_BAOJIAO_CARD = 27;
//路段免交储值卡
const int CARD_TYPE_ROAD_FREE_STORE = 28;
//路段包交储值卡
const int CARD_TYPE_ROAD_BAOJIAO_STORE = 29;
//路段免交记帐卡
const int CARD_TYPE_ROAD_FREE_TALLY = 30;
//路段包交记帐卡
const int CARD_TYPE_ROAD_BAOJIAO_TALLY = 31;
//身份卡
const int CARD_TYPE_OP_CARD = 51;
//纸卡
const int CARD_TYPE_PAPER = 9;

//旧标签卡
const int CARD_TYPE_OLD_BOX_CARD = 26;
//旧包交卡
const int CARD_TYPE_MONTH_CARD = 3;

// 云通包交卡用户类型
const int CARD_UserType_FullFree=5;		// 全免用户
const int CARD_UserType_RegionFree=7;	// 区间用户

const int FILE_LEN_0015_GB = 43;
const int FILE_LEN_0012_GB = 40;
const int FILE_LEN_0019_GB = 43;  //FILE_LEN_0019_GB = FILE_LEN_0012_GB+3;

enum FREE_REGION_TYPE
{
    frNone=0,
    frRoad=1,		//	1-路段
    frAdmRegion=2,			//	2-行政区域
    frNeighborSta=3,		//	3-相邻站
    frNeighborStaPlus=4		//	4-相邻站+入出口
};

#pragma pack(push,1)
// M1卡通用基本信息
struct  TM1BasicInfoRaw
{
    quint8 bVersion ;						  //	1	卡片版本号=101
    quint16 wNetworkId;				  	//	3	网络编号，发行该卡的收费网络编号=5301
    quint8   bType;							    //	4	卡片类型，0-通行卡；51-身份卡  26-标签卡
    quint32  dwCardNum ;					  //	8	卡号
    quint32 dwStartTime ;					//	12	启用时间，采用UNIX时间格式，从1970年01月01日零时开始
    quint32 dwExpireTime;					//	16	到期时间，采用UNIX时间格式，从1970年01月01日零时开始

};
// 通行卡基本信息文件	01
typedef struct
{
    TM1BasicInfoRaw  M1BasicInfo;		    //	16	M1卡通用基本信息
    quint8 bIssueKeyVer;                  //  17  发行密钥版本号
    quint8 dwIssueSAM[6];                   //  21  发行SAM卡编号
    quint32 dwIssueOrg ;				        	//	25	发行机构编码
    quint32  dwIssueOp  ;					          //	29	发行操作员编码
    quint8  Reserve;				//	32	保留
    quint8  Reserve2[14];			//	46	保留
    quint16  wCRC;							            //	48	CRC校验
}TTollCardBasicInfoRaw,TBoxCardBasicInfoRaw;
//老通行卡基本信息文件	1001
typedef struct
{
    TM1BasicInfoRaw  M1BasicInfo ;		    //	16	M1卡通用基本信息
    quint8  bIssueKeyVer ;                  //  17  发行密钥版本号
    quint32 dwIssueSAM;                   //  21  发行SAM卡编号
    quint32 dwVerifyCode  ;				        //	25	校验码
    quint16 wFree1 ;					          //	29	免费开始站1
    quint16 wFree2 ;                   //  31  免费结束站2
    quint8  Reserve[3];				//	32	保留
    quint8   Reserve2[16];			//	46	保留
}TOldBoxCardBasicInfoRaw,TOldTollCardBasicInfoRaw;// ;//旧标签卡基本信息文件 2001
// 标签卡基本信息文件	1001
// 标签卡附属信息文件定义2001
struct TBoxCardAppInfoRaw
{
    quint32  dwPersonizeOrg ;				        	//个性化机构
    quint32 dwPersonizeTime ;					          //个性化时间
    quint32 dwBoxID;                       //卡盒编号
    quint32  dwCount;                       //卡盒卡数
    quint8 Reserve1[16];			//保留
    quint8 Reserve2[16];			//保留
    quint8 Reserve3[16];			//保留
    quint8 Reserve4[16];			//保留
    quint8 Reserve5[16];			// 保留
    quint16 wCRC;                 //CRC校验
};
// 身份卡基本信息文件	1001
struct TOpCardBasicInfoRaw
{
    TM1BasicInfoRaw M1BasicInfo;		    //	16	M1卡通用基本信息
    quint8  bIssueKeyVer;                  //  17  发行密钥版本号
    quint8 szIssueSAM [6];     //  21  发行SAM卡编号
    quint32 dwIssueOrg  ;					        //	25	发行机构编码
    quint32   dwIssueOp ;					          //	29	发行操作员编码
    quint8  Reserve1 ;			                //	32	保留，填0x00
    quint8  Reserve2[13];      //  45  保留
    quint8 wCRC [3];          //	48	CRC校验
};
// 身份卡信息文件定义  2001
struct TOpCardInfoRaw
{
    quint32 dwOpId;                       //	4	操作员工号
    char  szOpName [12];			//	16	姓名，无信息字节均为0x00
    char  szID[18];        //  34  身份证号
    quint8  Reserve[11];       //  45  保留
    quint8  wCRC [3];						//	48	CRC校验
};
// 身份卡附属信息文件定义	3001
struct  TOpCardAppInfoRaw
{
    quint8   bDataFileVersion  ;             //  1   数据文件版本号
    quint32 dwOrg ;						            //	5	所在机构，身份卡首次制作时写入0x00，下级可以更改
    quint8 bAuthorization ;				        //	6	操作权限
    quint8 bWorkStatus;					          //	7	工作状态
    quint32 dwPersonizeOrg ;			          //	11	个性化机构编码，保留
    quint32 dwPersonizeTime;			        //	15	个性化时间，保留
    quint8 Reserve;					              //	16	保留
    quint8 baPassword[16];		//	32	操作员密码
    quint8 Reserve2 [13];			//	45	保留
    quint8 wCRC[3];					  //	48	CRC校验
};

// 通行卡联网收费信息文件定义	02
struct    TTollCardTollInfoRaw
{
    quint8   baStaId[3];					//	入口收费站编码
    quint8   bLaneId;						              //	入口车道
    quint8   bMaintenanceAndbPassStatus;	    //	5	高4位存储维护发卡标识（0-正常发卡、1-维护发卡）；低4位存储入出状态或入口车道类型（0-出口收回、1-封闭式入口、5-便携式入口、7-手持入口、9-无人值守入口）
    quint32  dwPassTime;					//	9	入/出时间，采用UNIX时间，从1970年01月01日零时开始
    quint8   bRecIndex;						//	10	入口流水记录号
    quint32  dwOpId;						  //	14	操作员号
    quint8   bShiftID;            //  15  班次
    quint8   bVehClass;						//	16	入口车型
    quint8   bVehTollType;				//	17	入口车类
    quint8   baPsamTermId[6];				//	23	入口PSAM卡号
    quint32  dwWriteCount;					//	27	卡片使用次数，写信息时累加
    quint32  dwCardBoxId;					//	31	卡箱逻辑编号
    quint8   bVehPlateColor;				//	32	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色；0xFF-未输入颜色
    char     szVehPlate[12];			//	44	车牌号码，无信息字节均为0x00
    quint8   Reserve[4];					//	48	保留，填0x00
    quint8   baFlagStaInfo[9];		//	57	标识站信息，标识站采用7位编码，使用“0000001B”-“1111111B” 表示127个标识站，“0000000B”保留为无标识站。9个字节可顺序存储10个标识站。最后两位置零。
    quint8   Reserve2[7];					//	64	保留，填0x00
    quint8   Reserve3[16];				//	80	保留，填0x00
    quint8   Reserve4[14];				//	94	保留，填0x00
    quint16   wCRC ;							//	96	CRC校验
};

//原M1卡片基本信息附属文件，只处理与包交卡有关信息
struct  TOldTollCardTollInfo1002Raw
{
    quint32  OperatorID;   //收费员工号(不处理)
    quint16 Station;      //收费站(不处理)
    char OperatorName[10]; //卡片持有者名(不处理)
    char VehiclePlate[12];    //车牌号码
    char   Reserve[16];        //保留
    quint8 VehicleClass;     //车型
    quint8   FreeRange;        //免费范围用于公务卡。0－省内，1－指定路网或路段，专用于公务卡，不用可填0xff
    quint16  FreeNetID;        //免费路网编号
};

struct  TOldTollCardTollInfo2001Raw
{
    quint16  EnNetID;                          //入口网络编码
    quint16  EnStation;                       //入口收费站编码
    quint8  EnLaneID;                         //入口收费站车道
    quint32  EnTime;                          //入口时间
    quint8  EnOperatorID[3];          //入口收费员编码
    quint8  EnShiftID;                        //入口班次
    quint8 EnVehClass;                       //入口车型
    quint8 EnVehStatus;                      //入口车种
    quint8  PortFlag;
    quint8 Reserve[16];
    char  EnCarNo[3];
    quint8  CheckPiont[9];
    quint32  Counter;
};

struct  TOldTollCardTollInfo2002Raw
{
    quint32  EnSAMNo;                   //车道SAM编号
    char EnVehPlate[12];      //入口自动车牌
    quint8  EnDirect;                    //入口方向
    quint32   ExTime;                    //出口时间
    quint16  ExNetID;                    //出口网络编号
    quint16  ExStation;                  //出口收费站编号
    char ExOperatorID[3];               //出口收费员工号
    quint32 CardBoxNo;                 //卡盒号
    quint8 ImageNo1[8];        //图片编号1
    quint8 ImageNo2[8];        //图片编号2
};
struct TOldTollCardTollInfoRaw
{
    TOldTollCardTollInfo2001Raw File2001;
    TOldTollCardTollInfo2002Raw File2002;
};
// DF01持卡人基本数据文件

struct TPro0016Raw
{
    quint8 bOwnerIdFlag;					//	1	持卡人身份标识
    quint8 bEmployeeFlag ;					//	2	本系统职工标识
    char  szOwnerName[20];				//	22	持卡人姓名
    char  szOwnerIdNo[32];				//	54	持卡人证件号码
    quint8   bOwnerIdType ;					//	55	持卡人证件类型
};
// DF01卡片发行基本数据文件
typedef struct
{
    char IssueOrg[8];					//	8	发卡方唯一标识
    quint8  bType ;							//	9	卡片类型，22-储值卡（鲁通B卡）；23-记账卡（鲁通A卡）
    quint8  bVersion ;							//	10	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    quint16  Network;					//	12	卡片网络编号
    quint8  CardNo[8];						//	20	CPU卡片内部编号
    char StartTime[4];					//	24	启用时间，格式：CCYYMMDD
    char ExpireTime[4];					//	28	到期时间，格式：CCYYMMDD
    char szVehPlate[12];				//	40	车牌号码
    quint8  bUserType ;						//	41	用户类型 0-普通用户; 6-全免云通卡（国标为公务车用户）; 7-区间免费云通卡
    quint8 bDiscount;				//	42 折扣率
    quint8 bVehtypeAndPlateColor;	        //	43	车型/车牌色 低4位存车型（1 - 客1、2 - 客2、3 - 客3、4 - 客4、10 - 货车、11 - 货1、12 - 货2、13 - 货3、14 - 货4、15 - 货5、5 - 集1、6 - 集2，7～9保留），高4位存车牌颜色 0-蓝; 1–黄; 2–黑; 3–白
} TPro0015Raw,*PPro0015Raw;


typedef struct
{
    char IssueOrg[8];					//	8	发卡方唯一标识
    quint8  bType ;							//	9	卡片类型，22-储值卡（鲁通B卡）；23-记账卡（鲁通A卡）
    quint8 bVersion ;							//	10	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    quint16 Network;					//	12	卡片网络编号
    quint8 Reserve[4];
    quint32 CardNo;						//	20	CPU卡片内部编号   老卡用了4位，整形存放
    quint32 StartTime;					//	24	启用时间，Unix时间
    quint32 ExpireTime;					//	28	到期时间，Unix时间
    char szVehPlate[12];				//	40	车牌号码
    quint8  bUserType ;						//	41	用户类型
    quint8 bDiscount;				//	42	折扣率
    quint8 bFCI;	        //	43	00
} TOldPro0015Raw ,*POldPro0015Raw;
// DF01联网收费信息文件
struct TPro0012Raw
{
    quint16 wNetworkId ;					//	2	入口路网号，路网号=5301
    quint8 baStaId[3];					//	5	入口收费站编码
    quint8  bLaneId ;						//	6	入口车道
    quint32 dwPassTime ;					//	10	入/出时间，采用UNIX时间，从1970年01月01日零时开始
    quint8  bVehClass ;						//	11	入口车型
    quint8  bPassStatus ;					//	12	通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    quint8  baPsamTermId[6];				//	18	入口PSAM卡号
    quint8  bRecIndex ;						//	19	入口流水记录号
    quint32 dwOpId ;						//	23	操作员号
    quint8 bShiftId ;						//	24	入口班次
    char szVehPlate[12];				//	36	车牌号码，无信息字节均为0x00
    quint8  bVehPlateColor ;				//	37	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色；0xFF-未输入颜色
    quint8  Reserve;					//	38	预留
    quint16  wCRC ;							//	40	CRC校验
};
typedef struct
{
    quint16 wNetworkId ;					//	2	入口路网号，路网号=5301
    quint16  wStaId;					//	5	入口收费站编码
    quint8 bLaneId ;						//	6	入口车道
    quint32 dwPassTime ;					//	10	入/出时间，采用UNIX时间，从1970年01月01日零时开始
    quint8  bVehClass ;						//	11	入口车型
    quint8  bPassStatus ;					//	12	通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    quint8 baFlagSta[9];   //标识站
    quint8  baOpId[3];    //	23	操作员号
    quint8  bShiftId ;						//	24	入口班次
    char  szVehPlate[12];				//	36	车牌号码，无信息字节均为0x00
    quint8 Reserve[4];   //	38	预留
} TOldPro0012Raw,*POldPro0012Raw ;

// DF01复合消费专用文件
struct  TPro0019Raw
{
    quint8 bCompoundAppType ;				//	1	复合应用类型标识符，按照标准，取值为0xAA
    quint8 bRecLen ;						//	2	记录长度，按照标准，取值为0x29
    quint8 bAppLockFlag ;					//	3	应用锁定标志，按照标准，取值为0x00
    TPro0012Raw  raw0012;				//	43
};

struct  TOldPro0019Raw
{
    quint8  bCompoundAppType ;				//	1	复合应用类型标识符，按照标准，取值为0xAA
    quint8 bRecLen;						//	2	记录长度，按照标准，取值为0x29
    quint8  bAppLockFlag ;					//	3	应用锁定标志，按照标准，取值为0x00
    TOldPro0012Raw  raw0012;				//	43
};

// DF01标识站信息文件
struct  TPro0009Raw
{
    quint8  baFlagStaInfo[9];	  //9 标识站信息，标识站采用7位编码，使用“0000001B”-“1111111B” 表示127个标识站，“0000000B”保留为无标识站。9个字节可顺序存储10个标识站。最后两位置零。
};

// DF01卡免费信息文件
struct TPro000ARaw
{
    quint8  bFreeType ;					    //	1	免费类型：1-管理处路段区间；2-行政区域区间；3-站点集合区间；4～256保留
    quint16  wFreeRegionNum; //array[0..1] of BYTE ;        //	3	区间数量
    quint8  baFreeRegion[80];                               //  83	免费区间
};

// M1卡通用基本信息
struct TM1BasicInfo
{
    quint32  dwCardId ;						//	卡片物理序号
    quint8  bVersion ;						//	卡片版本号=01
    quint16   wNetworkId ;					//	网络编号，发行该卡的收费网络编号=5301
    quint8    bType ;							//	卡片类型，0-通行卡；51-身份卡
    quint32  dwCardNum ;					//	卡号
    QDateTime StartTime ;				//	启用时间
    QDateTime ExpireTime ;				//	到期时间
    quint8  bIssueKeyVer;                  // 发行密钥版本号
    QString szIssueSAM;                   // 发行SAM卡编号
    quint32   dwIssueOrg ;					//	发行机构编码
    quint32  dwIssueOp ;					//	发行操作员编码
public:
    void clear()
    {
        dwCardId = 0;
        bVersion  = 0;
        wNetworkId = 0;
        bType = 0;
        dwCardNum = 0;
        bIssueKeyVer = 0;
        szIssueSAM = "";
        dwIssueOrg = 0;
        dwIssueOp = 0;
    }
};

// 身份卡基本信息
struct TOpCardBasicInfo
{
    TM1BasicInfo   M1BasicInfo ;			//	M1卡通用基本信息
    quint32   dwOpId ;						//	操作员工号
    QString  szOpName;					//	姓名
    QString  szID;                    //  身份证号
    bool  bCRC ;							//	CRC校验
public:
    void clear()
    {
        M1BasicInfo.clear();		//	M1卡通用基本信息
        dwOpId=0 ;						//	操作员工号
        szOpName="";					//	姓名
        szID="";                    //  身份证号
        bCRC=false;							//	CRC校验
    }
};

// 身份卡附属信息
struct TOpCardAppInfo
{
    quint8  bDataFileVersion ;             //  1   数据文件版本号
    quint32   dwOrg ;						//	所在机构，身份卡首次制作时写入0，下级可以更改
    quint8  bAuthorization ;				//	操作权限
    quint8  bWorkStatus ;					//	工作状态
    quint32   dwPersonizeOrg ;				//	个性化机构编码，保留
    QDateTime  PersonizeTime ;			//	个性化时间，保留
    QString szPassword;				//	操作员密码
    bool  bCRC ;							//	CRC校验
public:
    void clear()
    {
        bDataFileVersion = 0;
        dwOrg  = 0;
        bAuthorization = 0;
        bWorkStatus = 0;
        dwPersonizeOrg = 0;
        //PersonizeTime = QDateTime::currentDateTime();
        szPassword = "";
        bCRC = false;
    }
};

//身份卡发行系统使用的Dll用结构，不能使用string类型
// M1卡通用基本信息
struct TDLLM1BasicInfo
{
    quint32   dwCardId ;						//	卡片物理序号
    quint8 bVersion ;						//	卡片版本号=01
    quint16   wNetworkId ;					//	网络编号，发行该卡的收费网络编号=5301
    quint8  bType ;							//	卡片类型，0-通行卡；51-身份卡
    quint32   dwCardNum ;					//	卡号
    QTSYSTEMTIME  StartTime ;				//	启用时间
    QTSYSTEMTIME ExpireTime;				//	到期时间
    quint8 bIssueKeyVer;                  // 发行密钥版本号
    char szIssueSAM[14];                   // 发行SAM卡编号
    quint32   dwIssueOrg ;					//	发行机构编码
    quint32  dwIssueOp ;					//	发行操作员编码
public:
    void clear()
    {
        dwCardId=0 ;						//	卡片物理序号
        bVersion=0 ;						//	卡片版本号=01
        wNetworkId=0 ;					//	网络编号，发行该卡的收费网络编号=5301
        bType=0 ;							//	卡片类型，0-通行卡；51-身份卡
        dwCardNum=0 ;					//	卡号
        //QTSYSTEMTIME  StartTime ;				//	启用时间
        //QTSYSTEMTIME ExpireTime;				//	到期时间
        bIssueKeyVer=0;                  // 发行密钥版本号
        //char szIssueSAM[14];                   // 发行SAM卡编号
        memset(szIssueSAM,0,sizeof(szIssueSAM));
        dwIssueOrg=0 ;					//	发行机构编码
        dwIssueOp=0 ;					//	发行操作员编码
    }
};
// 身份卡基本信息
struct TDLLOpCardBasicInfo
{
    TDLLM1BasicInfo  M1BasicInfo  ;			//	M1卡通用基本信息
    quint32  dwOpId ;						//	操作员工号
    char szOpName[14];					//	姓名
    char  szID[19];             //  身份证号
    bool bCRC ;							//	CRC校验
public:
    void clear()
    {
        M1BasicInfo.clear();		//	M1卡通用基本信息
        dwOpId=0 ;						//	操作员工号
        //char szOpName[14];					//	姓名
        memset(szOpName,0,sizeof(szOpName));
        //char  szID[19];             //  身份证号
        memset(szID,0,sizeof(szID));
        bCRC=false ;							//	CRC校验
    }
};
// 身份卡附属信息
struct TDLLOpCardAppInfo
{
    quint8 bDataFileVersion ;             //  1   数据文件版本号
    quint32 dwOrg ;						//	所在机构，身份卡首次制作时写入0，下级可以更改
    quint8  bAuthorization ;				//	操作权限
    quint8 bWorkStatus ;					//	工作状态
    quint32 dwPersonizeOrg ;				//	个性化机构编码，保留
    QTSYSTEMTIME PersonizeTime ;			//	个性化时间，保留
    char szPassword[33];				//	操作员密码
    bool bCRC ;							//	CRC校验
public:
    void clear()
    {
        bDataFileVersion=0 ;             //  1   数据文件版本号
        dwOrg=0 ;						//	所在机构，身份卡首次制作时写入0，下级可以更改
        bAuthorization=0 ;				//	操作权限
        bWorkStatus=0 ;					//	工作状态
        dwPersonizeOrg=0 ;				//	个性化机构编码，保留
        //QTSYSTEMTIME PersonizeTime ;			//	个性化时间，保留
        //char szPassword[33];				//	操作员密码
        memset(szPassword,0,sizeof(szPassword));
        bCRC=false ;							//	CRC校验
    }
};
///////////////////////////////////////////////////

//标签卡附属信息文件定义2001
struct  TBoxCardAppInfo
{
    quint32 dwPersonizeOrg;			          //	1	个性化机构编码，保留
    QDateTime PersonizeTime ;			        //	2	个性化时间，保留
    quint32 dwBoxID ;                  //  3 卡盒编号
    quint32 dwCount;                       //4 卡盒卡数
    bool bCRC;							//	CRC校验
public:
    void clear()
    {
        dwPersonizeOrg=0;			          //	1	个性化机构编码，保留
        //QDateTime PersonizeTime ;			        //	2	个性化时间，保留
        dwBoxID=0 ;                  //  3 卡盒编号
        dwCount=0;                       //4 卡盒卡数
        bCRC=false;							//	CRC校验
    }
};

struct TBaoJiaoCardInfo
{
    QDateTime StartTime ;				//	启用时间
    QDateTime ExpireTime ;				//	到期时间
    char szVehPlate[16];				//	车牌号码
    quint8  bUserType ;						//	用户类型  5- 全免用户  7-区间免费
    quint8  bVehPlateColor ;				//	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色
    quint8   bVehClass ;						//	车型
    quint8  bVehTollType ;					//	免费车类，1-公务；2-联合收割；3-防汛；4-防火；5-救灾；6-临时免费；7-电煤
    quint8 bFreeRegionType ;				//	免费区间类型
    quint16 wFreeRegionNum ;				//	免费区间数量
    quint8 FreeRegion[80] ;				//	免费区间（按位存储）
public:
    void clear()
    {
        memset(szVehPlate,0,sizeof(szVehPlate));
        bUserType = 0;
        bVehPlateColor = 0;
        bVehClass = 0;
        bVehTollType = 0;
        bFreeRegionType = 0;
        wFreeRegionNum = 0;
        memset(FreeRegion,0,sizeof(FreeRegion));
    }
};

// 通行卡基本信息
typedef struct  //TTollCardBasicInfo
{
    bool  bOldCard;           //是否是老通行卡;
    TM1BasicInfo M1BasicInfo ;			//	M1卡通用基本信息
    bool bCRC;							//	CRC校验
public:
    void clear() //wumax ==? M1BasicInfo.clear(); 不能链接到M1BasicInfo why
    {
        bOldCard=false;           //是否是老通行卡;
        M1BasicInfo.clear();                   //	M1卡通用基本信息
        bCRC=false;							//	CRC校验
    }
}TTollCardBasicInfo,TBoxCardBasicInfo ;

// 包交卡基本信息
struct TMonthCardBasicInfo
{
    bool  bValidCard;             //有效卡
    TM1BasicInfo  M1BasicInfo;			//	M1卡通用基本信息
    //  {**以下为包交卡信息**};
    TBaoJiaoCardInfo  BaoJiaoInfo;
    bool bCRC;							//	CRC校验
public:
    void clear()
    {
        bValidCard = false;
        M1BasicInfo.clear();
        BaoJiaoInfo.clear();
        bCRC = false;
    }
};

//标签卡基本信息文件

// 联网收费信息文件定义
struct  TCardTollInfo
{
    quint16  wNetworkId ;					//	入口路网号，路网号=5301
    quint32   dwStaId ;						//	入口收费站编码
    quint8   bLaneId ;						//	入口车道
    quint8   bPassStatus ;					//	通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    quint8 bMaintenance ;                                       //      维护发卡标识（0-正常发卡、1-维护发卡）
    QDateTime  PassTime ;                                       //	入口时间
    quint8 bRecIndex ;						//	入口流水记录号
    quint32   dwOpId ;						//	入口操作员号
    quint8  bShiftId ;						//	入口班次
    quint8 bVehClass ;						//	入口车型
    quint8 bVehTollType ;					//	入口车类
    quint8 baPsamTermId[6];                                     //	入口PSAM卡号
    quint32  dwWriteCount ;					//	卡片使用次数，写信息时累加
    quint32 dwCardBoxId ;					//	卡箱逻辑编号
    char szVehPlate[16];                                        //	车牌号码，无信息字节均为0x00
    quint8  bVehPlateColor ;                                    //	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色；0xFF-未输入颜色
    quint8   bFlagStaCount ;					//	标识站个数，0-10
    quint8  baFlagStaInfo[10];                                  //	标识站信息
    bool   bCRC ;                                               //	CRC校验
public:
    void clear()
    {
        wNetworkId = 0;
        dwStaId = 0;
        bLaneId = 0;
        bPassStatus = 0;
        bMaintenance = 0;
        bRecIndex = 0;
        dwOpId = 0;
        bShiftId = 0;
        bVehClass = 0;
        bVehTollType = 0;
        memset(baPsamTermId,0,sizeof(baPsamTermId));
        dwWriteCount = 0;
        memset(szVehPlate,0,sizeof(szVehPlate));
        bVehPlateColor = 0;
        bFlagStaCount = 0;
        memset(baFlagStaInfo,0,sizeof(baFlagStaInfo));
        bCRC = false;
    }
    void Copy( const TCardTollInfo& others)
    {
        wNetworkId=others.wNetworkId;
        dwStaId=others.dwStaId;
        bLaneId=others.bLaneId;
        bPassStatus=others.bPassStatus;
        bMaintenance=others.bMaintenance;
        PassTime=others.PassTime;
        bRecIndex=others.bRecIndex;
        dwOpId=others.dwOpId;
        bShiftId=others.bShiftId;
        bVehClass=others.bVehClass;
        bVehTollType=others.bVehTollType;
        memcpy(baPsamTermId,others.baPsamTermId,6);
        dwWriteCount=others.dwWriteCount;
        dwCardBoxId=others.dwCardBoxId;
        memcpy(szVehPlate,others.szVehPlate,16);
        bVehPlateColor=others.bVehPlateColor;
        bFlagStaCount=others.bFlagStaCount;
        memcpy(baFlagStaInfo,others.baFlagStaInfo,10);
        bCRC=others.bCRC;
    }
};
#pragma pack(pop)

// Pro卡基本信息
struct TProCardBasicInfo
{
    bool  bOldCard;                                     //      是否是老ProCard;
    quint32 dwCardId ;                                  //	卡片物理序号
    char szIssueOrgId[20];				//	发卡方唯一标识
    quint8 bType ;                                      //	卡片类型，22-储值卡（鲁通B卡）；23-记账卡（鲁通A卡）
    quint8 bVersion ;                                   //	卡片版本号，记录发行版本号，0x10代表支持复合消费，其它保留
    quint16 wNetworkId;                                 //	卡片网络编号
    char szCardNo[17];					//	CPU卡片内部编号
    QDateTime  StartTime ;				//	启用时间
    QDateTime ExpireTime ;				//	到期时间
    char szVehPlate[16];				//	车牌号码
    quint8  bUserType ;                                 //	用户类型 0-普通用户; 6-全免鲁通C卡（国标为公务车用户）; 7-区间免费鲁通C卡
    quint8  bVehPlateColor ;				//	车牌颜色，0-蓝色；1-黄色；2-黑色；3-白色
    quint8  bVehClass ;                                 //	车型
    quint8  bVehTollType ;                              //      免费车类，1-公务；2-联合收割；3-防汛；4-防火；5-救灾；6-临时免费；7-电煤
    quint8  bDiscount;                                  //      折扣率0-不打折, 5表示九五折，90 表示一折, 100表示全免
    quint8  bFreeRegionType ;				//	免费区间类型
    quint16 wFreeRegionNum ;				//	免费区间数量
    quint8  FreeRegion[80] ;				//	免费区间（按位存储）
public:
    void clear()
    {
        bOldCard = false;
        dwCardId = 0;
        memset(szIssueOrgId,0,sizeof(szIssueOrgId));
        bType = 0;
        bVersion = 0;
        wNetworkId = 0;
        memset(szCardNo,0,sizeof(szCardNo));
        memset(szVehPlate,0,sizeof(szVehPlate));
        bUserType = 0;
        bVehPlateColor = 0;
        bVehClass = 0;
        bVehTollType = 1;
        bDiscount = 0;
        bFreeRegionType = 0;
        wFreeRegionNum = 0;
        memset(FreeRegion,0,sizeof(FreeRegion));
    }
    void Copy(const TProCardBasicInfo& othes)
    {
        bOldCard=othes.bOldCard;
        dwCardId=othes.dwCardId;
        memcpy(szIssueOrgId,othes.szIssueOrgId,20);
        bType=othes.bType;
        bVersion=othes.bVersion;
        wNetworkId=othes.wNetworkId;
        memcpy(szCardNo,othes.szCardNo,17);
        StartTime=othes.StartTime;
        ExpireTime=othes.ExpireTime;
        memcpy(szVehPlate,othes.szVehPlate,16);
        bUserType=othes.bUserType;
        bVehPlateColor=othes.bVehPlateColor;
        bVehClass=othes.bVehClass;
        bVehTollType=othes.bVehTollType;
        bDiscount=othes.bDiscount;
        bFreeRegionType=othes.bFreeRegionType;
        wFreeRegionNum=othes.wFreeRegionNum;
        memcpy(FreeRegion,othes.FreeRegion,80);
    }
};
// Pro卡消费信息
struct TProCardConsumeInfo
{
    QDateTime ConsumeTime ;	// 扣款时间(IN)
    quint32 dwMoney ;			// 扣款金额(IN)
    quint8 psamTermNo[6];     // PSAM卡终端机编号(OUT)
    quint32 dwTermSeq ;		// 终端机交易序号(OUT)
    quint8  bTransType ;		// 交易类型(OUT)
    quint8 Spare1[3];			// 保留(OUT)
    quint32  dwTac ;			// TAC(OUT)
    quint16 wCardSeq ;			// 用户卡交易序号(OUT)
    quint32 dwBalanceBefore ;	// 扣款前余额(OUT)
    quint32   dwBalanceAfter ;	// 扣款后余额(OUT)
    quint8  bKeyVer ;			// 密钥版本(OUT)
    quint8  Spare2[7];			// 保留(OUT)
public:
    void clear()
    {
        dwMoney =0;
        memset(psamTermNo,0,sizeof(psamTermNo));
        dwTermSeq = 0;
        bTransType = 0;
        memset(Spare1,0,sizeof(Spare1));
        dwTac = 0;
        wCardSeq = 0;
        dwBalanceBefore = 0;
        dwBalanceAfter = 0;
        bKeyVer = 0;
        memset(Spare2,0,sizeof(Spare2));
    }
};
//转换卡类型，返回储值卡、记账卡
inline quint8 CardINToYTType(quint8 ACardType)
{
    quint8 temp;
    switch(ACardType)
    {
    //普通储值卡
    case CARD_TYPE_STORE_CARD:
        //路段储值包交卡
    case CARD_TYPE_ROAD_BAOJIAO_STORE:
        //路段储值免交卡
    case CARD_TYPE_ROAD_FREE_STORE:
        temp=CARD_TYPE_STORE_CARD;
        break;
        //普通记账卡
    case CARD_TYPE_TALLY_CARD:
        //全省包交卡
    case CARD_TYPE_ALLFREE_CARD:
        //全省免交卡
    case CARD_TYPE_ALL_BAOJIAO_CARD:
        //路段记账包交卡
    case CARD_TYPE_ROAD_BAOJIAO_TALLY:
        //路段记账免交卡
    case CARD_TYPE_ROAD_FREE_TALLY:
        temp=CARD_TYPE_TALLY_CARD;
        break;
    }
    return temp;
}
//用户类型
inline quint8 CardINToYTUseType(quint8 ACardType,quint8 AUserType)
{
    quint8 temp;
    switch(ACardType)
    {
    //**********普通记账卡、储值卡返回卡内用户类型**************//
    //普通记账卡
    case CARD_TYPE_TALLY_CARD:
        //普通储值卡
    case CARD_TYPE_STORE_CARD:
        temp=AUserType;
        break;
        //************全省包交、免交均属于全免用户******************//
        //全省包交卡
    case CARD_TYPE_ALLFREE_CARD:
    case CARD_TYPE_ALL_BAOJIAO_CARD:
        temp=CARD_UserType_FullFree;
        break;
        //************路段包交、免交均属于区间用户***************//
        //路段储值包交卡
    case CARD_TYPE_ROAD_BAOJIAO_STORE:
        //路段储值免交卡
    case CARD_TYPE_ROAD_FREE_STORE:
        //路段记账包交卡
    case CARD_TYPE_ROAD_BAOJIAO_TALLY:
        //路段记账免交卡
    case CARD_TYPE_ROAD_FREE_TALLY:
        temp=CARD_UserType_RegionFree;
        break;
    default:
        temp=AUserType;
        break;
    }
    return temp;
}
//判断是否为免费卡
inline bool IsFreeCard(const TProCardBasicInfo& proCardBasicInfo)
{
    return (proCardBasicInfo.bType >= CARD_TYPE_ALLFREE_CARD &&
            proCardBasicInfo.bType <= CARD_TYPE_ROAD_BAOJIAO_TALLY);
}

// {-------------------------------------------------------------------------------
//   参  数: const proCardBasicInfo: TProCardBasicInfo
//   返回值: boolean
//   说  明: 判断是否是云通卡
// -------------------------------------------------------------------------------}
inline bool IsYTCard(const  TProCardBasicInfo& proCardBasicInfo)
{
    return  ((proCardBasicInfo.bType ==CARD_TYPE_STORE_CARD) ||
             (proCardBasicInfo.bType == CARD_TYPE_TALLY_CARD)||
             (proCardBasicInfo.bType >= CARD_TYPE_ALLFREE_CARD &&
              proCardBasicInfo.bType <= CARD_TYPE_ROAD_BAOJIAO_TALLY));

}

// {-------------------------------------------------------------------------------
//   参  数: ACardType:Integer
//   返回值: String
//   说  明: 获取卡名称
// -------------------------------------------------------------------------------}
inline QString GetCardTypeName(int ACardType)
{
    switch( ACardType )
    {
    //通行卡
    case CARD_TYPE_PASS_CARD:
        return "通行卡";
        //储值卡
    case  CARD_TYPE_STORE_CARD :
        return "储值卡";
        break;
        //记账卡
    case    CARD_TYPE_TALLY_CARD :
        return "记账卡";
        break;
        //身份卡
    case     CARD_TYPE_OP_CARD  :
        return "身份卡";
        break;
        //纸卡
    case  CARD_TYPE_PAPER  :
        return "纸卡";
        break;
        //包交卡
    case  CARD_TYPE_MONTH_CARD:
        return "包交卡";
        break;
        //全省免交
    case CARD_TYPE_ALLFREE_CARD:
        return "全省免交卡";
        //全省包交
    case CARD_TYPE_ALL_BAOJIAO_CARD:
        return "全省包交卡";
        //路段免交储值卡
    case CARD_TYPE_ROAD_FREE_STORE:
        return "路段免交储值卡";
        //路段包交储值卡
    case CARD_TYPE_ROAD_BAOJIAO_STORE:
        return "路段包交储值卡";
        //路段免交记帐卡
    case CARD_TYPE_ROAD_FREE_TALLY:
        return "路段免交记帐卡";
        //路段包交记帐卡
    case CARD_TYPE_ROAD_BAOJIAO_TALLY:
        return "路段包交记帐卡";
        //卡盒卡
    case CARD_TYPE_BOX_CARD:
        return "卡盒卡";
        break;
    default:
        return  "其它卡";
        break;
    }
}
#endif // CARDFILEDEF_H
