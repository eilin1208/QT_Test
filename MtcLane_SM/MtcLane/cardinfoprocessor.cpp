#include "cardinfoprocessor.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "formmainwindow.h"
#include "lanectrl.h"
#include "FareCalc/FareCalc.h"
#include "devicefactory.h"
#include "exittollitem.h"
#include "CommonModule/vehplatefunc.h"
#include "QDateTime"
#include "ParamModule/qareacodeparamfile.h"
#include "ParamModule/qcardlistparamfile.h"
#include "formauth.h"
#include "laneinfo.h"
#include "formmessage.h"
#include "formordinartprocess.h"
CardInfoProcessor::CardInfoProcessor(QObject *parent) :
    QObject(parent)
{
}

bool CardInfoProcessor::PreprocessTollCard(QCardReader* Reader)
{
    QSysParaDicParamFile* sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QSysParamInfo SysInfo = sysParam->GetSysParamInfo();
    //    bool result = false;
    Reader = getDeviceFactory()->getCardReader(DEV_CardReader1);

    //wumax ==    memset(&FCardTollInfo, 0, sizeof(FCardTollInfo));
    FCardTollInfo.clear();
    //wumax==    memset(&FTollCardBasicInfo, 0, sizeof(FTollCardBasicInfo));
    FTollCardBasicInfo.clear();
    // 读卡

    if (!Reader->ReadTollCard(FTollCardBasicInfo, FCardTollInfo))
    {
        ::showWarning(tr("读卡失败！"));
        return false;
    }
    LogMsg("lane",tr("读取通行卡信息，卡号：%1,入口站：%2").arg(FTollCardBasicInfo.M1BasicInfo.dwCardNum).arg(FCardTollInfo.dwStaId));
    // 判断卡类型
    if (CARD_TYPE_PASS_CARD != FTollCardBasicInfo.M1BasicInfo.bType)
    {
        ::showWarning(tr("非通行卡！"));
        return false;
    }
    //发送刷卡事件
    TollItem* pExit = (TollItem*)getLaneCtrl()->getTollItem();
    pExit->SendTollCardInfo(FTollCardBasicInfo,FVehEntryInfo);
    // 判断入口标识
    if (FCardTollInfo.bPassStatus == 0)
    {
        ::showWarning(tr("卡内无入口信息！"));
        return false;
    }
    //维护状态发卡，只能在维护状态下收卡 （0-正常发卡、1-维护发卡）
    // 收费员不能处理测试员发出的通行卡
    if ((lsNormalWorking == getLaneInfo()->getShiftState()) && (0 != (FCardTollInfo.bMaintenance)))
    {
        ::showWarning(tr("该卡为测试通行卡！"));
        return false;
    }
    // 测试员不能处理收费员发出的通行卡
    if((lsRepare == getLaneInfo()->getShiftState()))
    {
        ::showWarning(tr("该卡非测试通行卡！"));
        return false;
    }
    //检测入口站是否合法
    if (FCardTollInfo.dwStaId != SysInfo.m_nStationID)
    {
        if ( ! FareCalc::VerifyEntryStaId(FCardTollInfo.dwStaId,SysInfo.m_nStationID))

        {
            LogMsg("lane",tr("无效入口站，站代码:%1").arg(FCardTollInfo.dwStaId));
            return false;
        }
    }
    return  true;
}

bool CardInfoProcessor::ProcessOpenCardEvent_TollCard(int nReaderId)
{
    clearSaveInfo();
    bool result = false;
    QCardReader* Reader = getDeviceFactory()->getCardReader((DEVNO)nReaderId);
    if (! PreprocessTollCard(Reader))
    {
        return false;
    }
    //记录入口信息
    SetEntryInfo();
    //记录出口输入信息
    SetExitTollItem();
    //比较车辆信息，有班长授权需要关闭读卡线程
    if(CheckEntryVehInfo())
    {
        result = ProcEntryInfo();
    }
    return result;


}

void CardInfoProcessor::SetEntryInfo()
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    FVehEntryInfo.bEnVLPC = -1;//和下面入口车牌区别-------------------------
    FVehEntryInfo.nEntryType = Entry_ByCard;//入口信息类型
    FVehEntryInfo.dwQryStationID = 0;//查询入口站
    FVehEntryInfo.dwEnStationID = FCardTollInfo.dwStaId;//入口站
    FVehEntryInfo.nLaneID = FCardTollInfo.bLaneId;//入口车道
    FVehEntryInfo.EnTime = FCardTollInfo.PassTime;//入口时间
    FVehEntryInfo.bEnWasteSN = FCardTollInfo.bRecIndex;//入口流水号
    FVehEntryInfo.bEnLaneType = FCardTollInfo.bPassStatus;//入口车道类型
    FVehEntryInfo.dwEnOper = FCardTollInfo.dwOpId;//入口操作员
    FVehEntryInfo.bEnVC = FCardTollInfo.bVehClass;//入口车型
    FVehEntryInfo.bEnVT = FCardTollInfo.bVehTollType;//入口车种
    FVehEntryInfo.bEnVLPC = FCardTollInfo.bVehPlateColor;//入口车牌颜色
    memcpy(FVehEntryInfo.szEnVLP, FCardTollInfo.szVehPlate,sizeof(FVehEntryInfo.szEnVLP));			//入口车牌
    Raw2HexStr(FVehEntryInfo.szEnPSamTerm,FCardTollInfo.baPsamTermId,6); //入口终端机编号
    FVehEntryInfo.nFlagCount = FCardTollInfo.bFlagStaCount;//标识站数
    memcpy(FVehEntryInfo.FlagInfo,FCardTollInfo.baFlagStaInfo,sizeof(FVehEntryInfo.FlagInfo));			//标识站信息
    pExit->SetVehEntryInfo(FVehEntryInfo);
    //费显显示入口站
    //getDeviceFactory()->getFD()->ShowEntrySta(FVehEntryInfo.szEnStationName);
    //QDebug() << "end SetEntryInfo...";
}

void CardInfoProcessor::SetExitTollItem()
{
    //QDebug() << "start SetExitTollItem...";
    ExitTollItem * pExit=(ExitTollItem*)getLaneCtrl()->getTollItem();
    //保存卡信息
    pExit->setFdwBlance(FdwBalance);
    pExit->setPhysicalCardType(m_PhysicalCardType);
    pExit->SetProCardInfo(FProCardBasicInfo);
    pExit->SetCardTollInfo(FCardTollInfo);
    pExit->SetTollCardInfo(FTollCardBasicInfo);
    if (TYPE_PRO == m_PhysicalCardType)
    {
        // 云通卡
        LogMsg("lane",tr("卡类型为云通卡"));
        pExit->setCardType(FProCardBasicInfo.bType);

    }
    else if( (TYPE_M1_S50_BLOCK == m_PhysicalCardType) || (TYPE_M1_S50_MAD == m_PhysicalCardType)|| (TYPE_M1_S70_BLOCK == m_PhysicalCardType)||(TYPE_M1_S70_MAD == m_PhysicalCardType))
    {
        // 通行卡、老记账卡
        LogMsg("lane",tr("卡类型为通行卡或老记账卡"));
        pExit->setCardType(FTollCardBasicInfo.M1BasicInfo.bType);
    }
    else
    {
        pExit->setCardType(-1);
    }

}

bool CardInfoProcessor::CheckEntryVehInfo()
{
    ExitTollItem * pExit = (ExitTollItem *)getLaneCtrl()->getTollItem();
    QString sError;
    // TSupRespResult srRet;
    bool bChecked;
    QSysParaDicParamFile* sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QSysParamInfo SysInfo = sysParam->GetSysParamInfo();
    sError ="";
    bChecked = true;

    if(pExit->getVehEntryInfo().nEntryType == Entry_ByManual || pExit->getVehEntryInfo().nEntryType == Entry_None)
    {
        return true;
    }
    //便于兼容新旧卡，在进行车型比较时，都是货车不比较
    //只要有一个车型为客车就进行比较  前提是计重收费的情况
    if(! IsTruck(pExit->getVehEntryInfo().bEnVC)||(!IsTruck(VehClass2Byte(pExit->m_VehInfo.VehClass))))
    {
        if(FCardTollInfo.bPassStatus != Tran_LaneType(LaneType_AutoCard)&&(pExit->getVehEntryInfo().bEnVC !=8))
        {
            if(pExit->getVehEntryInfo().bEnVC != VehClass2Byte(pExit->m_VehInfo.VehClass))
            {
                //TODO:车型信息比较
                if(pExit->getVehEntryInfo().bEnVC > VehClass2Byte(pExit->m_VehInfo.VehClass))
                {
                    getOrdinaryWidget()->hide();
                    getAuth()->setMonType(mqVehClassDiff);
                    int nRet = getAuth()->doModalShow();
                    getOrdinaryWidget()->show();
                    bChecked = (nRet == 1);
                }
                sError ="车型";
            }
        }
    }

    if ((! CompareVehPlate(pExit->m_VehInfo.szVehPlate,pExit->getVehEntryInfo().szEnVLP))||((pExit->m_VehInfo.nVehPlateColor>0) && (pExit->getVehEntryInfo().bEnVLPC>0)&& (pExit->m_VehInfo.nVehPlateColor!=pExit->getVehEntryInfo().bEnVLPC)))
    {
        if (SysInfo.m_bVLPCheck )
        {
            sError.append("车牌");
            //bChecked = false;
        }
    }
    else
    {
        ::showWarning(tr("车牌与入口信息不符").arg(GetVehColorQString(pExit->getVehEntryInfo().bEnVLPC)).arg(pExit->getVehEntryInfo().szEnVLP));
    }

    return  bChecked;


}
/*-------------------------------------------------------------------------------
  过程名:    ProcEntryInfo
  参数:      无
  返回值:    bool
  用途:      验证入口信息合法性，并根据相应的信息计算费率
-------------------------------------------------------------------------------*/
bool CardInfoProcessor::ProcEntryInfo()
{
    ExitTollItem * pExit = (ExitTollItem *)getLaneCtrl()->getTollItem();
    pExit->m_VehInfo.VehClass = (CVehClass)getLaneInfo()->getVehClass();
    //qDebug()<<"getLaneInfo()->getVehClass()"<<getLaneInfo()->getVehClass();
    pExit->m_VehInfo.VehType = (CVehType)getLaneInfo()->getVehType();
    QDateTime time =QDateTime::currentDateTime();
    pExit->SetOccurTime(time);

    //无卡最大里程收费不判断入口站是否合法
    if((!FareCalc::VerifyEntryStaId(FVehEntryInfo.dwEnStationID,getLaneInfo()->getOrgInfo().sOrgCode.toUInt()))&&(!pExit->setiNoCard(11)))
    {
        ::showWarning(tr("入口站:%1,不合法!").arg(FVehEntryInfo.dwEnStationID));
        return false;
    }

    return  true;
}
/*-------------------------------------------------------------------------------
  说明:      读取CPU卡信息，并检查卡信息是否合法
  参数:     QCardReader Reader
  返回值:    true:卡信息合法，可继续处理；
             false:卡信息不合法，需要继续检测卡片

-------------------------------------------------------------------------------*/
bool CardInfoProcessor::PreprocessProCard(QCardReader *Reader)
{


    QSysParaDicParamFile* sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QSysParamInfo SysInfo = sysParam->GetSysParamInfo();

    QAreaCodeParamFile * pAreaCodeTable = (QAreaCodeParamFile *)getParamMgr()->GetParamFile(cfAreaCode);
    SAreaCodeValue AreaCode ;
    FProCardBasicInfo.clear();
    FdwBalance = 0;
    FCardTollInfo.clear();
    FCardTollInfo.bVehPlateColor = 0xFF;
    FProCardBasicInfo.bVehPlateColor = 0xFF;
    // 读卡
    if(! Reader->ReadProCard(FProCardBasicInfo,FdwBalance,FCardTollInfo))
    {
        ::showWarning(tr("读卡失败！"));
        return false;
    }
    LogMsg("lane",tr("读取云通卡信息，物理卡号：%1,卡号：%2,余额：%3").arg(FProCardBasicInfo.dwCardId).arg(FProCardBasicInfo.szCardNo).arg(FdwBalance));
    // 判断卡类型
    if (!IsYTCard(FProCardBasicInfo))
    {
        ::showWarning(tr("非云通卡"));
        return false;
    }
    //TODO:这里判断卡类型，并赋值卡用户类型
    if(FProCardBasicInfo.bType == CARD_TYPE_ALLFREE_CARD ||
            FProCardBasicInfo.bType == CARD_TYPE_ALL_BAOJIAO_CARD)
    {
        FProCardBasicInfo.bUserType = CARD_UserType_FullFree;
    }
    if(FProCardBasicInfo.bType >= CARD_TYPE_ROAD_FREE_STORE &&
            FProCardBasicInfo.bType <= CARD_TYPE_ROAD_BAOJIAO_TALLY)
    {
        FProCardBasicInfo.bUserType = CARD_UserType_RegionFree;
    }
    ExitTollItem * pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //发送刷卡事件
    pExit->SendProCardInfo(FProCardBasicInfo,FdwBalance,FVehEntryInfo);
    //费显显示扣款前余额
    if( CARD_TYPE_STORE_CARD == FProCardBasicInfo.bType)
    {
        getDeviceFactory()->getFD()->ShowBalanceBeforeDeduct(FdwBalance,false);
    }
    else
    {
        getDeviceFactory()->getFD()->ShowBalanceBeforeDeduct(-1,true,false);
    }
    // 判断入口标识
    //判断存在入口标识
    if ((ORG_NETWORKID != FCardTollInfo.wNetworkId) || (0==FCardTollInfo.bPassStatus))
    {
        ::showWarning(tr("卡内无入口信息！"));
        return false;
    }

    // 收费员不能处理测试员发出的通行卡
    if ((lsNormalWorking == getLaneInfo()->getShiftState()) && (0 != (FCardTollInfo.bMaintenance)))
    {
        ::showWarning(tr("该卡为测试通行卡！"));
        return false;
    }

    // 测试员不能处理收费员发出的通行卡
    if ((lsRepare == getLaneInfo()->getShiftState()) && (0 == (FCardTollInfo.bMaintenance)))
        //		and (0 = ($F0 and FCardTollInfo.bPassStatus))) then
    {
        ::showWarning(tr("该卡非测试通行卡！"));
        return false;
    }
    //TODO 获得当前时间暂时没用到
    QDateTime curTime = QDateTime::currentDateTime();

    //检测入口站是否合法
    if (FCardTollInfo.dwStaId != getLaneInfo()->getOrgInfo().sOrgCode.toUInt())
    {
        if (! FareCalc::VerifyEntryStaId(FCardTollInfo.dwStaId, getLaneInfo()->getOrgInfo().sOrgCode.toUInt()))
        {
            LogMsg("lane",tr("入口站:%1,不合法").arg(FCardTollInfo.dwStaId));
            //            ShowWarnMsg(Format('入口站:%d,不合法!',[FCardTollInfo.dwStaId]));
            return false;
        }
    }
    // 判断发卡区域是否为可处理区域
    QString sAreaCode = QString::number(FProCardBasicInfo.wNetworkId);
    if(! pAreaCodeTable->QryAreaCode(sAreaCode,&AreaCode))
    {
        LogMsg("lane",tr("网络号:%1,非合法区域卡").arg(FProCardBasicInfo.wNetworkId));
        return false;
    }
    return  true;

}

bool CardInfoProcessor::ProcessOpenCardEvent_ProCard(int nReaderId)
{
    clearSaveInfo();
    QCardReader *Reader = getDeviceFactory()->getCardReader((DEVNO)nReaderId);
    QString sErrorMsg;
    bool result = false;
    if (! PreprocessProCard(Reader))
    {
        return false;
    }
    //    //检查卡片有效性、黑名单
    //    //不符时，转入现金支付
    CheckCardValidity();

    //    //比较卡内绑定车辆信息与输入的信息是否相符
    //    //TODO车卡信息不一致时，转现金支付 ,不需要拦截
    CompareVehProCard(sErrorMsg);

    //    //记录入口信息
    SetEntryInfo();

    //    //记录出口输入信息
    SetExitTollItem();

    //比较卡内入口信息与输入的信息是否相符
    if(CheckEntryVehInfo())
    {
        result = ProcEntryInfo();
    }

    if (sErrorMsg.length() > 0)
    {
        ::showWarning(sErrorMsg);
    }

    return result;

}

void CardInfoProcessor::SetPhyCardType(int nCardType)
{
    m_PhysicalCardType = (CARD_TYPE_PHYSICAL)nCardType;
}

bool CardInfoProcessor::CheckCardValidity()
{
    QDateTime curTime = QDateTime::currentDateTime();
    QCardListParamFile* pCardListTable = (QCardListParamFile*)getParamMgr()->GetParamFile(cfCardBList);
    quint16 CardBType ;//卡黑名单类型
    QString CardBDesc;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    // TODO 判断卡片在有效期
    if ((curTime < FProCardBasicInfo.StartTime)||(FProCardBasicInfo.ExpireTime < curTime))
    {
        ::showWarning(tr("过期卡!"));
        pExit->setExceptCard(true);
    }
    //判断卡片在黑名单
    QString sCardNet = QString::number(FProCardBasicInfo.wNetworkId);
    if(pCardListTable->IsBlackCard(FProCardBasicInfo.szCardNo,sCardNet,FProCardBasicInfo.bType,CardBType,CardBDesc))
    {
        LogMsg("lane",tr("原因:%1,卡号:%2,卡黑名单！").arg(CardBDesc).arg(FProCardBasicInfo.szCardNo));
        pExit->setExceptCard(true);
        pExit->setCardBlack(true);
    }
    return  true;
}

bool CardInfoProcessor::CompareVehProCard(QString &sError)
{
    QString tmpStr;
    int vlpLen;
    bool bChecked;
    ExitTollItem * pExit = (ExitTollItem *)getLaneCtrl()->getTollItem();
    pExit->setExceptCard(false);
    QSysParaDicParamFile* sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QSysParamInfo SysInfo = sysParam->GetSysParamInfo();

    // NOTE:出口车型还需与计重比较
    sError = "";
    bChecked = true;

    if (FProCardBasicInfo.bVehClass > 0)
    {
        //便于兼容新旧卡，在进行车型比较时，都是货车不比较
        //只要有一个车型为客车就进行比较  前提是计重收费的情况
        if ((!IsTruck(FProCardBasicInfo.bVehClass))||(!IsTruck(VehClass2Byte( pExit->m_VehInfo.VehClass))))
        {
            if (FProCardBasicInfo.bVehClass != VehClass2Byte(pExit->m_VehInfo.VehClass))
            {
                sError = "车型、";
                bChecked = false;
            }
        }
        // 车牌绑定的云通卡必须输入全车牌，车牌长度>=8
        tmpStr = GB2312toUtf8(FProCardBasicInfo.szVehPlate);
        vlpLen = strlen(pExit->m_VehInfo.szVehPlate);
        LogMsg("lane",tr("车牌号码：%1，长度：%2").arg(tmpStr).arg(vlpLen));
        if ((!CompareVehPlate(pExit->m_VehInfo.szVehPlate,tmpStr.trimmed().toLocal8Bit().data()))
                ||((pExit->m_VehInfo.nVehPlateColor != -1)
                   &&(FProCardBasicInfo.bVehPlateColor != 0xFF)
                   &&((quint8)(pExit->m_VehInfo.nVehPlateColor)!= FProCardBasicInfo.bVehPlateColor)))
        {
            if (SysInfo.m_bVLPCheck)
            {

            }
        }
        else
        {
            LogMsg("lane",tr("车牌与卡内信息不符(卡内:%1%2)").arg(GetVehColorQString(FProCardBasicInfo.bVehPlateColor).arg(tmpStr)));
        }
    }
    // 判断车类不符
    if (IsYTCard(FProCardBasicInfo))
    {
        // 公务卡、包缴卡使用卡内车类，输入无效，刷卡后车类变为卡内车类
        pExit->m_VehInfo.VehType = Byte2VehType(FProCardBasicInfo.bVehTollType);
        if (FProCardBasicInfo.bVehTollType == 0)
        {
            pExit->m_VehInfo.VehType = VT_Normal;
        }
    }

    else if((CARD_TYPE_TALLY_CARD == FProCardBasicInfo.bType)||(CARD_TYPE_STORE_CARD==FProCardBasicInfo.bType))
    {
        if (VT_Normal != pExit->m_VehInfo.VehType)
        {
            sError.append("车种");
        }
    }
    pExit->setExceptCard(!bChecked);
    if (! bChecked)
    {
        sError.remove(sError.length()-1,1);
        sError.append("与卡内信息不符，请核实！");
        ::showWarning(sError);
        getLaneCtrl()->DoLaneEvent(LET_ExVehInfoUn);
        return false;
    }
    return true;
}


CardInfoProcessor* getCardProcessor()
{
    static CardInfoProcessor processor;
    return &processor;
}

bool CardInfoProcessor::ClearProCardState()
{
    TProCardBasicInfo ProCardBasicInfo;
    quint32 dwBalance = 0;
    TCardTollInfo CardTollInfo;
    QCardReader* reader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    QString sCard = pExit->getProCardBasicInfo().szCardNo;
    if(pExit->getChangeEntryFlag())
    {
        return true;
    }
    bool bOpen = false;
    for(int i = 0; i < 3; i++)
    {
        bOpen = reader->OpenCard();
        if(bOpen)
        {
            break;
        }
        else
        {
        }
    }
    if(!bOpen)
    {
        ::showWarning(tr("请将云通卡（%1）放到读卡器上").arg(pExit->getProCardBasicInfo().szCardNo), false, FormWarning::WARNING);
        return false;
    }
    if(!reader->ReadProCard(ProCardBasicInfo,dwBalance, CardTollInfo ))
    {
        ::showWarning(tr("读卡失败"), false, FormWarning::WARNING);
        return false;
    }
    if(reader->GetCardId().compare(sCard) != 0)
    {
        LogMsg("lane", tr("getCardId:%1，保存的印刷卡号：%2，两个相同，无法写").arg(reader->GetCardId(), sCard));
        ::showWarning(tr("请刷云通卡(%1)").arg(sCard), false, FormWarning::WARNING);
        return false;
    }

    //判断卡类型
    if(TYPE_PRO != pExit->getPhysicalCardType())
    {
        ::showWarning(tr("非云通卡，请刷云通卡（%1）").arg(sCard));
        return false;
    }
    //写卡、清除入口标志
    QDateTime writeProTime = QDateTime::currentDateTime();
    if(!reader->ProConsume(sCard, 0, writeProTime, *getLaneInfo()->getProCardInfo()))
    {
        ::showWarning(tr("写卡失败!"));
        return false;
    }
    pExit->SetProCardConsumeInfo(*getLaneInfo()->getProCardInfo());
    pExit->SetOccurTime(writeProTime);
    //写卡成功后,将改写入口标志值改为true
    pExit->setChangeEntryFlag(true);
    return true;
}

bool CardInfoProcessor::ClearPassCardState()
{
    QCardReader* reader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    QString sCardId = QString::number(pExit->getTollBasicInfo().M1BasicInfo.dwCardNum);
    if(pExit->getChangeEntryFlag())
    {
        return true;
    }
    //尝试三次打开卡片
    bool bOpenSuccess = false;
    for(int i = 0; i < 3; i++)
    {
        if(reader->OpenCard())
        {
            bOpenSuccess = true;
            break;
        }
    }
    if(!bOpenSuccess)
    {
        ::showWarning(tr("请将通行卡（%1）放到读卡器上").arg(sCardId), false, FormWarning::WARNING);
        return false;
    }

    //读通行卡
    //因为使用印刷号标识卡，在写之前一定要读一次。
    TTollCardBasicInfo cardBasicInfo;
    TCardTollInfo cardTollInfo;
    if(!reader->ReadTollCard(cardBasicInfo, cardTollInfo))
    {
        ::showWarning(tr("读通行卡失败!"), false, FormWarning::WARNING);
        return false;
    }

    if(cardBasicInfo.M1BasicInfo.bType != CARD_TYPE_PASS_CARD)
    {
        ::showWarning(tr("非通行卡,处理失败!"), false, FormWarning::WARNING);
        return false;
    }
    if(sCardId.compare(reader->GetCardId()) != 0)
    {
        ::showWarning(tr("请刷通行卡（%1）").arg(sCardId));
        return false;
    }

    //写卡
    if(!reader->WriteExitInfo(sCardId, getDataFileMgr()->getCardBoxInfo().CardBoxID))
    {
        ::showWarning(tr("写卡失败!"), false, FormWarning::WARNING);
        return false;
    }
    pExit->SetOccurTime(QDateTime::currentDateTime());
    getDataFileMgr()->AddOneCard();
    //判断卡盒是否已经满
    if(getDataFileMgr()->getCardBoxInfo().nCardCount >= MAX_CARDBOX_COUNT)
    {
        //提示收费员下班换卡盒
        getMsgWidget()->setTitle(tr("卡盒已满"));
        getMsgWidget()->setMessage(tr("卡盒已满\n请立刻下班更换卡盒"));
        getMsgWidget()->doModalShow();
    }
    pExit->setChangeEntryFlag(true);
    return true;
}

void CardInfoProcessor::clearSaveInfo()
{
    FVehEntryInfo.Clear();
    //m_PhysicalCardType = TYPE_NULL;
    FCardTollInfo.clear();
    FTollCardBasicInfo.clear();
    FProCardBasicInfo.clear();
    FdwBalance = 0;
    m_bVehPlate = false;
}













