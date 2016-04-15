#ifndef MSGDEFINE_H
#define MSGDEFINE_H


#include <QObject>
//获取参数
const int MSG_TYPE_REQPARAM = 55;
//获取新参数
const int MSG_TYPE_REQNEWPARAM = 56;
//查询
const int MSG_TYPE_QRY = 2;
//业务
const int MSG_TYPE_YW = 1;
//监控下发的公告信息最大长度
const int MAX_JK_MSG_LEN = 512;
//压缩方式
const char ZIP_TYPE_NONE='0';
const char ZIP_TYPE_ZIP='1';
const char ZIP_TYPE_RAR='2';
//发送数据
const int SEND_SUPERVISOR_ONLY  =1 ;	// 仅发送监控前台
const int SEND_DATABASE_ONLY    =2 ;	// 仅发送收费站后台
const int SEND_BOTH             =3 ;	// 发送监控前台和收费站后台

//*****64位特情定义*******
const int DS_TOLLBYVC     = 1;          //货车按车型收费
const int DS_TSBDB2S      = 2;         //车型不符入大出小 * 货车大改小 时，要同时填1、2
const int DS_TSBDS2B      = 3;         //车型不符入小出大 * 货车大改小 时，要同时填1、3
const int DS_RESERVE1     = 4;         //预留
const int DS_TSVMP2F      = 5;         //车类不符收改免
const int DS_TSVMF2P      = 6;         //车类不符免该收
const int DS_TSVTH2K      = 7;         //客货不符货改客
const int DS_TSVTK2H      = 8;         //客货不符客改货
const int DS_UXPAY        = 9;         //U型收费
const int DS_UXFREE       = 10;         //U型免费
const int DS_RESERVE2     = 11;         //预留
const int DS_SEOVTIME     = 12;        //非正常超时
const int DS_SECG1        = 13;        //闯关（收费员上班）
const int DS_SECG2        = 14;        //闯关（收费员未上班）
const int DS_SECG3        = 15;        //闯关（设备误报警）
const int DS_SECG4        = 16;        //闯关（车辆误入）
const int DS_SECG5        = 17;        //闯关（车辆退回）
const int DS_SECG6        = 18;        //闯关（闯关被拦截）
const int DS_WXTXJ        = 19;        //无效通行劵（不可读）
const int DS_SEJT         = 20;        //J型车
const int DS_TSIVERR      = 21;        //废票
const int DS_PYHC         = 22;        //优惠
const int DS_SEBADCARD    = 23;        //坏卡
const int DS_SECCARD      = 24;        //换卡
const int DS_CARDGQ       = 25;        //卡过期（通行介质）
const int DS_CARDG        = 26;        //卡灰名单
const int DS_TSPLATEERR   = 27;        //车牌不符
const int DS_VEHB         = 28;        //车辆黑名单
const int DS_VEHG         = 29;        //车辆灰名单
const int DS_VEHW         = 30;        //车辆白名单
const int DS_PSAMB        = 31;        //PSAM卡黑名单
const int DS_PSAMNW       = 32;        //PSAM卡不住白名单
const int DS_WEIGHTC      = 33;        //计重更改
const int DS_TSIVRPT      = 34;        //重打票
const int DS_INVOCIEC     = 35;        //修改票号
const int DS_HDOWNBAR     = 36;        //手动落杆
const int DS_LOCK         = 37;        //锁杆
const int DS_ALOCKCARD    = 38;        //自动锁卡
const int DS_HLOCKCARD    = 39;        //手动锁卡
const int DS_RESERVE3     = 40;        //预留
const int DS_RepairInv    = 41;        //补票
const int DS_Tract        = 42;        //拖车
const int DS_NoBoxCard    = 43;        //非本卡盒卡
const int DS_NoCard       = 50;        //无卡

//卡盒操作类型
const int CARDBOXOP_UP        = 1;      //换上卡盒
const int CARDBOXOP_DOWN      = 2;      //换下卡盒
const int CARDBOXOP_FORCEDOWN = 3;      //强制换下卡盒
#pragma pack(push,1)
//报文头
struct TMsgHeadInfo
{
    //报文长度，含报文头
    quint32 TotalLength;
    //报文编码
    char cMsgID[2];
    //报文类型
    char cMsgType[2];
    //压缩方式 '0'-不压缩 '1'-Zip压缩 '2'-rar压缩
    char ZipType;
    //发送方机构编码(6位站编码+3位车道编码)
    char Sender[9];
    //接收方机构编码
    char Receiver[9];
    //发送时间
    char SendTime[14];
    //MD5校验码
    char VerifyCode[32];
};
//报文基本信息
struct TMsgBasicInfo
{
    //报文头
    TMsgHeadInfo msgHead;
    //发送ID
    quint64 WasteID;
    //位元表
    char wy[4];
public:
    void clear()
    {
        memset(&msgHead,0,sizeof(msgHead));
        WasteID = 0;
        memset(wy,0,4);
    }
};
//车道基本信息
struct  TMsgLaneBasicInfo
{
    //业务时间
    char OccurTime[14];
    //压线圈时间
    char TriggerTime[14];
    //第一次车型输入时间
    char VTInputTime[14];
    //业务结束时间
    char EndWasteTime[14];
    //收费站编码
    char StationID[6];
    //车道编码
    char LaneID[3];
    //车道业务序列号
    quint8 WasteSN;
    //所属省级中心
    char BL_ProvID[6];
    //所属网络编号
    char BL_NetWorkID[6];
    //所属路段中心
    char BL_RoadID[6];
    //所属分中心
    char BL_SubCenter[6];
    //车道类型
    quint8 OrgType;
    //车道程序版本号
    char LaneAppVer[8];
    //操作员工号
    char OperNo[9];
    //操作员姓名
    char OperName[10];
    //上班时间YYYYMMDDhhmmss
    char LoginTime[14];
    //上班流水
    quint8 LoginWasteSN;
    //逻辑日期YYYYMMDD
    char Lchar[8];
    //统计班次
    quint8 ShiftNo;
    //操作员班组
    quint16 GroupID;
    //车道状态
    quint8 LaneState;
    //PSAM终端机编号
    char PSamTerm[12];
    //参数版本号
    char ParaVer[100];
    //车道权限
    char LaneRole[8];
};
//车辆信息
struct TMsgVehInfo
{
    //除判车种
    quint8 PVT;
    //最终车种
    quint8 VT;
    //初判车种
    quint8 PVC;
    //自动识别车型
    quint8 AutoVC;
    //最终车型
    quint8 VC;
    //自动识别车牌颜色shortint
    char AutoVLPC;
    //自动识别车牌
    char AutoVLP[12];
    //最终使用车牌颜色shortint
    char VLPC;
    //最终使用车牌
    char VLP[12];
    //过车数量
    int VCnt;
    //感应车流
    int InductCnt;
    //车速
    quint16 VSpeed;
};
//卡信息结构
struct TMsgICCInfo
{
    //卡片类型
    quint8 CardType;
    //发卡方网络号
    char CardNet[4];
    //卡号
    char CardID[16];
    //卡物理序列号
    quint32 CardSN;
    //通行卡卡数
    quint16 CardCnt;
    //卡内余额 longint
    quint32 CardBal;
};
//支付卡信息
struct TMsgPayCardInfo
{
    //支付卡类型
    quint8 PayCardType;
    //支付卡网络号
    char PayCardNet[4];
    //支付卡卡号
    char PayCardID[20];
    //支付卡物理序列号
    quint32 PayCardSN;
    //支付卡用户类型
    quint8 PayCardUserType;
    //支付卡交易序列号
    quint16 PayCardTranSN;
    //卡内车牌
    char PayCardVLP[12];
    //卡内车牌颜色
    char PayCardVLPC;
    //卡内车型
    quint8 PayCardVehClass;
    //psam交易序列号
    int PSAMTradeSN;
    //tac码
    int TAC;
    //pasm终端机编号
    char PSamTerm[12];
    //支付卡上次余额
    quint32 PayCardBalBefore;
    //支付卡余额
    quint32 PayCardBalAfter;
    //交易类型
    quint8 TransType;
    //密钥版本
    quint8 KeyVer;
    //银联支付信息
    char UnionPayInfo[64];
};
//)OBU信息
struct TMsgOBUInfo
{
    char OBUProvider[8];
    char OBUID[16];
};
//卡箱信息
struct TMsgCardBoxInfo
{
    //卡盒顺序号
    quint8 CardBoxIndex;
    //卡盒编号
    char CardBoxID[9];
    //盒内卡数
    quint16 CardCount;
};
//按键信息
struct TMsgKeyInfo
{
    //按键次数
    quint16 KeyNum;
    //按键信息 KKHHMMSS，kk十六进制+时分秒;
    // 本字段长度为按键次数乘8
    char KeyValue[1024];
};
//交易状态信息
struct TMsgDealInfo
{
    //特情状态
    char SpInfo[64];
    //设备状态
    char DevInfo[32];
};
//授权信息
struct  TMsgAuthInfo
{
    //授权类型
    quint8 AuthType;
    //授权工号
    char AuthID[9];
    //授权姓名
    char AuthName[10];
    //授权时间
    char AuthTime[14];
};
//车辆入口信息
struct TMsgVehEnInfo
{
    //查询入口站
    char QueryEnStation[6];
    //入口站号
    char EnStation[6];
    //车道编号
    char EnLane[3];
    //时间
    char EnTime[14];
    //序列号
    quint8 EnWasteSN;
    //所属省中心
    char EnProvID[6];
    //所属网络编号
    char EnNetWorkID[6];
    //所属路段中心
    char EnRoadID[6];
    //所属分中心
    char EnSubCenter[6];
    //车道机构类型
    quint8 EnLaneType;
    //收费员工号
    char EnOper[9];
    //车型
    quint8 EnVC;
    //车种
    quint8 EnVT;
    //车牌颜色
    char EnVLPC;
    //车牌
    char EnVLP[12];
    //psam卡终端机号
    char EnPSAMTerm[12];
};
//记重信息
struct TMsgWeightInfo
{
    //自动识别轴组信息
    char AutoAxisInfo[84];
    //最终收费轴组信息
    char AxisInfo[84];
    //最终车型总轴数
    quint8 AxleNum;
    //总重
    int TotalWeight;
    //限载总重
    int LimitWeight;
    //超限率
    quint16 OverWerghtRate;
    //修改前轴组信息
    char PreAxisInfo[84];
    //修改前总轴数
    quint8 PreAxleNum;
    //修改前总重
    int PreTotalWetght;
    //修改前限载总重
    int PreLimitWeight;
    //修改前超限率
    quint16 PreOverWeightRate;
};
//记重基础信息
struct TMsgTollBaseInfo
{
    //超时时间
    int OverTime;
    //计费里程
    quint32 TollDistance;
    //实际里程
    quint32 RealDistance;
    //经过标识站数
    quint8 FlagStationCnt;
    //经过标识站信息
    char FlagStationInfo[60];
    //计费方式
    quint8 TollFlag;
    //折扣率
    quint16 Rebate;
    //费率版本号
    char FalcVer[14];
    //免费区间类型
    quint8 FreeType;
    //免费区间数量
    quint8 FreeAreaCnt;
    //免费区域
    char FreeArea[301];
};
//通行费信息
struct TMsgTollInfo
{
    //支付方式
    quint8 PayType;
    //货车基本费额
    quint32 TruckBasicMoney;
    //应收通行费金额
    quint32 TollMoney;
    //应收卡成本费金额
    quint32 CardCost;
    //超限加收通行费金额
    quint32 OverWeightMoney;
    //通行费免收金额
    quint32 FreeMoney;
    //折扣优惠金额
    quint32 RebateMoney;
    //应实收通行费金额
    quint32 CashMoney;
    //应实收卡成本金额
    quint32 CashCardMoney;
    //支付序列号
    quint8 PaySN;
    //本次最终支付通行费金额
    quint32 LastMoney;
    //本次最终支付卡成本金额
    quint32 LastCardMoney;
};
//拆分信息
struct TMsgSplitInfo
{
    //拆分次数
    quint16 SpliteNum;
    //拆分信息
    char SplitInfo[2701];
    //收费单元数
    quint16 CellNum;
    //经过收费单元信息
    char CellInfo[1201];
    //收费站数量
    quint16 StationNum;
    //经过收费站信息
    char PathInfo[901];
};
//票据修改信息
struct TMsgInvModefyInfo
{
    //修改前起号
    char PreInvNo[12];
    //修改前数量
    quint16 PreInvCnt;
    //修改后起号
    char AftInvNo[12];
    //修改后数量
    quint16 AftInvCnt;
};
//票据打印信息
struct TMsgInvPrintInfo
{
    //打印发票编号
    char Inv[12];
    //打印发票张数
    quint8 InvCnt;
};
//班次汇总信息
struct TMsgShiftSum
{
    //班次
    char ShiftID[2];
    //逻辑日
    char Lchar[8];
    //班次起始时间
    char StartTime[14];
    //班次开始流水号
    quint8 StartWasteSN;
    //班次截止时间
    char EndTime[14];
    //记录条数
    int DealCnt;
    //前线圈计数
    int FrontLoopCnt;
    //后线圈计数
    int BackLoopCnt;
    //栏杆降杆次数
    int BarDownCnt;
    //栏杆抬杆次数
    int BarUpCnt;
    //票据起号
    char InvBeginID[12];
    //票据止号
    char InvEndID[12];
    //总车辆计数
    int VCnt;
    //通行卡计数
    int CardCnt;
    //纸卡计数
    int PapCnt;
    //坏卡计数
    int DamCardCnt;
    //运通卡计数
    int YTCardCnt;
    //公务卡计数
    int OffCardCnt;
    //包交卡计数
    int MonthCardCnt;
    //应收通行费金额
    quint32 TollMoney;
    //应收卡成本金额
    quint32 CardCost;
    //超限加收通行费金额
    quint32 OverWeightMoney;
    //通行费免收金额
    quint32 FreeMoney;
    //折扣通行费金额
    quint32 RebateMoney;
    //应实收通行费金额
    quint32 CashMoney;
    //应实收卡成本金额
    quint32 CashCardMoney;
    //储值卡金额
    quint32 StoreMoney;
    //记账卡金额
    quint32 TallyMoney;
    //公务车金额
    quint32 OfficeMoney;
    //包交车金额
    quint32 DiscountCarMoney;
    //银联卡支付
    quint32 UinonPayMoney;
    //免费金额
    quint32 TollFreeMoney;
    //收费车现金金额
    quint32 TollCashMoney;
};
//车道参数启用信息
struct TMsgparamUseInfo
{
    //参数代码
    quint8 ParaID;
    //当前版本号
    char Ver[14];
    //当前版本启用时间
    char UseTime[14];
    //当前版本记录数
    int RecordCnt;
    //新版本号
    char NewVer[14];
    //新版本启用时间
    char NewUseTime[14];
    //新本版记录数
    int NewRecordCnt;
};
//请求监控回控信息
struct TMsgRequestMonInfo
{
    //事件代码
    int EventCode;
    //回应类型 0,不需回应 1，需回应
    quint8 ResponseType;
    //回应结果0,中止 1，继续处理
    quint8 ResponseResult;
    char SpEventSet[32];
};
//图像基本结构
struct TMsgImageInfo
{
    //抓拍时间
    char PhotoTime[14];
    //图像文件名
    char ImageFileName[128];
    //图像大小
    int ImageSize;
    //图像数据PChar
    char* ImageData;
};
//发送报文

struct TMsgToSend
{
    TMsgBasicInfo BasicInfo;
    //车道基本信息
    TMsgLaneBasicInfo LaneInfo;
    //车辆信息
    TMsgVehInfo VehInfo;
    //ic卡信息结构
    TMsgICCInfo ICCInfo;
    //支付卡信息
    TMsgPayCardInfo PayCardInfo;
    //obu信息
    TMsgOBUInfo OBUInfo;
    //卡箱信息
    TMsgCardBoxInfo CardBoxInfo;
    //按键信息
    TMsgKeyInfo KeyInfo;
    //交易状态信息
    TMsgDealInfo DealInfo;
    //授权信息
    TMsgAuthInfo AuthInfo;
    //车辆入口信息
    TMsgVehEnInfo VehEnInfo;
    //记重信息
    TMsgWeightInfo WeightInfo;
    //计费基础信息
    TMsgTollBaseInfo TollBaseInfo;
    //通行费信息
    TMsgTollInfo TollInfo;
    //拆分信息
    TMsgSplitInfo SplitInfo;
    //票据修改信息
    TMsgInvModefyInfo InvModefyInfo;
    //票据打印信息
    TMsgInvPrintInfo InvPrintInfo;
    //工班汇总信息
    TMsgShiftSum ShiftSum;
    //参数启用信息
    TMsgparamUseInfo ParamUseInfo;
    //请求监控回控信息
    TMsgRequestMonInfo RequestMonInfo;
    //抓拍图像信息
    TMsgImageInfo CapImage;
    //车牌图像信息
    TMsgImageInfo VLPImage;
};
// 基本回应报文
struct TMsgResponseInfo
{
    // 报文基本信息
    TMsgHeadInfo MsgHead;
    // 2.发送ID
    quint64 WasteID;
    // 处理结果(1:处理成功,2:处理失败,3:处理失败，需要重发)
    quint8 RecResult;
    //描述
    char Descirption[128];
    //授权信息
    TMsgAuthInfo AuthInfo;
};
//监控发送公告用的结构内容 wqs添加20081002
struct  TJKMsgInfo
{
    TMsgHeadInfo Header;
    //公告信息长度
    int MsgLength;
    //生效时间
    char UseTime[14];
    //失效时间
    char ExpireTime[14];
    //发送操作员
    char SenderOper[10];
    //优先级从1-9依次增高
    quint8 Priority;
    //字体
    char DisFont[20];
    //字符集 0西方 134汉字
    int CharSet;
    //字形 0 常规 1 粗体
    quint8 Style;
    //字体大小
    quint16 DisSize;
    //字体颜色
    int Color;
    //背景颜色
    int bkColor;
    //是否显示下划线 0常规 1下划线
    quint8 Underline;
    //显示方式0滚动 1 固定
    quint8 ShowWay;
    char MsgDetail[MAX_JK_MSG_LEN];
};
//参数请求报文
struct  TMsgReqParam
{
    TMsgHeadInfo MsgHead;
    quint16 FileCnt;
    char FileInfo[2048];
};
//监控通知车道改变监控IP
struct TJKMsgUpdateMonIP
{
    TMsgHeadInfo MsgHead;
    char MonIP[20];
};
//数据保存结构体
struct TDataToSave
{
    // 发生时间
    quint64 OccurTime;
    //报文编码
    int MsgID;
    // 报文类型
    int MsgType;
    // 报文结构
    char* pMsg;
    // 报文结构长度
    int nMsgLen;
    int TransFlag1;
    int TransFlag2;
    int TransFlag3;
    int TransFlag4;
public:
    void clear()
    {
        OccurTime = 0;
        MsgID = 0;
        MsgType = 0;
        pMsg = NULL;
        nMsgLen = 0;
        TransFlag1 = 0;
        TransFlag2 = 0;
        TransFlag3 = 0;
        TransFlag4 = 0;
    }
};
//按入口关键字查询图像报文
struct TQryParam_Key
{
    //入口时间
    char EntryTime[14];
    //入口站
    char EnStation[6];
    //入口车道
    char EnLaneId[3];
    //入口车道类型
    quint8 EnLaneType;
    //入口流水号
    quint8 EnWastSN;
    //填充字节
    char Reservr[39];
};
struct TMsgQryEnImg
{
    TMsgHeadInfo msgHead;
    TQryParam_Key qryKey;
};

#pragma pack(pop)

void FillMsgHead(int MsgID,int MsgType, int ReceivedID,int laneId, int nOrgCode, TMsgHeadInfo& MsgHead);
#endif // MSGDEFINE_H
