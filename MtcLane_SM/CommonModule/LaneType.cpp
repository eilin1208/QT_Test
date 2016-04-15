#include "LaneType.h"

quint8 VehClass2Byte(CVehClass vc)
{
    switch(vc)
    {
    case VC_Car1: return 1;
    case VC_Car2: return 2;
    case VC_Car3: return 3;
    case VC_Car4: return 4;
    case VC_Truck1: return 11;
    case VC_Truck2: return 12;
    case VC_Truck3: return 13;
    case VC_Truck4: return 14;
    case VC_Truck5: return 15;
    default: break;
    }
    return 0;
}

quint8 Tran_LaneType(LaneType type)
{
    switch(type)
    {
    case LaneType_MtcIn: return 1;
    case LaneType_MtcOut: return 2;
    case LaneType_EtcIn: return 3;
    case LaneType_EtcOut: return 4;
    case LaneType_PortableIn: return 7;
    case LaneType_PortableOut: return 8;
    case LaneType_HandheldIn: return 9;
    case LaneType_HandheldOut: return 10;
    case LaneType_AutoCard: return 11;
    case ltNTC_OUT: return 12;
    case ltETC_FLAG: return 13;
    default: break;
    }
    return 0;
}

LaneType Byte2LaneType(quint8 type)
{
    switch(type)
    {
    case 1: return LaneType_MtcIn;
    case 2: return LaneType_MtcOut;
    case 3: return LaneType_EtcIn;
    case 4: return LaneType_EtcOut;
    case 7: return LaneType_AutoCard;
    default: break;
    }
    return ltNone;
}

quint8 VehType2Byte(CVehType type)
{
    switch(type)
    {
    case VT_None: return 0;
    case VT_Normal: return 1;
    case VT_Servant: return 5;
    case VT_MonthCar: return 6;
    case VT_ArmVeh: return 8;
    case VT_Polic: return 9;
    case VT_Motorcade:return 10;
    case VT_Rescue: return 11;
    case VT_RedCross: return 12;
    case VT_ForestFire: return 13;
    case VT_Other: return 14;
    case VT_LvTongIn: return 20;
    case VT_LvTongOut: return 21;
    case VT_Combine: return 22;
    case VT_Holiday: return 16;
    default: break;
    }
    return 0;
}

CVehType Byte2VehType(quint8 type)
{
    switch(type)
    {
    case 0: return VT_None;
    case 1: return VT_Normal;
    case 5: return VT_Servant;
    case 6: return VT_MonthCar;
    case 8: return VT_ArmVeh;
    case 9: return VT_Polic;
    case 10: return VT_Motorcade;
    case 11: return VT_Rescue;
    case 12: return VT_RedCross;
    case 13: return VT_ForestFire;
    case 14: return VT_Other;
    case 20: return VT_LvTongIn;
    case 21: return VT_LvTongOut;
    case 22: return VT_Combine;
    default: break;
    }
    return VT_None;
}

CVehClass Byte2VehClass(quint8 b)
{
    switch(b)
    {
    case 0: return VC_None;
    case 1: return VC_Car1;
    case 2: return VC_Car2;
    case 3: return VC_Car4;
    case 11: return VC_Truck1;
    case 12: return VC_Truck2;
    case 13: return VC_Truck3;
    case 14: return VC_Truck4;
    case 15: return VC_Truck5;
    default: break;
    }
    return VC_None;
}

quint8 Tran_MOrgType(TMsgOrgType t)
{
    switch(t)
    {
    case OT_MTCLANE: return '1';
    case OT_ETCLANE: return '2';
    case OT_STATION: return '3';
    case OT_CENTER: return '6';
    default: break;
    }
    return '0';
}

quint16 CardType2W(TCardType t)
{
    switch(t)
    {
    case CT_PassCard: return 0;
    case CT_TestPassCard: return 1;
    case CT_YBMPassCard: return 2;
    case CT_ServantCard: return 3;
    case CT_YFCard: return 4;
    case CT_OBU: return 5;
    case CT_CardBoxCard: return 6;
    case CT_BJCard: return 7;
    case CT_NoCard: return 8;
    case CT_PaperCard: return 9;
    case CT_MonthCard: return 21;
    case CT_PASMCard: return 24;
    case CT_ISAMCard: return 25;
    case CT_TestStoreCard: return 26;
    case CT_TestTallyCard: return 27;
    case CT_POStoreCard: return 28;
    case CT_POTallyCard: return 29;
    case CT_TollOperCard: return 51;
    case CT_MonOperCard: return 52;
    case CT_MainOperCard1: return 53;
    case CT_MainOperCard2: return 54;
    case CT_HighOperCard: return 55;
    default: break;
    }
    return 0;
}

CPayType Byte2PayType(quint8 b)
{
    switch(b)
    {
    case 0: return PYT_Unknow;
    case 1: return PYT_Cash;
    case 2: return PYT_StoreCard;
    case 3: return PYT_TallyCard;
    case 4: return PYT_ServantCard;
    case 5: return PYT_MonthCard;
    case 6: return PYT_Reserver1;
    case 7: return PYT_Reserver2;
    case 8: return PYT_UniPayCard;
    case 9: return PYT_OntCartoon;
    case 10: return PYT_ProvOutStore;
    case 11: return PYT_ProvOutTally;
    default: break;
    }
    return PYT_Unknow;
}

QString getLaneTypeName(int nType)
{
    switch(nType)
    {
    case LaneType_MtcIn:
        return "MTC入口";
    case LaneType_MtcOut:
        return "MTC出口";
    case LaneType_EtcIn:
        return "ETC入口";
    case LaneType_EtcOut:
        return "ETC出口";
    case LaneType_PortableIn:
        return "便携式入口";
    case LaneType_PortableOut:
        return "便携式出口";
    case LaneType_HandheldIn:
        return "手持机入口";
    case LaneType_HandheldOut:
        return "手持机出口";
    case LaneType_AutoCard:
        return "无人值守入口";
    case ltNTC_OUT:
        return "无人值守出口";
    default:
        return "";
    }
}
bool IsFullFreeVehTollMode(CVehType AVehType)
{
    switch(AVehType)
    {
    case VT_LvTongIn:
        return true;
    case VT_LvTongOut:
        return true;
    case VT_ArmVeh:
        return true;
    case VT_Polic:
        return true;
    case VT_Motorcade:
        return true;
    case VT_Rescue:
        return true;
    case VT_RedCross:
        return true;
    case VT_ForestFire:
        return true;
    case VT_Combine:
        return true;
    case VT_Servant:
        return true;
    case VT_Other:
        return true;
    default:
        return false;
    }
}
