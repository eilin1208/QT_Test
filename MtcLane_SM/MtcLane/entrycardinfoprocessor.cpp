#include "entrycardinfoprocessor.h"
#include "lanectrl.h"
#include "laneinfo.h"
#include "formmainwindow.h"
#include "devicefactory.h"
#include "tollitem.h"
#include "CardReaderArm/qcardreader.h"
#include "exittollitem.h"
#include <QDateTime>
#include "ParamModule/qcardlistparamfile.h"
#include "ParamModule/qareacodeparamfile.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "CommonModule/vehplatefunc.h"
#include "CommonModule/MsgDefine.h"
#include "datafilemgr.h"
Entrycardinfoprocessor ::Entrycardinfoprocessor(QObject *parent):
    QObject(parent)
{

}


bool Entrycardinfoprocessor::Prompt()
{
    // 判断上一辆车处理完成
    if(!getLaneCtrl()->CanDoNextCar())
    {
        ::showWarning(tr("前车尚未离开,暂不处理"));
        return false;
    }
    // 判断是否允许前线圈无车发卡
    if(!getLaneCtrl()->getRepairCard() && getLaneInfo()->getUseFrontLoop() == 1)
    {
        // 判断前线圈存在车辆
        if( getLaneInfo()->isEntryLane() && !getDeviceFactory()->getIOBoard()->GetDIPortStatus(DI_LoopFront))
        {
            ::showWarning(tr("前线圈无车,不允许发卡"),true);
            return false;
        }
    }
    // 判断车辆信息输入完整
    TollItem *pExit = (TollItem *)getLaneCtrl()->getTollItem();
    if ((pExit->m_VehInfo.VehClass == VC_None)||
            (pExit->m_VehInfo.VehType == VT_None))
    {
        ::showWarning(tr("请按下【车型】键输入车型"),true);
    }
    else
    {
        ::showWarning(tr("请修改车种或按【确定】键刷卡"),true);
    }
    return true;
}
//写卡成功放行车辆
void Entrycardinfoprocessor::EntryWriteCardSucComplate()
{
    //保存临时数据
    getLaneCtrl()->Complete(m_CardTollInfo.PassTime);
    //抬杆放行
    getLaneCtrl()->ConfirmComplete();
}

void Entrycardinfoprocessor::setCardBoxId(int boxId)
{
    m_CardTollInfo.dwCardBoxId = boxId;
}

bool Entrycardinfoprocessor::Processopencardevent_Procard(int nReaderId)
{
    QString sCardId;
    // 读卡相关工作
    if (!ReadProCard(nReaderId))
    {
        return false;
    }

    // 比较车辆信息
    if(getLaneInfo()->isAutoLane())
    {
    }
    else
    {
        if (!CompareVehProCard())
        {
            return false;
        }
    }
    SaveEntryTollItem();

    return true;
}
// 读CPU卡并判断卡片可用性
bool Entrycardinfoprocessor::ReadProCard(int nReaderId)
{
    QCardReader *reader = getDeviceFactory()->getCardReader((DEVNO)nReaderId);
    QDateTime curTime = QDateTime::currentDateTime();
    quint16  CardBType ;//卡黑名单类型
    QString CardBDesc;//卡黑名单描述
    // 读卡
    if (!reader->ReadProCard(m_ProCardBasicInfo, m_dwBalance, m_CardTollInfo))
    {
        ::showWarning(tr("读卡失败！"),true);
        return false;
    }
    TollItem * pExit = (TollItem*)getLaneCtrl()->getTollItem();
    pExit->SendProCardInfo(m_ProCardBasicInfo,m_dwBalance);
    QAreaCodeParamFile * pAreaCodeTable = (QAreaCodeParamFile *)getParamMgr()->GetParamFile(cfAreaCode);
    SAreaCodeValue AreaCode ;
    // 判断发卡区域是否为可处理区域
    QString sAreaCode = QString::number(m_ProCardBasicInfo.wNetworkId);
    if(! pAreaCodeTable->QryAreaCode(sAreaCode,&AreaCode))
    {
        LogMsg("lane",tr("网络号:%1,无效区域卡").arg(m_ProCardBasicInfo.wNetworkId));
        return false;
    }

    // 判断存在入口标识
    if ( m_CardTollInfo.bPassStatus != 0) // 存在入口标识
    {
        LogMsg("lane",tr("入口CPU已发出卡，继续写卡"));
    }
    //只有新卡才判断有效期
    if(!m_ProCardBasicInfo.bOldCard)
    {
        if ((curTime < m_ProCardBasicInfo.StartTime)||
                (m_ProCardBasicInfo.ExpireTime < curTime))
        {
            LogMsg("lane",tr("入口CPU过期卡"));
            ::showWarning(tr("过期卡"),true);
            return false;
        }
    }
    // 判断卡片在黑名单
    ExitTollItem *pExitToll = (ExitTollItem*)getLaneCtrl()->getTollItem();
    QCardListParamFile* pCardListTable = (QCardListParamFile*)getParamMgr()->GetParamFile(cfCardBList);
    QString sCardNet = QString::number(m_ProCardBasicInfo.bType);
    if(pCardListTable->IsBlackCard(m_ProCardBasicInfo.szCardNo,sCardNet,m_ProCardBasicInfo.bType,CardBType,CardBDesc))
    {
        LogMsg("lane",tr("原因:%1,卡号:%2,卡黑名单！").arg(CardBDesc).arg(m_ProCardBasicInfo.szCardNo));
        pExitToll->setExceptCard(true);
        pExitToll->setCardBlack(true);
    }
    return true;
}

bool Entrycardinfoprocessor::CompareVehProCard()
{
    QString strPrompt,tmpStr;
    bool bChecked;
    TollItem * pExit = (TollItem *)getLaneCtrl()->getTollItem();
    QSysParaDicParamFile* sysParam = (QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QSysParamInfo SysInfo = sysParam->GetSysParamInfo();
    strPrompt = "";
    bChecked = true;
    // 判断车型、车牌、车类不符
    if (m_ProCardBasicInfo.bVehClass > 0)
    {
        // 判断车型不符
        if (VehClass2Byte(pExit->m_VehInfo.VehClass) != m_ProCardBasicInfo.bVehClass)
        {
            strPrompt = "车型、";
            bChecked = false;
        }
        // 车牌绑定的云通卡必须输入全车牌，车牌长度>=8
        tmpStr = m_ProCardBasicInfo.szVehPlate;
        if ((!CompareVehPlate(pExit->m_VehInfo.szVehPlate,tmpStr.trimmed().toLocal8Bit().data()))||
                ((pExit->m_VehInfo.nVehPlateColor != -1) && (m_ProCardBasicInfo.bVehPlateColor!=0xFF)
                 &&(quint8)(pExit->m_VehInfo.nVehPlateColor)!=m_ProCardBasicInfo.bVehPlateColor))
        {
            if (SysInfo.m_bVLPCheck)
            {
                strPrompt.append("车牌、");
                bChecked = false;
            }
            else
            {
                LogMsg("lane",tr("车牌与卡内信息不符(卡内:%1%2)").arg(GetVehColorQString(m_ProCardBasicInfo.bVehPlateColor).arg(tmpStr)));
            }


        }
    }

    // 判断车类不符
    if (IsYTCard(m_ProCardBasicInfo))
    {
        // 公务卡、包缴卡使用卡内车类，输入无效，刷卡后车类变为卡内车类
        pExit->m_VehInfo.VehType = Byte2VehType(m_ProCardBasicInfo.bVehTollType);
        if (m_ProCardBasicInfo.bVehTollType == 0)
        {
            pExit->m_VehInfo.VehType = VT_Normal;
            //TODO    frameMain.SetVehType(GetVehTypeName(LaneCtrl.TollItem.m_VehInfo.VehType));
        }
        else if((CARD_TYPE_TALLY_CARD == m_ProCardBasicInfo.bType)||
                (CARD_TYPE_STORE_CARD == m_ProCardBasicInfo.bType))
        {
            if (pExit->m_VehInfo.VehType != VT_Normal)
            {
                strPrompt.append("车种");
                bChecked = false;
            }
        }
        if (!bChecked)
        {
            strPrompt.remove(strPrompt.length()-1,1);
            strPrompt.append("与卡内信息不符，请核实");
            ::showWarning(strPrompt);
            //入口车辆信息不符，提示并返回输入状态机
            getLaneCtrl()->DoLaneEvent(LET_EnVehInfoUn);
            return false;
        }
    }
    return true;
}

void Entrycardinfoprocessor::SetEntryInfo()
{
    TollItem * pExit =  (TollItem*)getLaneCtrl()->getTollItem();
    // 清空char数组类型，包括入口PSAM卡号、车牌号码，无信息字节均为0x00、标识站信息
    memset(m_CardTollInfo.baPsamTermId, 0,sizeof(m_CardTollInfo.baPsamTermId));
    memset(m_CardTollInfo.szVehPlate,0,sizeof(m_CardTollInfo.szVehPlate));
    memset(m_CardTollInfo.baFlagStaInfo,0,sizeof(m_CardTollInfo.baFlagStaInfo));
    // 入口路网号，路网号=5301
    m_CardTollInfo.wNetworkId = ORG_NETWORKID;
    // 入口收费站编码
    m_CardTollInfo.dwStaId =getLaneInfo()->getOrgInfo().sOrgCode.toUInt();
    // 入口车道
    m_CardTollInfo.bLaneId = getLaneInfo()->getLaneCode();
    // 通行状态，0-出口收回；1-封闭式入口发出；3-封闭式ETC入口发出；5-便携式入口发出；7-手持入口发出；9-无人值守入口发出
    m_CardTollInfo.bPassStatus = Tran_LaneType((LaneType)getLaneInfo()->getLaneType());//(LaneInfo.LaneType);
    if(getLaneInfo()->getShiftState() == lsRepare)//TODO
    {
        //维护发卡标识
        m_CardTollInfo.bMaintenance = 1;
    }
    m_CardTollInfo.PassTime = QDateTime::currentDateTime();       // 入口时间
    //保存入口时间 为业务时间
    pExit->SetOccurTime(m_CardTollInfo.PassTime);
    // 入口流水记录号
    m_CardTollInfo.bRecIndex = getLaneInfo()->ReadTransID();
    // 入口操作员号
    m_CardTollInfo.dwOpId = getLaneInfo()->getOper().dwOper;
    // 入口车型
    m_CardTollInfo.bVehClass = VehClass2Byte(pExit->m_VehInfo.VehClass);
    // 入口车类
    m_CardTollInfo.bVehTollType = VehType2Byte(pExit->m_VehInfo.VehType);
    // 入口班次
    m_CardTollInfo.bShiftId =getLaneInfo()->getShiftInfo().wShift;//LaneInfo.GetShiftInfo.ShiftNo;
//    //qDebug() << tr("入口班次") <<m_CardTollInfo.bShiftId ;
    //入口PSAM卡号在卡读写器单元中调用写入口信息时赋值
    //	m_cardTollInfo.baPsamTermId=1;							// 入口PSAM卡号
    // 卡片使用次数，写信息时累加
    m_CardTollInfo.dwWriteCount ++;
    //	卡箱逻辑编号
    //   TODO m_CardTollInfo.dwCardBoxId =;//LaneInfo.pDateFile.GetCardBoxInfo.CardBoxID;
    m_CardTollInfo.dwCardBoxId = getDataFileMgr()->getCardBoxInfo().CardBoxID;
    QString tmpVlp =  getLaneInfo()->getVehPlate();
    memcpy(m_CardTollInfo.szVehPlate,Utf8toGB2312(tmpVlp).data(),
           qMin(sizeof(m_CardTollInfo.szVehPlate),(quint32)Utf8toGB2312(tmpVlp).length()));
    //车牌颜色
    m_CardTollInfo.bVehPlateColor = pExit->m_VehInfo.nVehPlateColor;
    // 标识站个数，0-10
    m_CardTollInfo.bFlagStaCount = 0;
}

void Entrycardinfoprocessor::SaveEntryTollItem()
{
    TollItem * pExit = (TollItem*)getLaneCtrl()->getTollItem();
    pExit->setPhysicalCardType(m_PhysicalCardType);
    // 云通卡
    pExit->SetProCardInfo(m_ProCardBasicInfo);
    pExit->SetTollCardInfo(m_TollCardBasicInfo);
    pExit->SetCardTollInfo(m_CardTollInfo);
    pExit->setFdwBlance(m_dwBalance);
    if (m_PhysicalCardType == TYPE_PRO )
    {
        // 云通卡
        pExit->setCardType(m_ProCardBasicInfo.bType);
    }
    else if ((m_PhysicalCardType == TYPE_M1_S50_BLOCK) || ( m_PhysicalCardType = TYPE_M1_S50_MAD)
             || (m_PhysicalCardType == TYPE_M1_S70_BLOCK) || (m_PhysicalCardType == TYPE_M1_S70_MAD))
    {
        // 通行卡
        pExit->setCardType(m_TollCardBasicInfo.M1BasicInfo.bType);
    }
    else
    {
        pExit->setCardType(-1);
    }
}
bool Entrycardinfoprocessor::ProcessOpenCardEvent_TollCard(int nReaderId)
{
    QCardReader *reader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    QString sCardID;
    // 读卡
    if (!reader->ReadTollCard(m_TollCardBasicInfo, m_CardTollInfo))
    {
        ::showWarning(tr("读卡失败"),true);
        return false;
    }
    TollItem *pExit = (TollItem*)getLaneCtrl()->getTollItem();
    pExit->SendTollCardInfo(m_TollCardBasicInfo);

    // 判断卡类型
    if ( m_TollCardBasicInfo.M1BasicInfo.bType != CARD_TYPE_PASS_CARD)
    {
        ::showWarning(tr("非通行卡！"),true);
        return false;
    }
    //判断是否是本卡盒中卡
    if (m_CardTollInfo.dwCardBoxId !=getDataFileMgr()->getCardBoxInfo().CardBoxID)//LaneInfo.pDateFile.GetCardBoxInfo.CardBoxID)
    {
        ::showWarning(tr("非本卡盒中卡"),true);
        return false;
    }

    // 对通行卡判断入口标识
    if ( m_CardTollInfo.bPassStatus !=0)
    {
        ::showWarning(tr("该卡已发出！"),true);
        return false;
    }
    // 设置写卡信息
    SetEntryInfo();
    sCardID =QString::number(m_TollCardBasicInfo.M1BasicInfo.dwCardNum);// m_TollCardBasicInfo.M1BasicInfo.dwCardNum;
    if (!reader->WriteEntryInfo(sCardID, m_CardTollInfo))
    {
        //发入口坏卡报文
        getLaneCtrl()->DoLaneEvent(LET_BadCard);
        ::showWarning(tr("写卡失败！"),true);
        return false;
    }
    LogMsg("lane",tr("通行卡写入口信息成功，卡号%1").arg(m_TollCardBasicInfo.M1BasicInfo.dwCardNum));
    //发卡时，卡盒中卡数减1
    getDataFileMgr()->DeleteOneCard();
    //判断卡盒用尽
    if(getDataFileMgr()->getCardBoxInfo().nCardCount <= 0)
    {
        ::showWarning(tr("通行卡已发完，请下班更换卡盒"),true);
    }
    //保存写卡信息
    SaveEntryTollItem();
    return true;
}

bool Entrycardinfoprocessor::WriteProCard()
{
    // 写卡
    // 判断上一辆车处理完成
    if(!getLaneCtrl()->CanDoNextCar())
    {
        ::showWarning(tr("前车尚未离开,暂不处理"));
        return false;
    }
    SetEntryInfo();
    QString sCardId = m_ProCardBasicInfo.szCardNo;
    QCardReader* reader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    if(!reader->WriteEntryInfo(sCardId,m_CardTollInfo))
    {
        ::showWarning(tr("写卡失败！"),true);
        return false;
    }
    LogMsg("lane",tr("云通卡写入信息成功，物理卡号%1卡号%2").arg(m_ProCardBasicInfo.dwCardId).arg(m_ProCardBasicInfo.szCardNo));
    return true;
}

Entrycardinfoprocessor* getEntryCardProcessor()
{
    static Entrycardinfoprocessor g_processor;
    return &g_processor;
}
