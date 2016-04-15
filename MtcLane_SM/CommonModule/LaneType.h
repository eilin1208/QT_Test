#ifndef LANETYPE_H
#define LANETYPE_H

#pragma once
#include <QtGui>
#include <QVariant>
#include <QDateTime>


struct QTSYSTEMTIME {
    quint16 wYear;
    quint16 wMonth;
    quint16 wDayOfWeek;
    quint16 wDay;
    quint16 wHour;
    quint16 wMinute;
    quint16 wSecond;
    quint16 wMilliseconds;
public:
    bool operator <(const QTSYSTEMTIME & rp)
    {
        return compare(&wYear, &rp.wYear, 0);
    }

    QTSYSTEMTIME& operator =(const QTSYSTEMTIME &rp)
    {
        this->wYear=rp.wYear;
        this->wMonth=rp.wMonth;
        this->wDayOfWeek=rp.wDayOfWeek;
        this->wDay=rp.wDay;
        this->wHour=rp.wHour;
        this->wMinute=rp.wMinute;
        this->wSecond=rp.wSecond;
        this->wMilliseconds=rp.wMilliseconds;
        return  *this;
    }

    void copyFromOthers(const QTSYSTEMTIME *rp)
    {
        this->wYear=rp->wYear;
        this->wMonth=rp->wMonth;
        this->wDayOfWeek=rp->wDayOfWeek;
        this->wDay=rp->wDay;
        this->wHour=rp->wHour;
        this->wMinute=rp->wMinute;
        this->wSecond=rp->wSecond;
        this->wMilliseconds=rp->wMilliseconds;
    }
    bool compare(const quint16 *lp, const quint16 *rp, int index)
    {
        if(index == 7)
        {
            return (*lp) < (*rp);
        }
        if(index == 2)
        {
            lp++;
            rp++;
            index++;
        }
        if((*lp) == (*rp))
        {
            return compare(++lp, ++rp, ++index);
        }
        else
        {
            return (*lp) < (*rp);
        }
    }
    friend bool operator >= (QTSYSTEMTIME & lp, QTSYSTEMTIME & rp)
    {
        return !(lp < rp);
    }

    friend bool operator <=(QTSYSTEMTIME & lp, QTSYSTEMTIME & rp)
    {
        return !(lp > rp);
    }
    bool operator ==(const QTSYSTEMTIME & rp)
    {
        const quint16 * lpPtr = &wYear;
        const quint16 * rpPtr = &rp.wYear;
        for(int i = 0; i < 8; i++)
        {
            if((*lpPtr) != (*rpPtr))
            {
                return false;
            }
            ++lpPtr;
            ++rpPtr;

        }
        return true;
    }
    bool operator >(QTSYSTEMTIME rp)
    {
        return compare(&rp.wYear, &wYear, 0);
    }
    void clear()
    {
        wYear=0;
        wMonth=0;
        wDayOfWeek=0;
        wDay=0;
        wHour=0;
        wMinute=0;
        wSecond=0;
        wMilliseconds=0;
    }
};

const quint16 OLD_ORG_NETWORKID = 3201;
const short ORG_NETWORKID =5301;
const int ORG_TYPE_CENTER=21;                   //	收费中心
const int ORG_TYPE_SUBCENTER=22;                //	收费分中心
const int ORG_TYPE_STATION=23;                  //	收费站
const int ORG_TYPE_VIRTUAL_STA=25;              //	立交虚拟点（计算鲁通卡免费区间用）
//const int ORG_TYPE_ROAD_OWNER=40;               //	拆账小业主
//const int ORG_TYPE_ADM_REGION=43;               //	行政区域
const int ORG_TYPE_ROAD = 41;       //拆账路段
const int ORG_TYPE_ADM_REGION=42;	//	行政区域

//定义流水业务类型
const int LST_TYPE_NORMAL	 =1;		//发卡收费
const int LST_TYPE_MOTORBEGIN=2;		//车队开始
const int LST_TYPE_MOTORPASS =3;		//车队过车
const int LST_TYPE_MOTOREND=4;                  //车队结束
const int LST_TYPE_HEDGE=5;                     //冲减入口倒车出口重打

const int LST_TYPE_SUBPAY=6;			//分段支付
const int LST_TYPE_REPAY=7;			//补费
const int LST_TYPE_CARLEADOUT=7;		//车辆引出

const int LST_TYPE_VIOLATE_LOGOUT=20;           //下班闯关
const int LST_TYPE_VIOLATE=21;                  //闯关
const int LST_TYPE_VIOLATE_FALSEALARM=23;	//误报警
const int LST_TYPE_VIOLATE_HELDBACK =22;	//闯关被拦截

const int Listen_PortID=40005;
const int PassCardNum_LEN=8;
const int PaperCardNum_LEN=7;

const char DV_NONE = '0';//未知
const char DV_OK = '1';//正常
const char DV_FAILURE = '2';//故障
const char DV_OTHER = '3';//其他类型故障/坏卡
//车牌长度
const int MAX_VEHPLATE_LEN=12;
//全牌长度
const int FULL_VEHPLATE_LEN =5;

//卡盒最大装卡数
const int MAX_CARDBOX_COUNT = 90000;

const int BadCard_Nomral = 1;
const int BadCard_Factitious = 2;

// 参数文件
enum CCfgFile
{
    cfSysParaDic,
    cfVCCode,
    cfVCTrans,
    cfOrgCode,
    cfDisSection,
    cfBaseRate,
    cfTollCell,
    cfSpRate,
    cfAllRate,
    cfAreaCode,
    cfCardBList,        //卡黑名单
    cfVBList,           //车辆黑名单
    cfPSamWhite,        //PSam白名单
    cfVGList,           //车辆灰名单
    cfLaneEvenCode,     //
    cfSpWeighPara,
    cfOper,             //操作员名单
    cfPublish,          //公告信息
    cfShift,            //
    cfCardRange,
    cfLaneSound,        //车道语音
    cfLaneKey,
    cfUnpayList,        //欠费流水
    cfHolidayFree,
    cfEnd
};

//车牌颜色定义
enum VP_COLOR
{
    VP_COLOR_NONE=-1,
    VP_COLOR_BLUE=0,
    VP_COLOR_YELLOW,
    VP_COLOR_BLACK,
    VP_COLOR_WHITE
};


//报文头中机构类型
enum TMsgOrgType
{
    OT_MTCLANE,
    OT_ETCLANE,
    OT_STATION,
    OT_CENTER
};

enum CLaneStatus
{
    //未启动
    lsNone,
    //未登陆
    lsUnlogin,
    //正常工作
    lsNormalWorking,
    lsRepare,
    lsWaitCfg
};



enum LaneType{
    LaneType_MtcIn=1,		//	MTC入口
    LaneType_MtcOut=2,		//	MTC出口
    LaneType_EtcIn=3,		//	ETC入口
    LaneType_EtcOut=4,		//	ETC出口
    LaneType_PortableIn=5,          //	便携式入口
    LaneType_PortableOut=6,         //	便携式出口
    LaneType_HandheldIn=7,          //	手持机入口
    LaneType_HandheldOut=8,         //	手持机出口
    LaneType_AutoCard=9,		//	无人值守入口
    ltNTC_OUT = 10,  //无人值守出口
    ltETC_FLAG,  // 标识站车道
    //未知类型
    ltNone
};
QString getLaneTypeName(int nType);

// 车类定义
enum CVehType
{
    VT_None,        //未知
    VT_Normal,        //普通
    VT_Servant,     // 公务车
    VT_MonthCar,   // 月票车（包缴车）
    VT_ArmVeh,      //  军警车
    VT_Polic,       //紧急车
    VT_Motorcade,   //车队
    VT_Rescue,      //救灾车
    VT_RedCross,    //红十字会车
    VT_ForestFire,  //森林消防车
    VT_Other,       //其他车辆
    VT_LvTongIn,      //省内绿通车
    VT_LvTongOut,     //省外绿通车
    VT_Combine,      //联合收割车
    VT_Holiday = 16  //节假日免费
};


// 车型种类
enum CVehClass
{
    VC_None=0,
    VC_Car1=1,		// 客1
    VC_Car2,		// 客2
    VC_Car3,		// 客3
    VC_Car4,		// 客4
    VC_Reserver1,
    VC_Reserver2,
    VC_Reserver3,
    VC_Truck = 10,
    VC_Truck1,		// 货1
    VC_Truck2,		// 货2
    VC_Truck3,		// 货3
    VC_Truck4,		// 货4
    VC_Truck5,		// 货5

    VC_J1=21,		// 集1
    VC_J2			// 集2
};

//支付方式
enum CPayType
{
    PYT_Unknow,			//未知
    PYT_Cash,				    //现金
    PYT_StoreCard,			//云通储值卡支付	入出	通行信息记录在储值卡中
    PYT_TallyCard,			//云通记账卡支付	入出	通行信息记录在记账卡中
    PYT_ServantCard,    //云通公务卡
    PYT_MonthCard,      //云通包缴卡
    PYT_Reserver1,
    PYT_Reserver2,
    PYT_UniPayCard,			//银联卡支付	出
    PYT_OntCartoon,    //一卡通支付
    PYT_ProvOutStore,    //省外储值卡
    PYT_ProvOutTally,   //省外记账
    PYT_Free = 99       //免费
};

//监控回控事件
enum TMonReqID
{
    mqUnKnow,
    mqDevFault,   //设备故障
    mqLockBar,      //锁杆
    mqUnLockBar,    //解除锁杆
    mqMotocate,     //车队
    mqFrontLoop,    //禁止未检测到车辆时发卡-使用前线圈
    mqUnFrontLoop,  //允许未检测到车辆时发卡-不使用前线圈
    mqVLP,          //启用车牌识别
    mqUnVLP,       //禁用车牌识别
    mqWeight,      //使用计重设备 启用计重收费
    mqUnWeight,    //禁用计重设备  货车按车型收费
    mqVehClassDiff,  //车型不符
    mqVehTypeDiff,   //车类不符
    mqVLPDiff,       //车牌不符
    mqVehInfoDiff,   //车辆信息不符
    mqRepairCard,   //补卡
    mqChangeCard,   //换卡
    mqUFree,        //U型减免通行费
    mqEditWeightInfo, //编辑计重信息
    mqDownInv,        //换下票据
    mqChangInv,       //更改票号
    mqWasteTicket,  //废票
    mqSimulate,     //启用模拟
    mqUnSimulate,    //禁用模拟
    mqOverTime1,     //正常超时
    mqOverTime2,      //可疑超时
    mqCompelDownBox   //强制换下
};

//卡类型
enum TCardType
{
    CT_PassCard,     //0-正常通行卡
    CT_TestPassCard, //1-测试通行卡
    CT_YBMPassCard,  //2-预编码通行卡
    CT_ServantCard,  //3-公务卡
    CT_YFCard,       //4-预付卡
    CT_OBU,          //5- 电子标签
    CT_CardBoxCard,  //6-卡盒卡
    CT_BJCard,       //7-包缴卡
    CT_NoCard,       //8-无卡/无纸券
    CT_PaperCard,    //9-纸卡
    CT_Reserve10,CT_Reserve11,CT_Reserve12,CT_Reserve13,CT_Reserve14,
    CT_Reserve15,CT_Reserve16,CT_Reserve17,CT_Reserve18,CT_Reserve19,
    CT_Reserve20,   //10--20预留
    CT_MonthCard,   //21-月票卡
    CT_StoreCard,   //22-储值卡
    CT_TallyCard,		//23-记账卡
    CT_PASMCard,    //24-PSAM卡
    CT_ISAMCard,    //25-ISAM卡
    CT_TestStoreCard,//26-测试储值卡
    CT_TestTallyCard,//27-测试记账卡
    CT_POStoreCard,  //28-省外储值卡
    CT_POTallyCard,  //29-省外记帐卡
    CT_Reserve30,CT_Reserve31,CT_Reserve32,CT_Reserve33,CT_Reserve34,
    CT_Reserve35,CT_Reserve36,CT_Reserve37,CT_Reserve38,CT_Reserve39,
    CT_Reserve40,CT_Reserve41,CT_Reserve42,CT_Reserve43,CT_Reserve44,
    CT_Reserve45,CT_Reserve46,CT_Reserve47,CT_Reserve48,CT_Reserve49,
    CT_Reserve50,   //10--20预留
    CT_TollOperCard,//51-收费员身份卡
    CT_MonOperCard, //52-收费班长身份卡
    CT_MainOperCard1,//53-维修员工作卡
    CT_MainOperCard2,//54-维修员工作卡
    CT_HighOperCard  //55-高级维护员工作卡
};

//设备类型
enum TDevType
{
    DT_None,
    DT_NET,          // 网络状态  0,未知 1正常 2 故障
    DT_PRINTER,      //打印机 '0'，未知,'1', 正常 '2'，故障 '3',缺纸
    DT_CARDREADER,   //卡读写器 '0'，未知,'1',正常 '2',故障 无人职守车道上读写器
    DT_CARDREADER1,  //卡读写器 '0'，未知,'1',正常 '2',故障 无人职守车道下读写器
    DT_CARDREADER2,   //卡读写器 '0'，未知,'1',正常 '2',故障 备用读写器
    DT_FEEDISPLAY,   //费显  '0',未知 '1'，正长 '2'，故障
    DT_AUTOBAR,      //栏杆  '0'，未知 '1'，起 '2'，落
    DT_TRIGLOOP1,     //触发线圈1 '0'，未知 '1'，有车 '2'，无车
    DT_TRIGLOOP2,    //触发线圈2 '0'，未知 '1'，有车 '2'，无车
    DT_FRONTLOOP,    //前线圈 '0'，未知 '1'，有车 '2'，无车
    DT_BACKLOOP,     //后线圈 '0'，未知 '1'，有车 '2'，无车
    DT_CANOPYLIGHT,  //雨棚信号灯 '0'未知 '1'绿 '2' 红
    DT_PASSLIGHT,    //通行信号灯  '0',未知 '1'，绿 ，'2'红
    DT_ALARMDETECTOR,//脚踏报警器      '0' 未知  '1',正常 '2'报警'

    DT_ADDCHAR,      //字符叠加   '0',未知 '1'，正常 '2'故障
    DT_WEIGHT,       //计重设备   '0',未知 '1'，正常 '2'故障
    DT_PLATERECOG,   //车牌识别   '0'，未知，'1'，正常 '2'故障
    DT_VideoCard,    //视频采集卡  '0',未知,  '1' 正常  '2'故障
    DT_CCU,          //CCU模块（车道技术柜）'0',未知,  '1' 正常  '2'故障
    DT_RSU,          //RSU(ETC天线)        '0',未知   '1'，正常 '2'故障
    DT_AUTOCARDMGR,  //自动发卡机（或人工道小卡机)  '0',未知 '1'，正常 '2'故障
    DT_CARDMGR1,     //自动发卡机内小卡机 1 :'0' 未知 '1' 正常 '2'故障 '3'坏卡
    DT_CARDMGR2,     //自动发卡机内小卡机 2 :'0' 未知 '1'正常 '2'故障 '3'坏卡
    DT_CARDMGR3,     //自动发卡机内小卡机 3 :'0' 未知 '1'正常 '2'故障 '3'坏卡
    DT_CARDMGR4,     //自动发卡机内小卡机 4 : '0' 未知 '1'正常 '2'故障 '3'坏卡
    // liujian
    DT_ALARMLIGHT,   //黄闪报警器      '0' 未知  '1',正常 '2'报警'
    DT_ALARMSOUND,   //声音报警器      '0' 未知  '1',正常 '2'报警'
    DT_Reserve1,     //wumax ==修改  原名为Reserve1
    DT_Reserve2,
    DT_Reserve3,
    DT_Reserve4,
    DT_Reserve5,
    DT_Reserve6,
    DT_Reserve7
};

//车辆信息改变类型
enum TVehChangeType
{
    VCT_None,
    VCT_VC,  //车型
    VCT_VT,  //车情
    VCT_VLP, //车牌
    VCT_Clear
};

//计重信息修改类型
enum TModWeightType
{
    MWT_None,
    MWT_GetInfo,      //获取
    MWT_ModInfo,      //修改
    MWT_InInfo,       //插入
    MWT_DelInfo,      //删除
    MWT_MergeInfo,    //合并
    MWT_SplitInfo,    //拆分
    MWT_KOverTime,    //客车超限
    MWT_TipAxie,     //顶轴
    MWT_VirtualAxie,  //虚轴
    MWT_RushPounr     //冲轴
};

//票据操作类型
enum TInvOperType
{
    IOT_None,
    IOT_InvUp,
    IOT_InvDown,
    IOT_InvMod
};

//车道事件类型(发送车道事件报文时使用)
enum TLaneEventType
{
    LET_None,
    LET_UsesFrontLoop,    //01	启用前线圈不检测车辆发卡  1
    LET_NotUsesFrontLoop, //02	禁用前线圈不检测车辆发卡 1
    LET_UsesSimulate,     //03	启用【模拟】键   1
    LET_NotUsesSimulate,  //04	禁用【模拟】键  1
    LET_UsesVLP,          //05	启用车牌识别   1
    LET_NotUsesVLP,       //06	禁用车牌识别  1
    LET_UsesWeight,       //07	启用货车按车型收费   1
    LET_NotUsesWeight,    //08	禁用货车按车型收费 1
    LET_UsesCardMgr,      //09	启用卡机
    LET_NotUsesCardMgr,   //10	禁用卡机
    LET_RepairCard,       //11	补卡
    LET_Hail,             //12	车辆引出
    LET_ReHandle,         //13	重新处理
    LET_Motor,            //14	车队      1
    LET_PaperCard,        //15	纸卡     1
    LET_BadCard,          //16	坏卡    1
    LET_NoCard,           //17	无卡   1
    LET_PassInFail,       //18 密码输入有误
    LET_UXingCar,         // 19 U型车
    LET_UFree,            //20	U型减免通行费
    LET_SectionPay,       //21	分段付款
    LET_LockBar,          //22	锁杆       1
    LET_RemoveBar,        //23	栏杆解锁   1
    LET_PayFail,          //24	扣款失败
    LET_Balance,          //25	余额不足
    LET_WritePayFail,     //26	云通卡出口写卡失败放行
    LET_InvMod,           //27	票据处理 1
    LET_CardBox,          //28	卡箱操作
    LET_EditWeight,       //29	编辑计重信息   1
    LET_BackCar,          //30	倒车
    LET_ViolateFalseAlarm, //31 误报警
    LET_ViolateHeldBack,   //32   闯关被拦截
    LET_VehPlateDiff,     //33 车牌不符    ETC专用
    LET_VehClassDiff,     //34 车型不符    ETC专用
    LET_Reserve35,//预留
    LET_DevChange,        //36	设备变化     1
    LET_QueryEnInfo,      //37	入口信息查询
    LET_QueryEnImage,     //38	入口图像查询
    LET_EnVehBlack,       //39	入口车辆黑名单
    LET_ExVehBlack,       //40	出口车辆黑名单
    LET_EnCardBlack,      //41	入口卡黑名单
    LET_ExCardBlack,      //42	出口卡黑名单
    LET_EnVehInfoUn,      //43	入口云通卡车辆信息不符
    LET_ExVehInfoUn,      //44	出口云通卡车辆信息不符
    LET_PSAMNoWhite,      //45	车道PSAM不在白名单
    LET_VehQray,          //46	车辆灰名单
    LET_NoOBU,            //47	无OBU       ETC专用
    LET_OBUBlack,         //48	OBU黑名单    ETC专用
    LET_NotAreaCard,      //49	非本区域卡   ETC专用
    LET_OBUVLPUn,         //50	OBU绑定车牌不符   ETC专用
    LET_ParaUpdate,       //51	车道参数更新
    LET_NetStatus,        //52	网络状态
    LET_PayCardOverTime,  //53	云通卡过期
    LET_FalseCard,        //54	伪卡/非法卡/无效卡
    LET_VehWhite,          //55	车辆白名单
    LET_IllegalDown,      //56 OBU非法拆卸    ETC专用
    LET_OBUNoCard,        // 57 未插卡        ETC专用
    LET_CardError,         //58 卡片错误      ETC专用
    LET_NoEntryInfo,        //59 无入口信息   ETC专用
    LET_IllegalEntryInfo,    //60 非法入口信息 ETC专用
    LET_OBUOverTime,         //61 OBU过期
    LET_StayOverTime,        //62 超时停留
    LET_OverTimeCar,         //63 超时车
    LET_CardDuSai,            //64 滞卡
    LET_CancelRePrint,        //65 打印失败，取消重新打印
    LET_CancelPrint,           //66 打印失败，取消打印
    LET_TSBDS2B               //车型不符入小出大
};

//历史交易信息
struct THistoryRec
{
    QDateTime WasteTime;
    QString CardID;
    QString VehClass;
    QString VehPlate;
    int Money;
    quint32 PayCardBal;
    QString EnStation;
};

//程序关闭后进行的工作
enum TWorkToDoAfterClose
{
    wcNone,
    wcClose,
    wcReboot,
    wcShutDown,
    wcRestart
};

//班长认证信息
struct TAuthorizeInfo
{
    //授权类型
    quint8 AuthType;
    //授权工号
    char AuthID[9];
    //授权姓名
    char AuthName[10];
    //授权时间
    QDateTime AuthTime;
public:
    void clear()
    {
        AuthType = 0;
        memset(AuthID,0,sizeof(AuthID));
        memset(AuthName,0,sizeof(AuthName));
    }
};

//车辆入口信息
enum
{
    Entry_None,                             //空
    Entry_ByManual,                         //手工选择入口
    Entry_ByCard,                           //卡内记录
    Entry_ByQry                             //查询返回
};

struct CVehEntryInfo
{
    int nEntryType;				//入口信息类型
    quint32 dwQryStationID;                 //查询入口站
    quint32 dwEnStationID;                  //入口站
    char szEnStationName[16];               //收费站名称
    int nLaneID;				//入口车道
    QDateTime EnTime;			//入口时间
    quint8 bEnWasteSN;			//入口流水号
    quint8 bEnLaneType;			//入口车道类型
    quint32 dwEnOper;			//入口操作员
    quint8 bEnVC;				//入口车型
    quint8 bEnVT;				//入口车种
    quint8 bEnVLPC;				//入口车牌颜色
    char szEnVLP[16];			//入口车牌
    char szEnPSamTerm[13];                  //入口终端机编号
    int nFlagCount;				//标识站数
    char FlagInfo[10];			//标识站信息
    int nPassTime;				//车辆行驶时间,单位秒
public:
    CVehEntryInfo()
    {
        Clear();
    }
    void Clear()
    {
        nEntryType = Entry_None;
        dwQryStationID = 0;
        dwEnStationID =0;
        memset(szEnStationName,0,sizeof(szEnStationName));
        nLaneID =0;
        //memset(&EnTime,0,sizeof(QTSYSTEMTIME));
        bEnWasteSN =0;
        bEnLaneType =0;
        dwEnOper =0;
        bEnVC =0;
        bEnVT =0;
        bEnVLPC = VP_COLOR_NONE;
        memset(szEnVLP,0,sizeof (szEnVLP));
        memset(szEnPSamTerm,0,sizeof (szEnPSamTerm));
        nFlagCount =0;
        memset(FlagInfo,0,sizeof FlagInfo);
        nPassTime =0;
    }
};
//计算通行费
struct TCalculateMoneyInfo
{
    qint32 nTruckBasicMoney; //货车基本费额
    qint32 nTollMoney;			//出口基础金额(折扣前)
    qint32 nCardCost;			//应收卡成本
    qint32 nOverWeightMoney;	//超限加收通行费
    qint32 nFreeMoney;			//通行费免收金额 (折扣前)鲁通c卡
    qint32 nDisCountTollMoney;		//出口计算通行费（折扣后）
    qint32 nRebateMoney;		//折扣优惠金额只针对实收不含免收
public:
    void clear()
    {
        nTruckBasicMoney = 0;
        nTollMoney = 0;
        nCardCost = 0;
        nOverWeightMoney = 0;
        nFreeMoney = 0;
        nDisCountTollMoney = 0;
        nRebateMoney = 0;
    }
};

//应收通行费
struct TTollMoneyInfo
{
    qint32 nCashMoney;			//应实收通行费
    qint32 nCashCardMoney;		//应收卡成本
    qint32 nFreeMoney;      //免费金额
public:
    void clear()
    {
        nCashMoney = 0;
        nCashCardMoney = 0;
        nFreeMoney = 0;
    }
};
//实收通行费信息
struct TLastMoneyInfo
{
    qint32 nPaySn;                              //支付序列号
    qint32 nLastMoney;				//实收通行费
    qint32 nLastCardMoney;
    //实收卡成本
public:
    void clear()
    {
        nPaySn = 0;
        nLastMoney = 0;
        nLastCardMoney = 0;
    }
};
//免费区间信息
struct TFreeAreaInfo
{
    quint8 bFreeType;
    quint8 bFreeAreaCnt;
    char FreeArea[301];
public:
    void clear()
    {
        bFreeType = 0;
        bFreeAreaCnt = 0;
        memset(FreeArea,0,sizeof(FreeArea));
    }
};
//标识站信息
struct TFlagStationInfo
{
    quint8 bFlagStationCnt;
    char FlagStations[60];
public:
    void clear()
    {
        bFlagStationCnt = 0;
        memset(FlagStations,0,sizeof(FlagStations));
    }
};
//拆分信息
struct TSplitInfo
{
    quint32 wSplitNum;
    char SplitInfo[2701];//AAAABBBBBBBCCCCCCC（18位一个单元），
    //AAAA为业主代码，BBBBBBB为现金实收/A卡支付/B卡支付/C卡区间外拆分结果精确到分，
    //CCCCCCC为免收/欠费/C卡区间内拆分结果精确到分，
    //最多支持150个业主
    quint32 wCellNum;
    char CellInfo[1201];//AAABBBBB（8位一个单元），
    //AAA为收费单元代码，BBBBB为每收费单元所占里程比例，
    //最多支持150个收费单元
    quint32 wStationNum;
    char PathInfo[901];
public:
    void clear()
    {
        wSplitNum = 0;
        memset(SplitInfo,0,sizeof(SplitInfo));
        wCellNum = 0;
        memset(CellInfo,0,sizeof(CellInfo));
        wStationNum = 0;
        memset(PathInfo,0,sizeof(PathInfo));
    }
};
//票据信息
struct TInvInfo
{
    qint64 PreInvBeginNo; //票据修改前起号
    quint32 PreInvCount; //票据修改前张数
    qint64 InvBeginNo; //票据号码
    quint32 InvCount; //发票张数
    qint64 InvEndNO;   //票据号码
};
//打印票信息
struct TInvPrintInfo
{
    qint64 InvNum;
    quint16 InvCnt;
public:
    void clear()
    {
        InvNum = 0;
        InvCnt = 0;
    }
};
//卡盒信息
struct TCardBoxInfo
{
    quint8 CardBoxIndex;
    quint32 FlagCardID;
    quint32 CardBoxID;
    quint16 nCardCount;
};


struct CVehInfo
{
    //初判车型
    CVehClass PVehClass;
    //自动识别车型
    CVehClass AutoVehClass;
    //最终车型
    CVehClass VehClass;
    //初判车种
    CVehType PVehType;
    //最终车种
    CVehType VehType;
    //自动识别车牌
    char szAutoVehPlate[MAX_VEHPLATE_LEN+1];
    //自动识别车牌颜色
    int nAutoVehPlateColor;
    //最终车牌
    char szVehPlate[MAX_VEHPLATE_LEN+1];
    //最终车牌颜色
    int nVehPlateColor;
    //车速（公里/小时）
    quint32 dwVSpeed;
public:
    void Clear()
    {
        PVehClass = VC_None;
        VehClass = VC_None;
        AutoVehClass = VC_None;

        PVehType = VT_None;
        VehType = VT_None;

        memset(szAutoVehPlate,0,MAX_VEHPLATE_LEN+1);
        nAutoVehPlateColor = VP_COLOR_NONE;

        memset(szVehPlate,0,MAX_VEHPLATE_LEN+1);
        nVehPlateColor=VP_COLOR_NONE;
    }

    CVehInfo & operator = (const CVehInfo &b)
    {
        if(&b==this)
            return *this;
        PVehClass = b.PVehClass;
        AutoVehClass = b.AutoVehClass;
        VehClass = b.VehClass;

        PVehType = b.PVehType;
        VehType = b.VehType;

        memcpy(szAutoVehPlate, b.szAutoVehPlate, sizeof(szAutoVehPlate));
        nAutoVehPlateColor = b.nVehPlateColor;
        memcpy(szVehPlate, b.szVehPlate, sizeof(szVehPlate));

        nVehPlateColor = b.nVehPlateColor;
        dwVSpeed = b.dwVSpeed;
        return *this;
    }

    bool IsEmpty() const
    {
        return VehClass==VC_None || VehType==VT_None;
    }

    bool IsNotEmpty() const
    {
        return
                VC_None!=VehClass ||
                VT_None!=VehType ||
                VP_COLOR_NONE!=nVehPlateColor ||
                0<strlen(szVehPlate);
    }
};

//票据长度
#define MAX_INVOICE_LENGTH 12
//最大张数
#define MAX_INVOICE_COUNT 2000
//最大票号
#define MAX_INVOICE_NO 999999999999

inline QString GetVehColorQString(int nColor)
{
    switch(nColor)
    {
    case VP_COLOR_NONE: return "";
    case VP_COLOR_BLUE: return QObject::tr("蓝");
    case VP_COLOR_YELLOW: return QObject::tr("黄");
    case VP_COLOR_BLACK: return QObject::tr("黑");
    case VP_COLOR_WHITE: return QObject::tr("白");
    default:
        return "";
    }
}

Q_DECLARE_METATYPE(QTSYSTEMTIME)

struct TOBUInfo
{
    quint32 OBUID;
    QString RSUTerminalId;        //PSAM卡终端机编号
    char IssuerIdentifier[8];     //发行商代码
    char SerialNumber[16];        //应用序列号
    QDateTime DateofIssue;        //启用日期
    QDateTime DateofExpire;       //过期日期
    QString VehPlateNumber;       //车牌号
    quint8 VehPlateColor;         //车牌色
    CVehClass VehClass;           //车辆类型
    quint8 VehUserType;           //车辆用户类型
public:
    void clear()
    {
        OBUID = 0;
        RSUTerminalId.clear();
        memset(IssuerIdentifier,0,sizeof(IssuerIdentifier));
        memset(SerialNumber,0,sizeof(SerialNumber));
        VehPlateNumber.clear();
        VehPlateColor = 0;
        VehClass = VC_None;
        VehUserType = 0;
    }
    TOBUInfo &operator =(TOBUInfo &obuinfo)
    {
        if(&obuinfo == this)
            return *this;
        clear();
        OBUID=obuinfo.OBUID;
        RSUTerminalId=obuinfo.RSUTerminalId;
        memcpy(IssuerIdentifier,obuinfo.IssuerIdentifier,sizeof(IssuerIdentifier));
        memcpy(SerialNumber,obuinfo.SerialNumber,sizeof(SerialNumber));
        DateofIssue=obuinfo.DateofIssue;
        DateofExpire=obuinfo.DateofExpire;
        VehPlateNumber=obuinfo.VehPlateNumber;
        VehPlateColor=obuinfo.VehPlateColor;
        VehClass=obuinfo.VehClass;
        VehUserType=obuinfo.VehUserType;
        return *this;
    }
    void copy(const TOBUInfo& obuinfo)
    {
        OBUID=obuinfo.OBUID;
        RSUTerminalId=obuinfo.RSUTerminalId;
        memcpy(IssuerIdentifier,obuinfo.IssuerIdentifier,sizeof(IssuerIdentifier));
        memcpy(SerialNumber,obuinfo.SerialNumber,sizeof(SerialNumber));
        DateofIssue=obuinfo.DateofIssue;
        DateofExpire=obuinfo.DateofExpire;
        VehPlateNumber=obuinfo.VehPlateNumber;
        VehPlateColor=obuinfo.VehPlateColor;
        VehClass=obuinfo.VehClass;
        VehUserType=obuinfo.VehUserType;
    }
};

struct TOBUTranInfo
{
    quint32 OBUID;              //OBU编号
    QDateTime PurchaseTime;      //消费时间
    char PSAMTerminalID[12];     //终端机号
    quint32 PSAMTransSerial;    //PSAM卡交易序号
    quint16 ICCPayserial;       //CPU卡脱机交易序号
    quint8 TransType;           //交易类型
    quint32 CardRestMoney;      //交易后卡余额，高位在前，低位在后；
    quint32 TAC;                //TAC码
public:
    void clear()
    {
        OBUID=0;
        memset(PSAMTerminalID,0,sizeof(PSAMTerminalID));
        PSAMTransSerial=0;
        ICCPayserial=0;
        TransType=0;
        CardRestMoney=0;
        TAC=0;
    }
    void copy(const TOBUTranInfo& transinfo)
    {
        OBUID=transinfo.OBUID;
        PurchaseTime=transinfo.PurchaseTime;
        memcpy(PSAMTerminalID,transinfo.PSAMTerminalID,sizeof(PSAMTerminalID));
        PSAMTransSerial=transinfo.PSAMTransSerial;
        ICCPayserial=transinfo.ICCPayserial;
        TransType=transinfo.TransType;
        CardRestMoney=transinfo.CardRestMoney;
        TAC=transinfo.TAC;
    }
};

quint8 Tran_LaneType(LaneType type);
LaneType Byte2LaneType(quint8 type);
quint8 VehType2Byte(CVehType type);
CVehType Byte2VehType(quint8 type);
quint8 VehClass2Byte(CVehClass b);
CVehClass Byte2VehClass(quint8 b);
quint8 Tran_MOrgType(TMsgOrgType t);
quint16 CardType2W(TCardType t);
CPayType Byte2PayType(quint8 b);
bool IsFullFreeVehTollMode(CVehType AVehType);
inline bool IsTruck(int nVehClass)
{
    return (VC_Truck<nVehClass && nVehClass<=VC_Truck5);
}

inline bool IsCar(int nVehClass)
{
    return (VC_Car1<=nVehClass && nVehClass<=VC_Car4);
}


// 获得车型名称
inline QString GetVehClassName_LPCSTR(CVehClass wVehClass)
{
    if (wVehClass > 0)
    {
        switch(wVehClass)
        {
        case VC_None:		return "未知";
        case VC_Car1:		return "客1";
        case VC_Car2:		return "客2";
        case VC_Car3:		return "客3";
        case VC_Car4:		return "客4";

        case VC_Truck1:		return "货1";
        case VC_Truck2:		return "货2";
        case VC_Truck3:		return "货3";
        case VC_Truck4:		return "货4";
        case VC_Truck5:		return "货5";

        case VC_J1:			return "集1";
        case VC_J2:			return "集2";

        default:			return "未知";
        }
    }
    return "未知";
}
inline QString GetVehClassNoByName_LPCSTR(CVehType sVehClassName)
{
    if(sVehClassName>0)
        switch(sVehClassName)
        {
        case  VT_Normal  :              return  "普通车" ;
        case  VT_Servant :              return "公务车"  ;
        case  VT_MonthCar :             return  "包缴车" ;
        case  VT_ArmVeh :                return "军警车"  ;
        case  VT_Polic :                 return  "紧急车" ;
        case  VT_Motorcade :               return "车队"  ;
        case  VT_Rescue :                 return  "救灾车" ;
        case  VT_RedCross:             return  "红十字会车" ;
        case  VT_ForestFire:           return  "森林消防车" ;
        case  VT_Other:                  return  "其他车辆" ;
        case  VT_LvTongIn:             return  "省内绿通车" ;
        case  VT_LvTongOut:            return "省外绿通车"  ;
        case  VT_Combine:              return "联合收割车"  ;
        default:                         return"未知"      ;

        }
    return "";

}

inline QString GetVehClassName(CVehClass wVehClass)
{
    return GetVehClassName_LPCSTR(wVehClass);
}

// 轴组类型
enum CAxisType
{
    AXT_0 = 0,				// 0：其它轴型/异型轴
    AXT_A1T1,				// 1：单轴单胎；
    AXT_A1T2,				// 2：单轴双胎；
    AXT_A2T1,				// 3：双联轴单胎；
    AXT_A2T12,				// 4：双联轴单双胎；
    AXT_A2T2,				// 5：双联轴双胎；
    AXT_A3T1,				// 6：三联轴单胎；
    AXT_A3T2,				// 7：三联轴双胎；
    AXT_A3T21,				// 8：三联轴双单胎；
    AXT_A3T22				// 9：三联轴双双胎；
};

inline int GetAxisNumForAxisType(int nAxisType)
{
    switch(nAxisType)
    {
    case AXT_A1T1:				//1：单轴单胎；
        return 1;
    case AXT_A1T2:				//2：单轴双胎；
        return 1;
    case AXT_A2T1:				//3：双联轴单胎；
        return 2;
    case AXT_A2T12:				//4：双联轴单双胎；
        return 2;
    case AXT_A2T2:				//5：双联轴双胎；
        return 2;
    case AXT_A3T1:				//6：三联轴单胎；
        return 3;
    case AXT_A3T2:				//7：三联轴双胎；
        return 3;
    case AXT_A3T21:				//8：三联轴双单胎；
        return 3;
    case AXT_A3T22:				//9：三联轴双双胎；
        return 3;
    default:
        return 0;
    }
}


inline int GetVehClassNoByName(const QString& sVehClassName)
{
    if(sVehClassName == QObject::tr("普通车"))
    {
        return VT_Normal;
    }
    if(sVehClassName == QObject::tr("公务车"))
    {
        return VT_Servant;
    }
    if(sVehClassName == QObject::tr("包缴车"))
    {
        return VT_MonthCar;
    }
    if(sVehClassName == QObject::tr("军警车"))
    {
        return VT_ArmVeh;
    }
    if(sVehClassName == QObject::tr("紧急车"))
    {
        return VT_Polic;
    }
    if(sVehClassName == QObject::tr("车队"))
    {
        return VT_Motorcade;
    }
    if(sVehClassName == QObject::tr("救灾车"))
    {
        return VT_Rescue;
    }
    if(sVehClassName == QObject::tr("红十字会车"))
    {
        return VT_RedCross;
    }
    if(sVehClassName == QObject::tr("森林消防车"))
    {
        return VT_ForestFire;
    }
    if(sVehClassName == QObject::tr("其它车"))
    {
        return VT_Other;
    }
    if(sVehClassName == QObject::tr("省内绿通车"))
    {
        return VT_LvTongIn;
    }
    if(sVehClassName == QObject::tr("省外绿通车"))
    {
        return VT_LvTongOut;
    }
    if(sVehClassName == QObject::tr("联合收割车"))
    {
        return VT_Combine;
    }
    return VT_None;
}

const int ETC_BARRIER_FRONT=0;
const int ETC_BARRIER_MIDDLE=1;
const int ETC_BARRIER_BACK=2;


enum CFreeType
{
    FT_None,
    FT_FullFree,
    FT_RoadOwner,                           //	2-管理处
    FT_AdmRegion,                           //	3-行政区域
    FT_NeighborSta,                         //	4-相邻站
    FT_NeighborStaPlus                      //	5-相邻站+入出口
};
#endif // LANETYPE_H
