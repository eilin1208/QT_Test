#ifndef CARDFILEMGR_H
#define CARDFILEMGR_H
#include <QTextCodec>
#include "CardFileMgr_global.h"

//class CARDFILEMGRSHARED_EXPORT CardFileMgr {
//public:
//    CardFileMgr();
//};
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
// DF01复合消费专用文件
struct  TPro0019Raw
{
    quint8 bCompoundAppType ;				//	1	复合应用类型标识符，按照标准，取值为0xAA
    quint8 bRecLen ;						//	2	记录长度，按照标准，取值为0x29
    quint8 bAppLockFlag ;					//	3	应用锁定标志，按照标准，取值为0x00
    TPro0012Raw  raw0012;				//	43
};
// DF01卡免费信息文件
struct TPro000ARaw
{
    quint8  bFreeType ;					    //	1	免费类型：1-管理处路段区间；2-行政区域区间；3-站点集合区间；4～256保留
    quint16  wFreeRegionNum; //array[0..1] of BYTE ;        //	3	区间数量
    quint8  baFreeRegion[80];                               //  83	免费区间
};
// DF01标识站信息文件
struct  TPro0009Raw
{
    quint8  baFlagStaInfo[9];	  //9 标识站信息，标识站采用7位编码，使用“0000001B”-“1111111B” 表示127个标识站，“0000000B”保留为无标识站。9个字节可顺序存储10个标识站。最后两位置零。
};
#pragma pack(pop)
static TTollCardBasicInfoRaw TollcardBasicInfo;
static TPro0015Raw Pro15Raw;
static TPro0012Raw Pro12Raw;
static TPro0019Raw Pro19Raw;
static TPro000ARaw ProARaw;
static TPro0009Raw Pro09Raw;
inline int Char2Hex(char c)
{
    if ('0'<=c && c<='9') return c-'0';
    if ('a'<=c && c<='f') return c-'a'+10;
    if ('A'<=c && c<='F') return c-'A'+10;
    return -1;
};
bool Hex2Raw(quint8 *pData, const char *szMsg, int nMsgLen)
{
    int hi, lo;
    for (int i=0;i<nMsgLen/2;i++)
    {
        hi=Char2Hex(szMsg[i*2]);
        lo=Char2Hex(szMsg[i*2+1]);
        if (hi<0 || lo<0) return false;
        pData[i]=(hi<<4)+lo;
    }
    return true;
};
quint8 Byte2Bcd(quint8 b)
{
    return  ((b /10) << 4) |
            (b % 10);
};
quint16 Word2Bcd (quint16 w)
{
    return (Byte2Bcd(w % 100) << 8) || Byte2Bcd(w / 100);
};

quint32  Dword2Bcd(quint32 dw)
{
    return (Word2Bcd(dw %10000) << 16) ||
            Word2Bcd(dw /10000);
};
bool ll2bcd(char *bcd, int bcd_len, quint64 d64)
{
    int tmp = 0;
    for(int i = bcd_len - 1; i >= 0; i--)
    {
        tmp = d64 % 100;
        bcd[i] = ((tmp / 10) << 4) + ((tmp % 10) & 0x0f);
        d64 /= 100;
    }
    return true;
};
QString GB2312toUtf8(const char * str, int size)
{
    if(size == -1)
    {
        return QTextCodec::codecForName("GB18030")->toUnicode(str);
    }
    else
    {
        return QTextCodec::codecForName("GB18030")->toUnicode(str, size);
    }
};
const char* Utf8toGB2312(const char* str, int size)
{
    //UTF-8 Char* to GB2312 QByteArray
    return QTextCodec::codecForName("GB18030")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str, size));
};
API int JT_OpenReader(int iComID,const char* sParas);
API int JT_CloseReader(int iComID);
API int JT_OpenCard(int iComID,quint8* sCardSer,int &iMaxLength);
API int JT_CloseCard(int iComID);
API int JT_LEDDisplay(int iComID,quint8 cRed,quint8 cGreen,quint8 cBlue);
API int JT_AudioControl(int iComID,quint8 cTimes,quint8 cVoice);
API int JT_ReadBlock(int iComID,int iKeyType,int iBlockn,const quint8* sKey,quint8 *sReply);
API int JT_WriteBlock(int iComID,int ikeyType,int iBlockn,const quint8* sKey,quint8 *sData);
API int JT_WriteKey(int iComID,int iKeyType,int iSec,const quint8* sKey,const quint8* sKeyData);
API int JT_CPUCCommand(int iComID,const quint8* sCommand,int iLenCmd,quint8* sReply,int &iLenRep);
API int JT_SamReset(int iComID,int iSockID,int iBaudrate,int iProtocolType,quint8* sReply,int &iLenRep);
API int JT_ChangeSamBaudrate(int iComID,int iSockID,int iBaudrate,int iProtocolType);
API int JT_SamCommand(int iComID,int iSockID,const quint8* sCommand,int iLenCmd,quint8*sReply,int &iLenRep);
API int JT_ReadFile(int iComID,int sFileId,quint8 sKeyType,int iAddr,int iLength,quint8*sReply);
API int JT_WriteFile(int iComID,int sFileID,quint8 sKeyType,int iAddr,int iLength,const quint8* sData);
API int JT_GetLastError(int iComID,char* sLastError,int iMaxLength);
API int JT_ReaderVersion(int iComID,char* sReaderVersion,int iRVerMaxLength,char *sAPIVersion,int iAPIVerMaxLength);
#endif // CARDFILEMGR_H
