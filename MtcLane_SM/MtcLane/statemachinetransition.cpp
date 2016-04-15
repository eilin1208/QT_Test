#include "statemachinetransition.h"
#include "formtablewidget.h"
#include "formmainwindow.h"
#include "ParamModule/qshiftparamfile.h"
#include "ParamModule/qparammgr.h"
#include "laneinfo.h"
#include "formmessage.h"
#include "datafilemgr.h"
#include "formtablewidget.h"
#include "forminformid.h"
#include "formentryplate.h"
#include "formentryvehinfo.h"
#include "MtcKey/MtcLaneState.h"
#include "devicefactory.h"
#include "formbase.h"
#include "formpapercard.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "formrecordtable.h"
#include "formmtctitle.h"
#include "datafilemgr.h"
#include "lanectrl.h"
#include "formamendweight.h"
#include "formwtinfo.h"
#include "entrytollitem.h"
#include "exittollitem.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qsysparadicparamfile.h"
#include "formauth.h"
#include "dialogshowproinfo.h"
#include "cardinfoprocessor.h"
#include "formcardbox.h"
#include "formvehquebegin.h"
#include "shiftsum.h"
#include "formdevicestate.h"
#include "formbottombar.h"
#include "formfunction.h"
#include "formwtdetail.h"
#include "formloadparam.h"
#include "formvpr.h"
#include "formexit.h"
#include "formentry.h"
#include "formlog.h"
#include "formpasswordverify.h"
#include "entrycardinfoprocessor.h"
#include "formautocard.h"
#include "formautovehinfo.h"
#include "ParamModule/qholidayparamfile.h"
#include "formregulatetime.h"
void TranConfirmShift::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //将用户选择的工班信息保存
    FormTableWidget* pTable = (FormTableWidget*)((MtcLaneState*)sourceState())->GetWindowPtr();
    quint16 wShiftNo =  pTable->currentValue().toUInt();
    LogMsg("lane", tr("操作员选择班次，班次代码为：[%1]..............").arg(wShiftNo));
    QShiftParamFile* pShiftFile = (QShiftParamFile*)getParamMgr()->GetParamFile(cfShift);
    SShiftValue shift = pShiftFile->getShiftInfoByNo(wShiftNo);
    LogMsg("lane", tr("操作员信息为[%1],[%2],[%3],[%4],[%5],[%6],[%7]").arg(shift.sStation)
           .arg(shift.sBLNetID).arg(shift.szBLRoadID).arg(shift.wShift).arg(shift.sShiftName)
           .arg(shift.wTimeDiff).arg(shift.sLastVer));
    //保存工班信息
    getLaneInfo()->setShiftInfo(shift);
    //保存登陆时间
    getLaneInfo()->setLogTime(QDateTime::currentDateTime());
    //设置目标状态的提示信息
    FormMessage * pMsg = (FormMessage*)((MtcLaneState*)targetState())->GetWindowPtr();
    //获取票号
    SInvInfo invInfo = getDataFileMgr()->GetInvInfo();
    pMsg->setTitle("票据确认");
    pMsg->setMessage(tr("当前票据号码：%1").arg(invInfo.InvBeginNo, 12, 10, QLatin1Char('0')));
    //    ::showWarning(tr("请核对票号，如有错误，请按【票号】键修改"),true);
    getDataFileMgr()->UpdateWidgetData();
    //    ::showWarning(tr("请核对票号，如有错误，请按【票号】键修改"),true);
}

void TranConfirmShift::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
}

void TranShowInvoiceMenu::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget* pTable = (FormTableWidget*)getTargetWidget();
    pTable->clearData();
    pTable->setTitle(tr("票号菜单"));
    pTable->insertItem(tr("换上票据"), 1);
    pTable->insertItem(tr("修改票据"), 2, getDataFileMgr()->LoadInvFromFile());
    ::showWarning(tr("选择号码后按【确认】键"),true);
    //发送更换票据报文
    getLaneCtrl()->DoLaneEvent(LET_InvMod);
}

void TranShowInvoiceMenu::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyInvoice)
    {
        *keyType = KC_Func;
    }
}
void TranRtInvConfirm::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyEsc)
    {
        *keyType = KC_Func;
    }
}
void TranChangeUpInvoice::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable1 = (FormTableWidget*)getSourceWidget();
    if(pTable1->currentValue().toInt()==1&&  keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        getInformWidget()->setInfoTitle1();
        *keyType = KC_Func;
    }
}

void TranModifyInvoice::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable2 = (FormTableWidget*)getSourceWidget();
    if(pTable2->currentValue().toInt()==2&& keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm)
    {
        getInformWidget()->setInfoTitle2();
        *keyType=KC_Func;
    }

}
void TranTurnOnCanLight::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getDeviceFactory()->getIOBoard()->SetCanopy(1);
    //设置字符叠加
    getDeviceFactory()->getVDM()->UpdateDateTime();
    getDeviceFactory()->getVDM()->UpdateCollector(getLaneInfo()->getOper().dwOper, false);
    getDeviceFactory()->getVDM()->UpdateLane("", getLaneInfo()->getLaneCode());
    //设置上班时间
    getLaneInfo()->setLogTime(QDateTime::currentDateTime());
    //上班流水号
    getLaneInfo()->setLogWaste(getLaneInfo()->ReadTransID());
    //发送上班报文
    getLaneCtrl()->getTollItem()->SetOccurTime(getLaneInfo()->getLogTime());
    getLaneCtrl()->getTollItem()->SendLoginMsg();
    //显示本次上班信息
    FormMTCTitle *pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);

    //显示操作员名称
    pTitle->setOperName(getLaneInfo()->getOper().sOperName);

    //显示工班信息
    pTitle->setShift(getLaneInfo()->getShiftInfo().sShiftName, getLaneInfo()->getLogTime());
    pTitle->setWorkState(lsNormalWorking);
    getLaneInfo()->setShiftState(lsNormalWorking);

    FormBottomBar* pBottom = NULL;
    getWidgetByType(emFormBottomBar, (QWidget**)&pBottom);
    pBottom->showOperNo(getLaneInfo()->getOper().dwOper);

    //自动发卡软件设置按钮可用
    if(getLaneInfo()->isAutoLane())
    {
        ((FormAutoCard*)getOrdinaryWidget())->showLogin(true);
    }
}

void TranTurnOnCanLight::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyCanopyLight)
    {
        *keyType = KC_Func;
    }
}
//更改票据用//可以注掉，没用了
void TranEnRepBill::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        MtcLaneState* state = (MtcLaneState*)sourceState();
        ForminformId* pWidget = (ForminformId*)state->GetWindowPtr();
        QString sError;
        if(!pWidget->IsEnterInvFinished(sError))
        {
            ::showWarning(sError);
            return;
        }
        else
        {
            *keyType = KC_Func;
        }
    }
    if(keyPropertity.isFuncKey() && keyPropertity.func ==KeyEsc)
    {
        ::showWarning(tr("更改数据中，不能取消！"),true);
    }
}

void TranEnRepBill::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

}

void TranFinInvoice::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //保存票号
    ForminformId* pWidget = static_cast<ForminformId *>(getSourceWidget());
    getDataFileMgr()->UpInvoice(pWidget->getNewInvoiceNo(),pWidget->getNewInvoiceNo()+ 100000, 100000 );

    //界面显示
    FormMessage * pMsg = static_cast<FormMessage*>(getTargetWidget());
    pMsg->setTitle("票据确认");
    pMsg->setMessage(tr("当前票据号码：%1").arg(pWidget->getNewInvoiceNo(), 12, 10, QLatin1Char('0')));
    //显示票号
    getDataFileMgr()->UpdateWidgetData();

    //清除用户输入信息
    pWidget->clearData();
}

void TranFinInvoice::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{

    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        MtcLaneState* state = (MtcLaneState*)sourceState();
        ForminformId* pWidget = (ForminformId*)state->GetWindowPtr();
        QString sError;
        if(!pWidget->IsEnterInvFinished(sError))
        {
            ::showWarning(sError);
            return;
        }
        else
        {
            *keyType = KC_Func;
        }
    }
}

void TranFinEnterPlate::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey()
            && (keyPropertity.func == KeyConfirm || keyPropertity.func == KeyEsc))
    {
        *keyType = KC_Func;
    }
}

void TranFinEnterPlate::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

}

void TranEnterPlate::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormEntryVehInfo* vehInfo = static_cast<FormEntryVehInfo*>(getSourceWidget());
    if(vehInfo->hasEnterVehType() && keyPropertity.isFuncKey() && keyPropertity.func == KeyEditPlate)
    {
        //liujian
        //页面显示
        getSourceWidget()->hide();

        int nRes = getShowEnPlate()->doModalShow();

        //getOrdinaryWidget()->showEntryInfo();
        if(nRes == 0)
        {
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            getShowEnPlate()->ResetShow();
            *keyType = KC_Func;
            return;
        }
        if(nRes == 1)
        {
            //FormEntryVehInfo* pVehInfo = (FormEntryVehInfo*)getSourceWidget();
            //qDebug()<<"dedao2:"<<getLaneInfo()->getVehPlate();
            vehInfo->setPlateInfo(getLaneInfo()->getVehPlateColor(), getLaneInfo()->getVehPlate());
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            //liujian 修改车牌报文
            //设置报文
            memset(getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate,0,sizeof(getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate));
            memcpy(getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate,
                   Utf8toGB2312(getLaneInfo()->getVehPlate()).data(),
                   qMin((quint32)Utf8toGB2312(getLaneInfo()->getVehPlate()).length(),
                        sizeof(getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate)));
            getLaneCtrl()->getTollItem()->m_VehInfo.nVehPlateColor = getLaneInfo()->getVehPlateColor();
            getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VLP);
            //qDebug() << "edit plate2" << getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate;
            getShowEnPlate()->ResetShow();
            *keyType = KC_Func;
        }
    }
}

void TranEnterPlate::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //qDebug() << "edit plate1" << getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate;
}


void TranNoCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    //判断是否填写完整,是否已经输入了车型车牌车类
    FormEntryVehInfo* pVehInfo = static_cast<FormEntryVehInfo*>(getSourceWidget());
    if(!pVehInfo->hasFinishVehInfo())
    {
        return;
    }
    //判断是否为无卡键
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyNoCard)
    {
        *keyType = KC_Func;
    }
}

void TranNoCard::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //getLaneCtrl()->DoLaneEvent(LET_NoCard);
    //这里获取到目标界面，加载要显示的内容
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->setTitle(tr("无卡处理"));
    pTable->clearData();
    pTable->setCol(1);
    pTable->insertItem(tr("无卡选站"));
    pTable->insertItem(tr("无卡最大里程"),1);
    getOrdinaryWidget()->hideEntryInfo();
    //填装报文信息
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //liujian 10-无卡 11-无卡最大里程
    pExit->setNoCard(0);
    pExit->getVehEntryInfo().bEnVLPC = -1;
    //getLaneInfo()->getVehEntryInfo()->bEnVLPC = -1;
    //发送车道事件报文
    getLaneCtrl()->DoLaneEvent(LET_NoCard);
}

void TranChooseCenter::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget * pTable =  static_cast<FormTableWidget*>(getSourceWidget());
    int nIndex = pTable->currentIndex();
    //当选择第一项，也就是选择入口收费站的时候，进行此跳转
    if(nIndex == 0)
    {
        if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
        {
            *keyType = KC_Func;
        }
        if(keyPropertity.isNumKey())
        {
            int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
            if(nNum == nIndex)
            {
                *keyType = KC_Func;
            }
        }
    }
}

void TranChooseCenter::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //这里在目标窗口中加载中心名称,中心从QOrgCodeParamFile类中获取
    FormTableWidget * pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("选择入口路段"));
    pTable->setCol(2);
    pTable->setRow(5);
    QOrgCodeParamFile* pFile =(QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    QVector<SOrgCodeValue> centerList;
    pFile->getOrgList(ORG_TYPE_ROAD, 0, centerList);
    foreach(SOrgCodeValue value, centerList)
    {
        pTable->insertItem(value.sOrgSName, value.sOrgCode);
    }
    getOrdinaryWidget()->hideEntryInfo();

    //填装报文信息
    //getLaneInfo()->getVehEntryInfo()->nEntryType = Entry_ByManual;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->getVehEntryInfo().nEntryType = Entry_ByManual;
    pExit->setNoCard(10);

    //清除卡信息
    pExit->ClearCardInfo();
}

void TranChooseSta::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm && pTable->currentEnable())
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
        if(nNum == -1)
        {
            nNum = 9;
        }
        if(nNum == pTable->currentIndex() && pTable->currentEnable())
        {
            *keyType = KC_Number;
        }
    }
}

void TranChooseSta::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //这里根据原窗口中选择的中心编号，获取该中心下的收费站名,并显示在目标界面上
    FormTableWidget *pSource = static_cast<FormTableWidget*>(getSourceWidget());
    quint32 dwRoad = pSource->currentValue().toUInt();
    FormTableWidget * pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("选择入口收费站"));
    pTable->setRow(5);

    QOrgCodeParamFile* pFile =(QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    QVector<SOrgCodeValue> centerList;

    pFile->getOrgList(ORG_TYPE_STATION, dwRoad, centerList);
    if(centerList.count() > 5)
    {
        pTable->setCol(2);
    }
    else
    {
        pTable->setCol(1);
    }
    SAllRateValue tmpValue;
    QAllRateParamFile* pAllRate = (QAllRateParamFile*)getParamMgr()->GetParamFile(cfAllRate);
    bool nRlt = false;
    foreach(SOrgCodeValue value, centerList)
    {
        nRlt = pAllRate->queryTollValue(tmpValue, getLaneInfo()->getOrgInfo().sOrgCode.toInt(),
                                        value.sOrgCode.toInt());
        pTable->insertItem(value.sOrgSName, value.sOrgCode, nRlt);
    }
    getOrdinaryWidget()->hideEntryInfo();
}

void TranShowFeeByConfirm::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm&& pTable->currentEnable())
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
        if(nNum == -1)
        {
            nNum = 9;
        }
        if(nNum == pTable->currentIndex() && pTable->currentEnable())
        {
            *keyType = KC_Number;
        }
    }
}

void TranShowFeeByConfirm::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //获取入口站编号
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    quint32 dwOrgCode = pTable->currentValue().toUInt();
    //getLaneInfo()->setOrgCode(QString::number(dwOrgCode));
    //从通行卡中获取入口信息，此次模拟实现
    //    getOrdinaryWidget()->setEntryInfo(QDateTime::currentDateTime(), tr("鲁A12345"), tr("港沟"), "00100000231", "00100001", QDateTime::currentDateTime(), "");
    //计算通行费,此处模拟实现
    //    int nFee = 500;
    //    getLaneInfo()->setMoney(nFee);
    //    FormEntryVehInfo* pVehInfo = static_cast<FormEntryVehInfo*>(getTargetWidget());
    //    pVehInfo->showResultMode(true);
    //    pVehInfo->setTollFee(getLaneInfo()->getFinalMoney());
    //    getOrdinaryWidget()->showEntryInfo();
    //填装报文信息
    //getLaneInfo()->getVehEntryInfo()->dwQryStationID = dwOrgCode;
    //getLaneInfo()->getVehEntryInfo()->dwEnStationID = dwOrgCode;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->getVehEntryInfo().dwQryStationID = dwOrgCode;
    pExit->getVehEntryInfo().dwEnStationID = dwOrgCode;

    //字符集转码
    strcpy(pExit->getVehEntryInfo().szEnStationName,
           Utf8toGB2312(pTable->currentText()).data());
    pExit->getVehEntryInfo().bEnVLPC = -1;

}

void TranNoCardMaxDis::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget * pTable =  static_cast<FormTableWidget*>(getSourceWidget());
    int nIndex = pTable->currentIndex();
    //当选择第一项，也就是选择入口收费站的时候，进行此跳转
    if(nIndex == 1)
    {
        if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
        {
            *keyType = KC_Func;
        }
        if(keyPropertity.isNumKey())
        {
            int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
            if(nNum == nIndex)
            {
                *keyType = KC_Func;
            }
        }
    }
}

void TranNoCardMaxDis::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //填装报文信息
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->setNoCard(11);
    pExit->ClearCardInfo();

}

void TranYunTongCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    //按键判断
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyOtherPayMethod)
    {
        ::showWarning(tr("其他付款键无效!"), false, FormWarning::WARNING);
        return;
    }
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyStoreCard)
    {
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        if(!getLaneCtrl()->CanDoNextCar())
        {
            ::showWarning(tr("前车未离开，无法操作!"), false, FormWarning::WARNING);
            return;
        }
        if(TYPE_PRO != getLaneCtrl()->getTollItem()->getPhysicalCardType())
        {
            ::showWarning(tr("入口未刷云通卡，不能使用云通卡支付！"), false, FormWarning::WARNING);
            return;
        }

        if(pExit->getExceptCard())
        {
            ::showWarning(tr("车型信息不符，卡内为【%1】，输入为【%2】，请收取现金")
                          .arg(GetVehClassName((CVehClass)pExit->getProCardBasicInfo().bVehClass))
                          .arg(GetVehClassName((CVehClass)getLaneInfo()->getVehClass())), false, FormWarning::WARNING);
            return;
        }

        //1、储值卡（普通用户、区间包交用户）弹出云通卡支付界面
        //1.1储值卡余额>=5时，弹出云通卡支付界面，先扣款，
        //余额充足，一次支付完成后，放行；
        //储值卡中余额不足时需要第二次支付，收取现金打印票据后放行。
        //1.2 储值卡余额<5元时，直接使用现金支付，收取现金打印票据后放行。
        //2、记账卡、储值卡全免用户 直接保存卡信息 并放行
        if(IsYTCard(pExit->getProCardBasicInfo()) &&
                pExit->getProCardBasicInfo().bUserType != CARD_UserType_FullFree)
        {
            //卡类型为 储值卡，用户类型不等于 全免用户
            int nRet = StoreConsume();
            if(nRet == 1)
            {
                *keyType = KC_Func;
                return;
            }
            else
            {
                if(nRet == 2)
                    return;
                if(nRet == 0)
                {
                    //TODO:显示扣款后的剩余金额
                    //TODO:发送通行费用到监控
                    pExit->SendMoneyInfo(*getLaneInfo()->getTollMoneyInfo(), pExit->getCalculateMoneyInfo());
                    //显示收费金额
                    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getSourceWidget());
                    pVeh->showResultMode(true);
                    ::showWarning(tr("请收取通行费，并选择付款方式!"), false, FormWarning::MESSAGE);
                    //应收金额
                    pVeh->setTollFee(getLaneInfo()->getTollMoneyInfo()->nCashMoney / 100,
                                     getLaneInfo()->getTollMoneyInfo()->nCashCardMoney / 100);
                    //免收金额
                    pVeh->setTollFreeFee(pExit->getFareQryResult().m_dwDiscountedFree / 100);
                    //费显显示
                    int nMoney = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
                    getDeviceFactory()->getFD()->ShowFare(nMoney * 100);
                }
                return;
            }

        }
        else if(pExit->getProCardBasicInfo().bType == CARD_TYPE_TALLY_CARD ||
                (/*pExit->getProCardBasicInfo().bType == CARD_TYPE_STORE_CARD &&pExit->getProCardBasicInfo().bUserType == CARD_UserType_FullFree*/
                 pExit->getProCardBasicInfo().bType == CARD_TYPE_ALLFREE_CARD ||
                 pExit->getProCardBasicInfo().bType == CARD_TYPE_ALL_BAOJIAO_CARD))
        {
            //旧记账卡只有全路网免费用户
            //对于区间免费用户认为是非法卡无法使用
            if(pExit->getProCardBasicInfo().bOldCard &&
                    pExit->getProCardBasicInfo().bType == CARD_TYPE_TALLY_CARD)
            {
                if(pExit->getProCardBasicInfo().bUserType != CARD_UserType_FullFree)
                {
                    ::showWarning(tr("无效区间包交卡，无法使用！"), false, FormWarning::WARNING);
                    return;
                }
            }
            //界面确认
            getSourceWidget()->hide();
            getOrdinaryWidget()->hideEntryInfo();
            int nRlt = getShowProInfo()->doModalShow();
            getOrdinaryWidget()->showEntryInfo();
            getSourceWidget()->show();
            if(nRlt == 0)
            {
                return;
            }
            //费显显示
            if(pExit->getProCardBasicInfo().bType == CARD_TYPE_ALLFREE_CARD||
                    pExit->getProCardBasicInfo().bType == CARD_TYPE_ALL_BAOJIAO_CARD)
            {
                getDeviceFactory()->getFD()->ShowBalanceAfterDeduct(0, pExit->getBalance());
            }
            else
            {
                getDeviceFactory()->getFD()->ShowBalanceAfterDeduct(0, -1, true);
            }
            pExit->setPayType(PYT_Free);

            getLaneCtrl()->Complete(QDateTime::currentDateTime(), true);
            //放行
            ExitConfirmComplete();
            *keyType = KC_Func;
        }
        //liujian清除免费区间信息
        pExit->getProCardBasicInfo().clear();
    }
}

void TranYunTongCard::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //TODO:进行云通卡扣款处理，包括写卡，报文发送，抬杆等操作
    //清除页面显示
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    getOrdinaryWidget()->clearInfo();
    //表格显示

    //过车数量、卡数量增加
    //    getLaneInfo()->setCardCount(getLaneInfo()->getCardCount()+1);
    //    getLaneInfo()->setVehCount(getLaneInfo()->getVehCount()+1);

    //清除laneinfo保存
    getLaneInfo()->clearCurVehInfo();
    getLaneInfo()->addAllowPass();
    //抬杆操作
    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    //lane info / ctrl
}
int TranYunTongCard::StoreConsume()
{
    ExitTollItem * pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //使用云通储值卡的免费车种可以直接放行
    if(getLaneInfo()->getTollMoneyInfo()->nCashMoney == 0)
    {
        //改写入出口标志
        if(!CardInfoProcessor::ClearProCardState())
        {
            return -1;
        }
        //支付类型  99 免费
        pExit->setPayType(PYT_Free);
        getLaneCtrl()->Complete(QDateTime::currentDateTime(), true);
        ExitConfirmComplete();
        return 1;
    }

    //判断卡内余额 不足5元时，无法扣款只能使用现金支付
    //这是需要首先清除支付卡内入口标识
    if(pExit->getBalance() < 500)
    {
        if(!CardInfoProcessor::ClearProCardState())
        {
            return -1;
        }

        //提示余额不足，使用现金支付
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle(tr(""));
        getMsgWidget()->setMessage(tr("云通卡余额不足\n请收取现金！"));
        getMsgWidget()->doModalShow();
        getOrdinaryWidget()->show();
        getSourceWidget()->setFocus();
        getLaneInfo()->setSubPay(true);
        return 0;
    }
    //余额大于等于5 可以弹出云通卡支付界面 先扣款  没有扣完的部分使用现金支付
    //总通行费
    double tollMoney = getLaneInfo()->getTollMoneyInfo()->nCashCardMoney +
            getLaneInfo()->getTollMoneyInfo()->nCashMoney;
    //页面显示
    getSourceWidget()->hide();
    getOrdinaryWidget()->hideEntryInfo();
    DialogShowProInfo* proInfo = getShowProInfo();
    int nRes = proInfo->doModalShow();
    getOrdinaryWidget()->showEntryInfo();
    getSourceWidget()->show();
    getSourceWidget()->setFocus();
    //支付失败
    if(nRes == -1)
    {
        return -1;
    }
    if(nRes == 0)
    {
        return 2;
    }
    else
    {
        TTollMoneyInfo tollMoneyInfo;
        tollMoneyInfo.nCashMoney = getLaneInfo()->getLastMoneyInfo()->nLastMoney;
        tollMoneyInfo.nCashCardMoney = getLaneInfo()->getLastMoneyInfo()->nLastCardMoney;
        pExit->SetMoneyInfo(tollMoneyInfo, *getLaneInfo()->getLastMoneyInfo());
        if(getLaneInfo()->getSubPay())
        {
            //设置并保存本次支付的应收通行信息（两次支付时需要把 总的应收通行费分开 ）
            TCalculateMoneyInfo calcMoney;
            memset(&calcMoney, 0, sizeof(calcMoney));
            calcMoney.nTruckBasicMoney = pExit->getCalculateMoneyInfo().nTruckBasicMoney;
            //本次应收金额等于 本次支付金额+免费金额+折扣金额
            calcMoney.nTollMoney = getLaneInfo()->getLastMoneyInfo()->nLastMoney+
                    pExit->getCalculateMoneyInfo().nFreeMoney+
                    pExit->getCalculateMoneyInfo().nRebateMoney;
            calcMoney.nCardCost = pExit->getCalculateMoneyInfo().nCardCost;
            calcMoney.nOverWeightMoney = pExit->getCalculateMoneyInfo().nOverWeightMoney;
            calcMoney.nFreeMoney = pExit->getCalculateMoneyInfo().nFreeMoney;
            calcMoney.nDisCountTollMoney = pExit->getCalculateMoneyInfo().nDisCountTollMoney;
            calcMoney.nRebateMoney = pExit->getCalculateMoneyInfo().nRebateMoney;
            pExit->SetCalculateMoneyInfo(calcMoney);
            //分段支付时  第一次支付的业务类型为 分段支付
            pExit->setYWType(LST_TYPE_SUBPAY);
            //保存业务流水
            getLaneCtrl()->Complete(QDateTime::currentDateTime(), !getLaneInfo()->getSubPay());

            //发送业务流水
            pExit->SendYewuWasteMsg(0);
            //设置第二次支付的应收通行费信息
            //免费、折扣以及超限加收金额全部跟随第一笔支付发送后台
            //第二次支付时，只有应收通行费
            memset(&calcMoney,0, sizeof(calcMoney));
            calcMoney.nTollMoney = getLaneInfo()->getTollMoneyInfo()->nCashMoney;
            calcMoney.nCardCost = getLaneInfo()->getTollMoneyInfo()->nCashCardMoney;
            return 0;
        }
        else
        {
            pExit->SetMoneyInfo(*getLaneInfo()->getTollMoneyInfo(),
                                *getLaneInfo()->getLastMoneyInfo());
            pExit->setYWType(LST_TYPE_NORMAL);
            getLaneCtrl()->Complete(QDateTime::currentDateTime(), !getLaneInfo()->getSubPay());
            //
            FormRecordTable* pRecord = NULL;
            getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
            int cardType = pExit->getPhysicalCardType();
            QString cardNum;
            if(cardType == TYPE_PRO)
            {
                cardNum = QString::number(QString::fromLocal8Bit(pExit->getProCardBasicInfo().szCardNo,16).toInt());
            }
            else
            {
                cardNum = QString::number(pExit->getTollBasicInfo().M1BasicInfo.dwCardNum);
            }
            QString vehType = GetVehClassName((CVehClass)(getLaneInfo()->getVehClass()));
            QString vehPlate = getLaneInfo()->getVehPlate();
            QString vehEnStaName = QString::number(pExit->getVehEntryInfo().dwEnStationID);
            //            pRecord->insertExitInfo(QDateTime::currentDateTime(),cardNum,vehType,vehPlate,vehEnStaName,
            //                                    (pExit->getTollMoney().nCashMoney+pExit->getTollMoney().nCashCardMoney)/100.0,
            //                                    (pExit->getLastMoney().nLastMoney+pExit->getLastMoney().nLastCardMoney)/100.0,
            //                                    pExit->getBalance()/100.0);
            pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
            pExit->ClearSavedInfo();
            ExitConfirmComplete();
        }
        return 1;
    }
}

void TranYunTongCard::ExitConfirmComplete()
{
    getLaneCtrl()->ConfirmComplete();
    //TODO:切换状态
}

void TranCashHandle::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyOtherPayMethod)
    {
        ::showWarning(tr("其他付款键无效!"), false, FormWarning::WARNING);
        return;
    }
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyCash)
    {
        //qDebug() << "****************tran handle cash ***********1";
        //判读是否有发票
        if(getDataFileMgr()->GetInvInfo().InvCount == 0)
        {
            ::showWarning(tr("请换上票据!"));
            return;
        }
        //判断前车是否离开
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        memset(&m_invPrtInfo, 0, sizeof(TInvPrintInfo));
        if(!getLaneCtrl()->CanDoNextCar())
        {
            ::showWarning(tr("前车未离开，无法操作!"), false, FormWarning::WARNING);
            return;
        }
        //读通行卡成功后，改写入出口标志
        if(getLaneCtrl()->getTollItem()->getCardType() == CARD_TYPE_PASS_CARD &&
                pExit->getVehEntryInfo().nEntryType == Entry_ByCard)
        {
            if(!CardInfoProcessor::ClearPassCardState())
            {
                return;
            }
        }

        else if(pExit->getPhysicalCardType() == TYPE_PRO &&
                pExit->getProCardBasicInfo().bType != 0xFF)
        {
            if(!getLaneInfo()->getSubPay())
            {
                //TODO:确认是否放弃云通卡交费，使用现金交费
                //                if(pExit->getExceptCard())
                //                {
                getOrdinaryWidget()->hide();
                getMsgWidget()->setTitle(tr(""));
                getMsgWidget()->setMessage(tr("是否放弃使用云通卡缴费\n并完成实际收费！"));
                int ret = getMsgWidget()->doModalShow();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                if(ret == 0)
                {
                    return;
                }
                else if(!CardInfoProcessor::ClearProCardState())
                {
                    return;
                }
                *keyType = KC_Func;
            }
            //            }
        }
        //qDebug() << "****************tran handle cash ***********2";
        getLaneInfo()->getLastMoneyInfo()->nPaySn++;
        getLaneInfo()->getLastMoneyInfo()->nLastMoney = getLaneInfo()->getTollMoneyInfo()->nCashMoney;
        getLaneInfo()->getLastMoneyInfo()->nLastCardMoney =
                getLaneInfo()->getTollMoneyInfo()->nCashCardMoney;
        pExit->setYWType(LST_TYPE_NORMAL);
        if(getLaneInfo()->getSubPay())
        {
            //分段支付现金支付部分的处理 保存支付通行费信息
            TTollMoneyInfo tollMoneyInfo;
            tollMoneyInfo.nCashMoney = getLaneInfo()->getLastMoneyInfo()->nLastMoney;
            tollMoneyInfo.nCashCardMoney =
                    getLaneInfo()->getLastMoneyInfo()->nLastCardMoney;
            pExit->SetMoneyInfo(tollMoneyInfo, *getLaneInfo()->getLastMoneyInfo());
            getLaneInfo()->setSubPay(false);
        }
        else
        {
            pExit->SetMoneyInfo(*getLaneInfo()->getTollMoneyInfo(),
                                *getLaneInfo()->getLastMoneyInfo());
        }
        pExit->setPayType(PYT_Cash);
        //
        //qDebug() << "****************tran handle cash ***********3";
        //判断是否需要打印，收取0元时不需要打印
        if(pExit->getLastMoney().nLastMoney == 0)
        {
            pExit->SetInvPrintInfo(m_invPrtInfo);
            *keyType = KC_Func;
            m_bPrint = false;
        }
        else
        {
            LogMsg("lane", tr("收取现金通行费：%1,卡成本金额：%2")
                   .arg(getLaneInfo()->getLastMoneyInfo()->nLastMoney / 100)
                   .arg(getLaneInfo()->getLastMoneyInfo()->nLastCardMoney / 100));
            *keyType = KC_Func;
            m_bPrint = true;
        }
        QTimer::singleShot(50, this , SLOT(timeOutToPrint()));
    }
}

void TranCashHandle::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //TODO:进行云通卡扣款处理，包括写卡，报文发送，抬杆等操作
    //清除页面显示
    //    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    //    pVeh->showResultMode(false);
    //    pVeh->clearData();
    //qDebug() << "********************** handle cash on transition";
    getLaneInfo()->addAllowPass();
}



bool TranCashHandle::PrintInvoiceInfo()
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    PrinterArm* pPrint = getDeviceFactory()->getPrinter();
    //qDebug() << "****************tran handle cash ***********4";
    //增加票据号
    //无论打印是否成功，票号都要增加一
    //数量减一
    if(!getDataFileMgr()->IncInvoice())
    {
        return false;
    }
    memset(&pPrint->m_NoteContent, 0, sizeof(pPrint->m_NoteContent));

    pPrint->GetPrintStatus();

    QOrgCodeParamFile* pOrg = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue orgInfo;
    pOrg->queryOrgCodeValue(orgInfo, pExit->getVehEntryInfo().dwEnStationID, ORG_TYPE_STATION);
    QString sTmpStr;
    sTmpStr = GetVehClassName_LPCSTR(pExit->m_VehInfo.VehClass);
    qstrncpy(pPrint->m_NoteContent.VehClass, Utf8toGB2312(sTmpStr).data(), 6);
    if(IsCar(VehClass2Byte(pExit->m_VehInfo.VehClass)) || !getLaneInfo()->getUseWeight())
    {
        pPrint->m_NoteContent.iFeeType = 0;
    }
    else if(getLaneInfo()->getUseWeight() && IsTruck(VehClass2Byte(pExit->m_VehInfo.VehClass)))
    {
        pPrint->m_NoteContent.iFeeType = 1;
    }

    //小写金额
    pPrint->m_NoteContent.Fare_Num = pExit->getLastMoney().nLastMoney;
    //大写金额
    //时间
    qstrncpy(pPrint->m_NoteContent.szDateTime,
             QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),
             32);
    //入口站

    qstrncpy(pPrint->m_NoteContent.EnStationName,
             Utf8toGB2312(orgInfo.sOrgSName).data(),
             32);
    //出口站
    qstrncpy(pPrint->m_NoteContent.ExStationName,
             Utf8toGB2312(getLaneInfo()->getOrgInfo().sOrgSName).data(),
             32);
    //收费员代码
    qstrncpy(pPrint->m_NoteContent.CollectorID,
             QString::number(getLaneInfo()->getOper().dwOper).toLocal8Bit().data(),
             9);
    if(pPrint->m_NoteContent.iFeeType == 1)
    {
        //总重
        qstrncpy(pPrint->m_NoteContent.weight,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetWeight()/1000.00).toLocal8Bit().data(),
                 10);
        //限载
        qstrncpy(pPrint->m_NoteContent.LimitW,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetLimitWeight()/1000.00).toLocal8Bit().data(),
                 10);
        //超限率
        qstrncpy(pPrint->m_NoteContent.OverP,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetOverloadRate()/1000.00).toLocal8Bit().data(),
                 10);
    }
    if(getLaneInfo()->getShiftState() == lsRepare)
    {
        pPrint->m_NoteContent.sMark = 1;
    }
    else if(getLaneInfo()->getShiftState() == lsNormalWorking)
    {
        pPrint->m_NoteContent.sMark = 0;
    }
    //qDebug() << "****************tran handle cash ***********5";
    if(!pPrint->Print())
    {
        LogMsg("lane", tr("打印失败"));
        ::showWarning(tr("打印失败"), false, FormWarning::WARNING);
        return false;
    }
    else
    {
        LogMsg("lane", tr("打印成功"));
        ::showWarning(tr("打印成功"), false, FormWarning::WARNING);
        return true;
    }
}

void TranCashHandle::timeOutToPrint()
{
    if(m_bPrint)
    {
        bool ret = PrintInvoiceInfo();
        //qDebug() << "****************tran handle cash ***********6" << ret;
        if(ret)
        {
            emit printSuccess();
        }
        else
        {
            emit printFail();
        }
    }
    else
    {
        emit freePass();
    }
}

bool TranPrintSuccess::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranPrintSuccess::onTransition(QEvent *event)
{
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    TInvPrintInfo inv;
    inv.InvCnt = 1;
    inv.InvNum = getDataFileMgr()->GetInvInfo().PreInvBeginNo;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->SetInvPrintInfo(inv);
    getLaneCtrl()->Complete(QDateTime::currentDateTime(),true);

    getOrdinaryWidget()->clearInfo();
    //表格显示
    FormRecordTable* pRecord = NULL;
    getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
    //清除laneinfo保存
    getLaneInfo()->clearCurVehInfo();
    //抬杆操作
    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    getLaneCtrl()->ConfirmComplete();
    pExit->ClearSavedInfo();
    //lane info / ctrl
}

bool TranPrintFail::eventTest(QEvent *event)
{   
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //liujian
    TInvPrintInfo inv;
    inv.InvCnt = 1;
    inv.InvNum = getDataFileMgr()->GetInvInfo().PreInvBeginNo;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->SetInvPrintInfo(inv);
    return true;
}

void TranPrintFail::onTransition(QEvent *event)
{
    //票打失败，显示票打失败页面
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle("");
    pTable->setCol(1);
    pTable->insertItem(tr("重打原发票(票号不变)"), 0);
    pTable->insertItem(tr("重打新发票(票号增加)"), 1);
    pTable->insertItem(tr("取消打印"),2);
}

void TranCancelPrint::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget * pTable =  static_cast<FormTableWidget*>(getSourceWidget());
    int nIndex = pTable->currentIndex();
    //当选择第一项，也就是选择入口收费站的时候，进行此跳转
    if(nIndex == 2)
    {
        if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
        {
            *keyType = KC_Func;
        }
        if(keyPropertity.isNumKey())
        {
            int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt();
            if(nNum == nIndex)
            {
                *keyType = KC_Func;
            }
        }
    }
}

void TranCancelPrint::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //发送打印失败取消打印报文
    getLaneCtrl()->getTollItem()->SendLaneEvent(LET_CancelPrint);
    //清除页面显示
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    getOrdinaryWidget()->clearInfo();
    TInvPrintInfo inv;
    inv.InvCnt = 1;
    inv.InvNum = getDataFileMgr()->GetInvInfo().PreInvBeginNo;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->SetInvPrintInfo(inv);
    getLaneCtrl()->Complete(QDateTime::currentDateTime(),true);
    getOrdinaryWidget()->clearInfo();
    //表格显示
    FormRecordTable* pRecord = NULL;
    getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    int cardType = pExit->getPhysicalCardType();
    QString cardNum;
    if(cardType == TYPE_PRO)
    {
        //cardNum = QString::fromLocal8Bit(pExit->getProCardBasicInfo().szCardNo,16);
        cardNum = QString::fromLocal8Bit(pExit->getProCardBasicInfo().szCardNo,strlen(pExit->getProCardBasicInfo().szCardNo));
    }
    else
    {
        cardNum = QString::number(pExit->getTollBasicInfo().M1BasicInfo.dwCardNum);
    }
    QString vehType = GetVehClassName((CVehClass)(getLaneInfo()->getVehClass()));
    QString vehPlate = getLaneInfo()->getVehPlate();
    QString vehEnStaName = QString::number(pExit->getVehEntryInfo().dwEnStationID);
    pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
    //清除laneinfo保存
    getLaneInfo()->clearCurVehInfo();
    //抬杆操作
    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    getLaneCtrl()->ConfirmComplete();
    //lane info / ctrl
    pExit->ClearSavedInfo();
    //getLaneInfo()->clearCurVehInfo();
}

void TranRePrint::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget * pTable =  static_cast<FormTableWidget*>(getSourceWidget());
    int nIndex = pTable->currentIndex();
    m_nIndex = nIndex;
    //当选择第一项，也就是选择入口收费站的时候，进行此跳转
    if(nIndex != 2)
    {
        if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
        {
            if(RePrint(nIndex))
            {
                *keyType = KC_Func;
            }
        }
        if(keyPropertity.isNumKey())
        {
            int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt();
            if(nNum == nIndex)
            {
                if(RePrint(nIndex))
                {
                    *keyType = KC_Func;
                }
            }
        }
    }
}

void TranRePrint::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

}

bool TranRePrint::RePrint(int nType)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    PrinterArm* pPrint = getDeviceFactory()->getPrinter();
    //增加票据号
    //无论打印是否成功，票号都要增加一
    //数量减一
    if(nType == 1)
    {
        if(!getDataFileMgr()->IncInvoice())
        {
            return false;
        }
    }
    memset(&pPrint->m_NoteContent, 0, sizeof(pPrint->m_NoteContent));

    pPrint->GetPrintStatus();

    QOrgCodeParamFile* pOrg = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue orgInfo;
    pOrg->queryOrgCodeValue(orgInfo, pExit->getVehEntryInfo().dwEnStationID, ORG_TYPE_STATION);
    QString sTmpStr;
    sTmpStr = GetVehClassName_LPCSTR(pExit->m_VehInfo.VehClass);
    qstrncpy(pPrint->m_NoteContent.VehClass, Utf8toGB2312(sTmpStr).data(), 6);
    if(IsCar(VehClass2Byte(pExit->m_VehInfo.VehClass)) || !getLaneInfo()->getUseWeight())
    {
        pPrint->m_NoteContent.iFeeType = 0;
    }
    else if(getLaneInfo()->getUseWeight() && IsTruck(VehClass2Byte(pExit->m_VehInfo.VehClass)))
    {
        pPrint->m_NoteContent.iFeeType = 1;
    }

    //小写金额
    pPrint->m_NoteContent.Fare_Num = pExit->getLastMoney().nLastMoney;
    //大写金额
    //时间
    qstrncpy(pPrint->m_NoteContent.szDateTime,
             QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),
             32);
    //入口站

    qstrncpy(pPrint->m_NoteContent.EnStationName,
             Utf8toGB2312(orgInfo.sOrgSName).data(),
             32);
    //出口站
    qstrncpy(pPrint->m_NoteContent.ExStationName,
             Utf8toGB2312(getLaneInfo()->getOrgInfo().sOrgSName).data(),
             32);
    //收费员代码
    qstrncpy(pPrint->m_NoteContent.CollectorID,
             QString::number(getLaneInfo()->getOper().dwOper).toLocal8Bit().data(),
             9);
    if(pPrint->m_NoteContent.iFeeType == 1)
    {
        //总重
        qstrncpy(pPrint->m_NoteContent.weight,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetWeight()/1000.00).toLocal8Bit().data(),
                 10);
        //限载
        qstrncpy(pPrint->m_NoteContent.LimitW,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetLimitWeight()/1000.00).toLocal8Bit().data(),
                 10);
        //超限率
        qstrncpy(pPrint->m_NoteContent.OverP,
                 tr("%1").arg(pExit->GetVehAxisInfo()->GetOverloadRate()/1000.00).toLocal8Bit().data(),
                 10);
    }
    if(getLaneInfo()->getShiftState() == lsRepare)
    {
        pPrint->m_NoteContent.sMark = 1;
    }
    else if(getLaneInfo()->getShiftState() == lsNormalWorking)
    {
        pPrint->m_NoteContent.sMark = 0;
    }

    if(!pPrint->Print())
    {
        LogMsg("lane", tr("打印失败"));
        ::showWarning(tr("打印失败"), false, FormWarning::WARNING);
        return false;
    }
    else
    {
        LogMsg("lane", tr("打印成功"));
        ::showWarning(tr("打印成功"), false, FormWarning::WARNING);
        return true;
    }
}

bool TranVehPass::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranVehPass::onTransition(QEvent *event)
{
    //车辆通过后线圈，发送报文，初始化数据等待下一辆车

}

void TranSpecialVeh::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() &&(keyPropertity.func == KeyArmVeh))
    {
        *keyType = KC_Func;
    }
    //普通车
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyNormalVeh)
    {
        *keyType = KC_Func;
    }
}

void TranSpecialVeh::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //计算优惠后金额并显示
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    //获取车类并显示
    int nVehClass = GetVehClassNoByName(keyPropertity.getLogicKeyName(KC_Func));
    pVeh->setVehClass(keyPropertity.getLogicKeyName(KC_Func), nVehClass );
    //laneinfo设置
    getLaneInfo()->setVehType(nVehClass);
    //设置优惠金额
    getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)nVehClass;
    //发送车辆信息改变报文
    getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VT);

}

void TranOtherFree::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyOtherFree)
    {
        *keyType = KC_Func;
    }
}

void TranOtherFree::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

    //这里加载其他免征的显示项
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("其他免征车"));
    pTable->setCol(1);
    pTable->insertItem(tr("救灾车"),tr("救灾车"));
    pTable->insertItem(tr("红十字会车"),tr("红十字会车"));
    pTable->insertItem(tr("森林消防车"),tr("森林消防车"));
    pTable->insertItem(tr("联合收割车"),tr("联合收割车"));
    pTable->insertItem(tr("其它车"),tr("其它车"));
}

void TranUpdateFeeByOtherFree::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
        FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
        if(nNum == pTable->currentIndex())
        {
            *keyType = KC_Number;
        }
    }
}

void TranUpdateFeeByOtherFree::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //计算优惠后金额并显示
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    //设置车类并显示
    QString sVehClass = pTable->currentValue().toString();
    int nVehClass = GetVehClassNoByName(sVehClass);
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->setVehClass(sVehClass, nVehClass);
    getLaneInfo()->setVehType(nVehClass);
    //设置优惠金额并显示
    getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)nVehClass;
    //发送车辆信息改变报文
    getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VT);
}

bool TranMonthCard::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranMonthCard::onTransition(QEvent *event)
{
    //这里处理包交卡刷卡事件
}

void TranMonthCardByOfficial::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyOffcial)
    {
        *keyType = KC_Func;
    }
}

void TranMonthCardByOfficial::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //这里处理包交卡按公务车键

}


void TranVehQueEnd::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{

    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyMotorcade)
    {
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle("退出车队确认");
        getMsgWidget()->setMessage(tr("确认要退出车队操作?"));
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"));
        int nRlt = getMsgWidget()->doModalShow();
        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        if(!nRlt)
        {
            return;
        }
        *keyType = KC_Func;
    }
}

void TranVehQueEnd::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //liubo
    if(!getLaneInfo()->isEntryLane())
    {
        getOrdinaryWidget()->showEntryInfo();
    }
    else
    {
        getOrdinaryWidget()->hideEntryInfo();
    }

    //清空报文中车队过车数量
    getLaneInfo()->setVehCount(0);
    FormEntryVehInfo * pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->clearData();
    FormVehQueBegin* pQue = static_cast<FormVehQueBegin*>(getSourceWidget());
    getLaneCtrl()->DoMotor(false, pQue->getVehCount());

}

void TranPaperCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyPaperCard)
    {
        FormEntryVehInfo * pVeh = static_cast<FormEntryVehInfo*>(getSourceWidget());
        if(pVeh->hasFinishVehInfo())
        {
            *keyType = KC_Func;
        }
        else
        {
            ::showWarning(tr("请输入车型"));
        }
    }
}

void TranPaperCard::onMtcKeyTransition(const MtcKeyPropertity &, int)
{
    getLaneCtrl()->DoLaneEvent(LET_PaperCard);
}

void TranPaperCardErr::onTransition(QEvent *event)
{
    FormPaperCard* pPaper = static_cast<FormPaperCard*>(getSourceWidget());
    ::showWarning(tr("无该入口站编码：%1").arg(pPaper->getEnterNo()));

}

bool TranPaperCardErr::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranPaperCardSuccess::onTransition(QEvent *event)
{
    //获取入口站编号/
    FormPaperCard* pPaper = static_cast<FormPaperCard*>(getSourceWidget());
    quint32 stationId = pPaper->getEnterNo().toUInt();
    //获取入口站名称
    QOrgCodeParamFile* pFile = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue orgValue;
    pFile->queryOrgCodeValue(orgValue, stationId);
    //清除卡信息/
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->ClearCardInfo();
    //填装报文信息
    pExit->getVehEntryInfo().nEntryType = Entry_ByManual;
    pExit->getVehEntryInfo().dwQryStationID = stationId;
    pExit->getVehEntryInfo().dwEnStationID = stationId;
    //字符集转码
    strcpy(pExit->getVehEntryInfo().szEnStationName,
           Utf8toGB2312(orgValue.sOrgSName).data());
    //保存业务类型/
    pExit->setCardType(CARD_TYPE_PAPER);
    //pExit->SetVehEntryInfo(*getLaneInfo()->getVehEntryInfo());
}

bool TranPaperCardSuccess::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranChooseBadCardType::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyBadCard)
    {/*
        if(!getLaneCtrl()->CanDoNextCar())
        {
            ::showWarning(tr("【坏卡】键无效"),true,FormWarning::WARNING);
            return;
        }*/
        FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getSourceWidget());
        if(pVeh->hasEnterVehType())
        {
            *keyType = KC_Func;
        }
        else
        {
            ::showWarning(tr("请输入车型"));
        }
    }
}

void TranChooseBadCardType::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //加载显示项
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("坏卡处理"));
    pTable->insertItem(tr("正常损坏"), 1);
    pTable->insertItem(tr("人为损坏"), 2);
}

void TranEnterCardNum::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
        FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
        if(nNum == pTable->currentIndex())
        {
            *keyType = KC_Number;
        }
    }
}

void TranEnterCardNum::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getLaneCtrl()->DoLaneEvent(LET_BadCard);
    //保存坏卡类型
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->setBadCardType(pTable->currentValue().toInt());
    pExit->getVehEntryInfo().nEntryType = Entry_ByManual;
}


void TranChooseEntrySta::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        //判断输入的卡号长度
        FormBadCard* pBad = static_cast<FormBadCard*>(getSourceWidget());
        if(pBad->lengthIsEnough())
        {
            *keyType = KC_Func;
            ::showWarning(tr(""), true, FormWarning::WARNING);
        }
        else
        {
            ::showWarning(tr("请至少输入8位数字!"));
        }
    }
}

void TranChooseEntrySta::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    QString CardID;
    char CardNet[4];
    TProCardBasicInfo ProCardBasicInfo;
    TTollCardBasicInfo M1cardbasicInfo;
    FormBadCard* pBad = static_cast<FormBadCard*>(getSourceWidget());
    ProCardBasicInfo.clear();
    M1cardbasicInfo.clear();
    //保存坏卡卡号
    getLaneInfo()->setCardNum(pBad->badCardNum());
    //将坏卡按卡号分开处理/
    //liujian
    //if(pBad->badCardNum().length()<=10)
    CardID = pBad->badCardNum();
    if(pBad->badCardNum().length()<=10)
    {
        M1cardbasicInfo.M1BasicInfo.bType=0;
        M1cardbasicInfo.M1BasicInfo.dwCardNum = CardID.toUInt();
        M1cardbasicInfo.M1BasicInfo.dwCardId=0;
        M1cardbasicInfo.M1BasicInfo.wNetworkId=0;
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        pExit->SetTollCardInfo(M1cardbasicInfo);
        pExit->setPhysicalCardType(TYPE_M1_S50_MAD);
        pExit->setCardType(CARD_TYPE_PASS_CARD);
        //pExit->getTollBasicInfo().M1BasicInfo = M1cardbasicInfo;

    }
    //liujian
    //if(pBad->badCardNum().length()>10 && pBad->badCardNum().length()<=20)
    else
        if(pBad->badCardNum().length()>10 && pBad->badCardNum().length()<=20)
        {
            ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
            //wumax == memset(&ProCardBasicInfo,0,sizeof(ProCardBasicInfo));
            ProCardBasicInfo.clear();
            ProCardBasicInfo.bVehPlateColor=0xFF;
            ProCardBasicInfo.bType=0xFF;
            memcpy(CardNet,CardID.toLocal8Bit().data(),sizeof(CardNet));
            ProCardBasicInfo.wNetworkId=ArCharToInt(CardNet,sizeof(CardNet));
            memcpy(ProCardBasicInfo.szCardNo,CardID.toLocal8Bit().data(),qMin(strlen(CardID.toLocal8Bit().data()),sizeof(ProCardBasicInfo.szCardNo)));
            pExit->SetProCardInfo(ProCardBasicInfo);
            pExit->setPhysicalCardType(TYPE_PRO);
            pExit->setCardType(-1);
        }
    //加载显示项
    //这里在目标窗口中加载中心名称,中心从QOrgCodeParamFile类中获取
    FormTableWidget * pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("[坏卡]选择入口路段"));
    pTable->setCol(2);
    pTable->setRow(5);
    QOrgCodeParamFile* pFile =(QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    QVector<SOrgCodeValue> centerList;
    pFile->getOrgList(ORG_TYPE_ROAD, 0, centerList);
    foreach(SOrgCodeValue value, centerList)
    {
        pTable->insertItem(value.sOrgSName, value.sOrgCode);
    }
}

void TranReturnInit::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyEsc)
    {
        *keyType = KC_Func;
    }
}

void TranReturnInit::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    //清除读卡入口信息
    getOrdinaryWidget()->clearInfo();
    //清除费显
    if(getDeviceFactory()->getFD() != NULL)
    {
        getDeviceFactory()->getFD()->ClearAll();
    }
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //清空金额信息
    pExit->ClearSavedInfo();
    getLaneInfo()->clearCurVehInfo();
    getOrdinaryWidget()->clearInfo();
}

//响应F12
void TranExitApp::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyAppExit)
    {
        *keyType = KC_Func;
    }
}

void TranExitApp::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget *pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->insertItem(tr("退出程序"),(int)emCloseApp);
    pTable->insertItem(tr("重启程序"),(int)emRestartApp);
    pTable->insertItem(tr("关闭计算机"),(int)emClosePC);
    pTable->insertItem(tr("重启计算机"),(int)emRestartPC);
}
//取消F12
void TranQuitSelect::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyEsc)
    {
        *keyType = KC_Func;
    }
}
//退出程序
void TranSelectExitApp::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget *pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        if(pTable->currentIndex() == keyPropertity.getLogicKeyName(KC_Func).toInt())
        {
            *keyType = KC_Func;
        }
    }

}
void TranSelectExitApp::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //系统关闭，记录时间
    QFileInfo fileInfo1(qApp->applicationDirPath() + "/RunTime.ini");
    QSettings settings(fileInfo1.fileName() , QSettings::IniFormat);
    quint64 time1 = settings.value("/runTime").toLongLong();
    quint64 time2 = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLongLong();
    if(time1 < time2)
    {
        settings.setValue("runTime",time2);
    }
    //liujian
    getLaneCtrl()->getTollItem()->SendAppMsg(false);
    if(getMainWindow() != NULL)
    {
        getMainWindow()->close();
    }
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    getLaneInfo()->setCloseType((emCloseType)pTable->currentValue().toInt());
    //显示提示信息，等待用户确认
    getMsgWidget()->setTitle(tr("系统关闭"));
    getMsgWidget()->setMessage(tr("系统正在释放资源并关闭\n请稍后..."));
    getSourceWidget()->hide();
    getMsgWidget()->doModalShow();
}

void TranLogOut::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyShift)
    {
        //下班写入时间
        QFileInfo fileInfo1(qApp->applicationDirPath() + "/RunTime.ini");
        QSettings settings(fileInfo1.fileName() , QSettings::IniFormat);
        quint64 time1 = settings.value("/runTime").toLongLong();
        quint64 time2 = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLongLong();
        if(time1 < time2)
        {
            settings.setValue("runTime",time2);
        }
        if(getDeviceFactory()->getIOBoard()->IsLockBar())
        {
            ::showWarning(tr("锁杆中不能下班！"),true);
            return;
        }
        //如果车辆未离开或者线圈有车，则不能下班
        if(!getLaneCtrl()->GetAllowLogOut())
        {
            ::showWarning(tr("车辆未离开，不能下班！"));
            return;
        }
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getMsgWidget()->setMessage(tr("请确认是否下班！"));
        //提示操作
        int nRlt = getMsgWidget()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            return;
        }

        //验证密码
        getPasswordVerify()->showType(FormPasswordVerify::LOGOUT);
        nRlt = getPasswordVerify()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();

            return;
        }
        //卡盒操作
        if(getDataFileMgr()->IsLoaded())
        {
            bool bRlt = getCardBox()->CardBoxRegister(false);
            if(!bRlt)
            {
                getOrdinaryWidget()->show();
                getSourceWidget()->show();
                getSourceWidget()->setFocus();
                return;
            }
        }

        getSourceWidget()->show();
        getOrdinaryWidget()->show();
        getSourceWidget()->setFocus();
        FormEntryVehInfo* vehInfo = static_cast<FormEntryVehInfo*>(getSourceWidget());
        //清空车型信息
        vehInfo->clearData();
        //更新字符叠加
        getDeviceFactory()->getVDM()->UpdateDateTime();
        *keyType = KC_Func;
    }
}

void TranLogOut::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

    FormMTCTitle* pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);
    pTitle->clearInfo();
    //清除工号
    getDeviceFactory()->getVDM()->UpdateCollector(0, false);
    //雨棚灯切换
    getDeviceFactory()->getIOBoard()->SetCanopy(0);
    //发送下班报文
    getLaneCtrl()->getTollItem()->SendLogoutMsg();
    //标题栏切换
    pTitle->setWorkState(lsUnlogin);
    //改变上班状态
    getLaneInfo()->setShiftState(lsUnlogin);
    if(getLaneInfo()->getShiftState() == lsUnlogin)
    {
        FormVpr* pVpr = NULL;
        getWidgetByType(emFormVpr, (QWidget**)&pVpr);
        pVpr->initPage();
    }
    FormBottomBar* pBottom = NULL;
    getWidgetByType(emFormBottomBar, (QWidget**)&pBottom);
    pBottom->showOperNo(0);

    if(getLaneInfo()->isAutoLane())
    {
        ((FormAutoCard*)getOrdinaryWidget())->showLogin(false);
    }

}

void TranFunction::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyFunc)
    {
        *keyType=KC_Func;
    }
}

void TranFunction::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)targetState();
    FormTableWidget *pTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    pTable->clearData();
    pTable->setCol(2);

    pTable->setTitle("功能选择");
    if(getLaneInfo()->isAutoLane())
    {
        pTable->insertItem(tr("更换卡盒"),1);
    }
    else
    {
        pTable->insertItem(tr("设置"),1);
        if(!getLaneInfo()->isEntryLane() || !getLaneCtrl()->CanDoNextCar())
        {
            pTable->insertItem("补卡",2,false);

        }
        else
        {
            pTable->insertItem("补卡",2);

        }
        if(getLaneInfo()->isEntryLane())
        {
            pTable->insertItem("编辑称重信息",3,false);
            pTable->insertItem("U型车选站",4,false);
            pTable->insertItem("U型车减免通行费",5,false);
            pTable->insertItem("重连读卡器",6);
            pTable->insertItem("重连打印机",7,false);
            pTable->insertItem("更换票据",8,false);
            pTable->insertItem("更换卡盒",9);
            pTable->insertItem("超长车", 10,false);

        }
        else
        {
            pTable->insertItem("编辑称重信息",3);
            pTable->insertItem("U型车选站",4,false);
            pTable->insertItem("U型车减免通行费",5,false);
            pTable->insertItem("重连读卡器",6);
            pTable->insertItem("重连打印机",7);
            pTable->insertItem("更换票据",8);
            pTable->insertItem("更换卡盒",9);
            pTable->insertItem("超长车", 10);
        }
    }
}

void TranAccedit::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    getLaneCtrl()->DoLaneEvent(LET_EditWeight);
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue()==3)
    {
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        //监控授权
        getAuth()->setMonType(mqEditWeightInfo);
        int nRlt = getAuth()->doModalShow();
        //        ::showWarning(tr("请刷班长卡，或输入工号密码授权，【取消】键返回！"), true);
        if(nRlt == 1)
        {
            getOrdinaryWidget()->show();
            getOrdinaryWidget()->hideEntryInfo();
            getModifyWeight()->doModalShow();

            //TODO:费显显示计重信息这里原来写的是count > 1
            if(getWtDataMgr()->GetVehicleCount() > 0 &&
                    IsTruck(VehClass2Byte(getLaneCtrl()->getTollItem()->m_VehInfo.VehClass)))
            {
                TWtVehicle *vehInfo = NULL;
                bool ret = getWtDataMgr()->GetFirstVehAxisInfo(&vehInfo);
                if(ret)
                {
                    if(getDeviceFactory()->getFD() != NULL)
                    {
                        getDeviceFactory()->getFD()->ShowWeightInfo(vehInfo->GetWeight(),
                                                                    (((qint64)vehInfo->GetWeight() - (qint64)vehInfo->GetLimitWeight()) >= 0 ?
                                                                         ((qint64)vehInfo->GetWeight() - (qint64)vehInfo->GetLimitWeight()) : 0),
                                                                    vehInfo->GetOverloadRate(), true);
                    }
                }
            }

            *keyType=KC_Func;
        }
        else
        {
            getSourceWidget()->show();
            getOrdinaryWidget()->show();
            getSourceWidget()->setFocus();
            *keyType = KC_Func;
        }
    }
}

void TranWeightToTable::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey()&&(keyPropertity.func==KeyConfirm||keyPropertity.func==KeyEsc))
    {
        *keyType=KC_Func;
    }

}

void TranCardToTable::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue().toInt()==6)
    {
        *keyType=KC_Func;
        QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
        pReader->StartDev();
    }
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue().toInt()==7)
    {
        *keyType=KC_Func;
        //        //qDebug()<<"重连打印机--待做";
        PrinterArm * nPrinter = getDeviceFactory()->getPrinter();
        nPrinter->StartDev();
        //        bool ret = nPrinter->StartDev();
        //        if(ret)
        //        {
        //            ::showWarning(tr("重连打印机成功！"),true);
        //        }
        //        else
        //        {
        //            ::showWarning(tr("重连打印机失败！"),true);
        //        }
    }
}

void TranIstallToTable::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();

    if(keyPropertity.isFuncKey()
            &&keyPropertity.func==KeyConfirm
            &&PTable->currentValue().toInt()==1)
    {
        *keyType=KC_Func;

    }

}

void TranIstallToTable::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    MtcLaneState *tstate=(MtcLaneState*)sourceState();
    FormTableWidget *TemTable=(FormTableWidget*)tstate->GetWindowPtr();
    TemTable->clearData();
    //    VideoCard_ARM * nVideoCard = getDeviceFactory()->getVideoCard();
    //TemTable->setCol(1);
    TemTable->setTitle("设置");
    //出口禁止使用TODO_liubo
    if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
    {
        if(getLaneInfo()->getUseFrontLoop() == 1)
        {
            TemTable->insertItem("允许存在线圈未检测到车辆发卡",1,false);
        }
        else
        {
            TemTable->insertItem("禁止存在线圈未检测到车辆发卡",1,false);
        }
    }
    else
    {
        if(getLaneInfo()->getUseFrontLoop() == 1)
        {
            TemTable->insertItem("允许存在线圈未检测到车辆发卡",1);
        }
        else
        {
            TemTable->insertItem("禁止存在线圈未检测到车辆发卡",1);
        }
    }
    if(getLaneInfo()->getUseSimulate() == 0)
    {
        TemTable->insertItem("启用【模拟】键",2);
    }
    else
    {
        TemTable->insertItem("禁止【模拟】键",2);
    }
    if(getLaneInfo()->getUsePlate() == 0)
    {
        TemTable->insertItem("启用车牌识别",3);
    }
    else
    {
        TemTable->insertItem("禁止车牌识别",3);
    }




}

void TranSetToCarInfo::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&
            PTable->currentValue().toInt()==1)
    {
        *keyType=KC_Func;
        if(getLaneInfo()->getUseFrontLoop() == 1)
        {
            getOrdinaryWidget()->hide();
            getSourceWidget()->hide();
            getAuth()->setMonType(mqUnFrontLoop);
            int nRlt = getAuth()->doModalShow();
            if(nRlt==1)
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                //getLaneInfo()->setUseFrontLoop(1);
                //                ::showWarning(tr("允许存在线圈未检测到车辆发卡功能成功"),true);
                //liujian
                getLaneCtrl()->DoLaneEvent(LET_UsesFrontLoop);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                return;
            }
        }
        else
        {
            getOrdinaryWidget()->hide();
            getSourceWidget()->hide();
            getAuth()->setMonType(mqFrontLoop);
            int nRlt = getAuth()->doModalShow();
            if(nRlt == 1)
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                //getLaneInfo()->setUseFrontLoop(0);
                //                ::showWarning(tr("禁止存在线圈未检测到车辆发卡功能成功"),true);
                //liujian
                getLaneCtrl()->DoLaneEvent(LET_NotUsesFrontLoop);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                return;
            }
        }

    }
    //TODO
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue().toInt()==2)
    {
        *keyType=KC_Func;
        if(getLaneInfo()->getUseSimulate() == 0)
        {
            getSourceWidget()->hide();
            getOrdinaryWidget()->hide();
            getAuth()->setMonType(mqSimulate);
            int nRlt = getAuth()->doModalShow();
            if(nRlt ==1)
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
                //                getLaneInfo()->setUseSimulate(1);
                //                ::showWarning(tr("启用模拟键成功"),true);
                //liujian
                getLaneCtrl()->DoLaneEvent(LET_UsesSimulate);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
            }
        }
        else
        {
            getSourceWidget()->hide();
            getOrdinaryWidget()->hide();
            getSourceWidget()->setFocus();
            getAuth()->setMonType(mqUnSimulate);
            int nRlt = getAuth()->doModalShow();
            if(nRlt == 1)
            {
                getOrdinaryWidget()->show();
                getSourceWidget()->show();
                getSourceWidget()->setFocus();
                //getLaneInfo()->setUseSimulate(0);
                //                ::showWarning(tr("禁用模拟键成功"),true);
                //liujian
                getLaneCtrl()->DoLaneEvent(LET_NotUsesSimulate);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
            }
        }

    }

    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue().toInt()==3)
    {
        *keyType=KC_Func;

        //        VideoCard_ARM * nVideoCard = getDeviceFactory()->getVideoCard();
        //        if( nVideoCard->isClose())
        if(getLaneInfo()->getUsePlate() == 0)
        {
            getSourceWidget()->hide();
            getOrdinaryWidget()->hide();
            getAuth()->setMonType(mqVLP);
            int nRlt = getAuth()->doModalShow();
            if(nRlt ==1)
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();

                //liujian
                getLaneCtrl()->DoLaneEvent(LET_UsesVLP);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
            }
        }
        else
        {
            getSourceWidget()->hide();
            getOrdinaryWidget()->hide();
            getAuth()->setMonType(mqUnVLP);
            int nRlt = getAuth()->doModalShow();
            if(nRlt ==1)
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();

                //liujian
                getLaneCtrl()->DoLaneEvent(LET_NotUsesVLP);
            }
            else
            {
                getSourceWidget()->show();
                getOrdinaryWidget()->show();
                getSourceWidget()->setFocus();
            }
        }
    }
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyEsc)
    {
        *keyType=KC_Func;
    }
}

bool TranCheckInvNum::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //判断是否还有发票，如果没有发票了，则通过该transition
    if(getDataFileMgr()->GetInvInfo().InvCount <= 0)
    {
        return true;
    }
    return false;
}
//更换票据流程F5_8
void TranRepBill::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    if(keyPropertity.isFuncKey()&&keyPropertity.func== KeyConfirm && PTable->currentValue().toInt()==8)
    {
        if(!getLaneCtrl()->CanDoNextCar())
        {
            getOrdinaryWidget()->hide();
            getOrdinaryWidget()->hideEntryInfo();
            getSourceWidget()->hide();
            getMsgWidget()->setTitle("更换票据");
            getMsgWidget()->setMessage(tr("车道内有未处理完成的车辆\n请先处理完毕后再更换票据！"));
            ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
            getMsgWidget()->doModalShow();
            getOrdinaryWidget()->show();
            getOrdinaryWidget()->showEntryInfo();
            getSourceWidget()->show();
            *keyType = KC_Func;
            return;
        }
        //更换票据时，要关闭雨棚灯
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getMsgWidget()->setTitle("更换票据");
        getMsgWidget()->setMessage(tr("您确定要更换票据吗？"));
        ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
        //提示操作
        int nRlt = getMsgWidget()->doModalShow();
        //雨棚开关
        getDeviceFactory()->getIOBoard()->SetCanopy(0);
        if(nRlt == 0)
        {
            getSourceWidget()->show();
            getOrdinaryWidget()->show();
            getSourceWidget()->setFocus();
            *keyType = KC_Func;
            getDeviceFactory()->getIOBoard()->SetCanopy(1);
        }
        else
        {
            //显示更改票据标题提示
            getInformWidget()->setInfoTitle2();
            getInformWidget()->doModalShow();
            //            getInformWidget()->setInfoTitle2();
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            *keyType=KC_Func;
            getDeviceFactory()->getIOBoard()->SetCanopy(1);
        }
        ::showWarning(tr(""),true);
    }
}
//更改卡盒流程
void TranRepCardBox::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = (FormTableWidget*)getSourceWidget();
    if(getLaneInfo()->isAutoLane())
    {
        if(keyPropertity.isFuncKey()&&keyPropertity.func== KeyConfirm && pTable->currentValue().toInt()==1)
        {
            if(!getLaneCtrl()->CanDoNextCar())
            {
                getOrdinaryWidget()->hide();
                //                getOrdinaryWidget()->hideEntryInfo();
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("车道内有未处理完成的车辆\n请先处理完毕后再更换卡盒！"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
                getMsgWidget()->doModalShow();
                getOrdinaryWidget()->show();
                //                getOrdinaryWidget()->showEntryInfo();
                getSourceWidget()->show();
                *keyType = KC_Func;
                return;
            }
            if( !getDataFileMgr()->IsLoaded())
            {
                getOrdinaryWidget()->hide();
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("您尚未装上卡盒\n确认要换上卡盒吗？"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"), true);
                //提示操作
                int nRestlt = getMsgWidget()->doModalShow();
                //取消卡盒操作
                if(nRestlt == 0)
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                    return;
                }
                bool nElt = getCardBox()->CardBoxRegister(true);
                if(nElt)
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                }
                else
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    //                ::showWarning(tr("不成功，请重新操作"),true);
                }
            }
            else
            {
                getOrdinaryWidget()->hide();
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("确认要换下卡盒吗？"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"), true);
                //提示操作
                int nRestlt = getMsgWidget()->doModalShow();
                //取消卡盒操作
                if(nRestlt == 0)
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                    return;
                }
                bool nElt = getCardBox()->CardBoxRegister(false);
                if(nElt)
                {
                    getCardBox()->CardBoxRegister(true);
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType = KC_Func;
                }
            }
        }
    }
    else
    {
        if(keyPropertity.isFuncKey()&&keyPropertity.func== KeyConfirm && pTable->currentValue().toInt()==9)
        {
            if(!getLaneCtrl()->CanDoNextCar())
            {
                getOrdinaryWidget()->hide();
                getOrdinaryWidget()->hideEntryInfo();
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("车道内有未处理完成的车辆\n请先处理完毕后再更换卡盒！"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
                getMsgWidget()->doModalShow();
                if(!getLaneInfo()->isEntryLane())
                {
                    getOrdinaryWidget()->showEntryInfo();
                }
                getOrdinaryWidget()->show();
                getSourceWidget()->show();
                *keyType = KC_Func;
                return;
            }
            if( !getDataFileMgr()->IsLoaded())
            {
                getOrdinaryWidget()->hide();
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("您尚未装上卡盒\n确认要换上卡盒吗？"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"), true);
                //提示操作
                int nRestlt = getMsgWidget()->doModalShow();
                //取消卡盒操作
                if(nRestlt == 0)
                {

                    getOrdinaryWidget()->show();

                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                    return;
                }
                bool nElt = getCardBox()->CardBoxRegister(true);
                if(nElt)
                {

                    getOrdinaryWidget()->show();

                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                }
                else
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    //                ::showWarning(tr("不成功，请重新操作"),true);
                }
            }
            else
            {
                //                if(!getLaneInfo()->isEntryLane())
                //                {
                getOrdinaryWidget()->hide();
                //                }
                getSourceWidget()->hide();
                getMsgWidget()->setTitle("更换卡盒");
                getMsgWidget()->setMessage(tr("确认要换下卡盒吗？"));
                ::showWarning(tr("【确认】键继续，【取消】键返回"), true);
                //提示操作
                int nRestlt = getMsgWidget()->doModalShow();
                //取消卡盒操作
                if(nRestlt == 0)
                {
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType=KC_Func;
                    return;
                }
                bool nElt = getCardBox()->CardBoxRegister(false);
                if(nElt)
                {
                    getCardBox()->CardBoxRegister(true);
                    getOrdinaryWidget()->show();
                    getSourceWidget()->show();
                    getSourceWidget()->setFocus();
                    *keyType = KC_Func;
                }
            }
        }
    }
}

void TranRepCardBox::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
}


void TranCheckInvNum::onTransition(QEvent *event)
{
    FormMessage* pWarn = static_cast<FormMessage*>(getTargetWidget());
    if(pWarn)
    {
        pWarn->setTitle("");
        pWarn->setMessage(tr("票据已用完\n请下班更换票据！"));
    }
}

void TranHandleFarm::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyFarmProduct)
    {
        //判断能否使用绿通车
        TWtVehicle *vehAxisInfo = NULL;
        if(!IsTruck(VehClass2Byte(getLaneCtrl()->getTollItem()->m_VehInfo.VehClass)))
        {
            ::showWarning(tr("只有货车才能选择绿通车！"), true, FormWarning::WARNING);
            return;
        }

        //货车超载不能使用绿通卡
        //if(getDeviceFactory()->getWtDev()->GetFirstVehAxisInfo(vehAxisInfo))
        if(getWtDataMgr()->GetFirstVehAxisInfo(&vehAxisInfo))
        {
            if(vehAxisInfo->GetOverloadRate() > 50)
            {
                ::showWarning(tr("货车超载不能选绿通车！"), true, FormWarning::WARNING);
                return;
            }
        }
        *keyType = KC_Func;
    }
}

void TranHandleFarm::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("绿通车菜单"));
    pTable->insertItem(tr("省内绿通车"), 20, true);
    pTable->insertItem(tr("省外绿通车"), 21, true);
}

void TranConfirmFarm::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if(keyPropertity.isNumKey())
    {
        if(pTable->currentIndex() == keyPropertity.getLogicKeyName(KC_Number).toInt())
        {
            *keyType = KC_Number;
        }
    }
}

void TranConfirmFarm::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    //改变存储的车型
    getLaneInfo()->setVehType(Byte2VehType(pTable->currentValue().toInt()));
    getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)getLaneInfo()->getVehType();
    //发送改变车类报文
    getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VT);
    //更改页面显示
    FormEntryVehInfo* pVehInfo = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVehInfo->setVehClass(pTable->currentText(), pTable->currentValue().toInt());

}


void TranJudgeUType::onTransition(QEvent *event)
{
    //加载超时项目
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    int nOverTime = pExit->GetOverTime() / 60;
    int mm = nOverTime % 60;
    int hh = nOverTime / 60;
    pTable->setTitle(tr("U型车").arg(hh).arg(mm));
    pTable->insertItem(tr("U型车选站"),1);
    pTable->insertItem(tr("U型车减免通行费"), 2);
    //pTable->insertItem(tr("U型车自动计算"),3);
}

bool TranJudgeUType::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //判断是否是U型车
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //如果已经处理过U型车，则不再次处理
    if(pExit->IsUXing())
    {
        return false;
    }

    if(pExit->getVehEntryInfo().dwEnStationID == getLaneInfo()->getOrgInfo().sOrgCode.toInt())
    {
        pExit->setUXing(true);
        return true;
    }
    return false;
}


bool TranCalcFee::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //设置车辆基本信息
    getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = (CVehClass)getLaneInfo()->getVehClass();

    //处理正常车辆的费率计算
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    QString sErr;

    //如果费率计算失败，则返回初始界面
    if(!pExit->CalcFare(sErr))
    {
        ::showWarning(tr("错误：%1").arg(sErr), false, FormWarning::WARNING);
        pExit->setOverTime(false);
        startTimer(100);
        m_bCalcError = true;
        return true;
    }
    return true;
}

void TranCalcFee::onTransition(QEvent *event)
{
    if(!m_bCalcError)
    {

        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        getLaneInfo()->getTollMoneyInfo()->nCashMoney = pExit->getFareQryResult().m_dwDiscountedDue;
        getLaneInfo()->getTollMoneyInfo()->nCashCardMoney = pExit->getCardCost();
        //计算节假日免费
        QHolidayParamFile* file = (QHolidayParamFile*)getParamMgr()->GetParamFile(cfHolidayFree);
        int rate = 100;
        bool ret = file->getVTFreeValue(getLaneInfo()->getVehClass(), rate);
        if(ret)
        {
            getLaneInfo()->getTollMoneyInfo()->nCashMoney *= (double)rate / 100.0;
            if(rate < 100)
            {
                getLaneCtrl()->getTollItem()->m_VehInfo.VehType = VT_Holiday;
            }
        }
        //发送通行费用到监控
        pExit->SendMoneyInfo(*getLaneInfo()->getTollMoneyInfo(), pExit->getCalculateMoneyInfo());
        int nMoney = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                      getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
        //显示收费金额
        FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
        pVeh->showResultMode(true);
        ::showWarning(tr("请收取通行费，并选择付款方式!"), false, FormWarning::MESSAGE);
        //应收金额
        pVeh->setTollFee(getLaneInfo()->getTollMoneyInfo()->nCashMoney / 100,
                         getLaneInfo()->getTollMoneyInfo()->nCashCardMoney / 100);
        //免收金额
        pVeh->setTollFreeFee(pExit->getFareQryResult().m_dwDiscountedFree / 100);
        //费显显示
        getDeviceFactory()->getFD()->ShowFare(nMoney * 100);
        //TODO:播放语音提示
        ////qDebug() << "end on tran calc fee...";
    }
    else
    {
        m_bCalcError = false;
    }

}

void TranCalcFee::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    //qDebug() << "sende fail signal";
    emit calcFail();
}

void TranCalcOverTimeNormal::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    getLaneInfo()->getTollMoneyInfo()->nCashMoney = pExit->getFareQryResult().m_dwDiscountedDue;
    getLaneInfo()->getTollMoneyInfo()->nCashCardMoney = pExit->getCardCost();
    //发送通行费用到监控
    pExit->SendMoneyInfo(*getLaneInfo()->getTollMoneyInfo(), pExit->getCalculateMoneyInfo());
    int nMoney = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
    //显示收费金额
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->setVehClass(tr("正常超时"));
    pVeh->showResultMode(true);
    ::showWarning(tr("请收取通行费，并选择付款方式!"), false, FormWarning::MESSAGE);
    //应收金额
    pVeh->setTollFee(getLaneInfo()->getTollMoneyInfo()->nCashMoney / 100,
                     getLaneInfo()->getTollMoneyInfo()->nCashCardMoney / 100);    //免收金额
    pVeh->setTollFreeFee(pExit->getFareQryResult().m_dwDiscountedFree / 100);
    //费显显示
    getDeviceFactory()->getFD()->ShowFare(nMoney * 100);
    //TODO:播放语音提示

}

void TranCalcOverTimeNormal::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
        //正常超时
        if(pTable->currentIndex() == 0)
        {
            //处理正常车辆的费率计算
            ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
            QString sErr;
            //设置车辆基本信息
            getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = (CVehClass)getLaneInfo()->getVehClass();
            getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)getLaneInfo()->getVehType();
            //如果费率计算失败，则返回初始界面
            if(!pExit->CalcFare(sErr))
            {
                ::showWarning(tr("错误：%1").arg(sErr), false, FormWarning::WARNING);
                pExit->setOverTime(false);
                return;
            }
            *keyType = KC_Func;
        }
    }
}

void TranExcOverTime::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
        //异常超时
        if(pTable->currentIndex() == 1)
        {
            ////qDebug() << "get here";
            //请求监控
            QEventLoop loop;
            FormAuth auth;
            connect(&auth, SIGNAL(authFinished()), &loop, SLOT(quit()));
            auth.setMonType(mqOverTime2);
            auth.show();
            auth.setFocus();
            auth.move(getSourceWidget()->mapToGlobal(getSourceWidget()->pos()));
            auth.resize(getSourceWidget()->size());
            getSourceWidget()->hide();
            loop.exec();
            if(auth.result())
            {
                FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
                pVeh->showResultMode(true);
                pVeh->setVehClass(tr("可疑超时"));
                //设置车辆基本信息
                getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = (CVehClass)getLaneInfo()->getVehClass();
                getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)getLaneInfo()->getVehType();
                //计算通行费
                ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
                pExit->setOverTime(true);
                QString sErr;
                if(!pExit->CalcTollFare(sErr))
                {
                    ::showWarning(tr("错误:%1").arg(sErr), true, FormWarning::WARNING);
                    pExit->setOverTime(false);
                }
                else
                {
                    *keyType = KC_Func;
                }
            }
            else
            {
                getSourceWidget()->show();
                getSourceWidget()->setFocus();
            }
        }
    }

}

void TranExcOverTime::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    getLaneInfo()->getTollMoneyInfo()->nCashMoney = pExit->getFareQryResult().m_dwDiscountedDue;
    getLaneInfo()->getTollMoneyInfo()->nCashCardMoney = pExit->getCardCost();
    //发送通行费用到监控
    pExit->SendMoneyInfo(*getLaneInfo()->getTollMoneyInfo(), pExit->getCalculateMoneyInfo());
    int nMoney = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
    //显示收费金额
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->setVehClass(tr("正常超时"));
    pVeh->showResultMode(true);
    ::showWarning(tr("请收取通行费，并选择付款方式!"), false, FormWarning::MESSAGE);
    //应收金额
    pVeh->setTollFee(getLaneInfo()->getTollMoneyInfo()->nCashMoney / 100,
                     getLaneInfo()->getTollMoneyInfo()->nCashCardMoney / 100);
    //免收金额
    pVeh->setTollFreeFee(pExit->getFareQryResult().m_dwDiscountedFree / 100);
    //费显显示
    getDeviceFactory()->getFD()->ShowFare(nMoney * 100);
    //TODO:播放语音提示
}

bool TranReadCard::eventTest(QEvent *event)
{
    ////qDebug() << "begin tran read card...";

    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }


    //    //判断用户是否输入完车辆信息

    return true;
}

void TranReadCard::onTransition(QEvent *event)
{

}

void TranShift::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    //验证按键信息
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyShift)
    {
        //弹出用户登陆输入窗口，提示用户使用用户名密码登陆
        getSourceWidget()->hide();
        getPasswordVerify()->showType(FormPasswordVerify::LOGIN);
        int nRlt = getPasswordVerify()->doModalShow();
        if(nRlt == 1)
        {
            *keyType = KC_Func;
        }
        else
        {
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            getLaneCtrl()->DoLaneEvent(LET_PassInFail);
        }
    }
}

void TranShift::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    showShiftInfo();
    if(getLaneInfo()->isMtcLane())
    {
        FormRecordTable* pRecord = NULL;
        getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
        pRecord->setCardAndVehNum(0,0);
        pRecord->clearExitInfo();
    }
}

void TranShift::showShiftInfo()
{
    //获取可用工班信息
    QShiftParamFile* pFile = (QShiftParamFile*)getParamMgr()->GetParamFile(cfShift);
    QList< SShiftValue > shiftArray;
    pFile->getShiftInfoByTime(QDateTime::currentDateTime(),shiftArray);

    //设置工班显示信息
    FormTableWidget* pTable = getTableWidget();
    pTable->clearData();
    pTable->setTitle(tr("班次选择"));
    pTable->setCol(1);
    QList<SShiftValue>::const_iterator it = shiftArray.begin();
    QString sShift;
    for(; it != shiftArray.end(); it++)
    {
        sShift = getShiftString(it->sShiftName, it->iWorkDate);
        pTable->insertItem(sShift, QVariant(it->wShift));
    }
}

QString TranShift::getShiftString(const QString& sShiftName, const QDate &workDate)
{
    return tr("%1 %2").arg(workDate.toString("yyyy-MM-dd")).arg(sShiftName);
}



void TranCardBox::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        if(getLaneInfo()->isAutoLane())
        {
            //根据频道号，设置卡盒编号
            *keyType = KC_Func;
        }
        else
        {
            getSourceWidget()->hide();
            //显示卡盒换上页面

            getCardBox()->CardBoxRegister(true);
            *keyType = KC_Func;
        }

    }
}

//锁杆

void TranLockBar::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyLockBar)
    {
        //隐藏源状态页面
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        if(!getDeviceFactory()->getIOBoard()->IsLockBar())
        {
            //显示确认页面
            getMsgWidget()->setTitle("锁杆确认");
            getMsgWidget()->setMessage("确认是否锁杆？");
            ::showWarning(tr("【确认】键继续，【取消】键返回"), true,FormWarning::WARNING);
        }
        else
        {
            //显示解锁确认页面
            getMsgWidget()->setTitle("解锁确认");
            getMsgWidget()->setMessage("确认解除锁杆，落下栏杆？");
            ::showWarning(tr("【确认】键继续，【取消】键返回"), true,FormWarning::WARNING);

        }
        int nRlt = getMsgWidget()->doModalShow();
        if(nRlt == 0)
        {
            getSourceWidget()->show();
            getOrdinaryWidget()->show();
            getSourceWidget()->setFocus();
            return;
        }
        else
        {
            //            ::showWarning(tr(" "),true);
            //监控授权
            //            ::showWarning(tr("请刷班长卡，或输入工号密码授权，【取消】键返回！"), true);
            getAuth()->setMonType(mqLockBar);
            nRlt = getAuth()->doModalShow();
            if(nRlt == 1)
            {
                *keyType = KC_Func;
            }
            getSourceWidget()->show();
            getOrdinaryWidget()->show();
            getSourceWidget()->setFocus();
        }
    }
}

void TranLockBar::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getLaneCtrl()->DoLockBar();
    FormDeviceState* pDev = NULL;
    getWidgetByType(emFormDevice, (QWidget**)&pDev);
    pDev->LockBar(getDeviceFactory()->getIOBoard()->IsLockBar());
}

void TranMotor::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyMotorcade)
    {
        if(getLaneCtrl()->GetVehAllowPass() > 0)
        {
            ::showWarning(tr("请等待车辆离开"));
            return;
        }
        if(getLaneCtrl()->IsTract())
        {
            ::showWarning(tr("正在进行拖车处理,请等待车辆离开"));
            return;
        }
        //显示用户确认
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle(tr("车队确认"));
        getMsgWidget()->setMessage(tr("车队开始\n请确认是否进行车队操作?"));
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"));
        int nRlt = getMsgWidget()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            return;
        }

        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        ExitTollItem* exit = (ExitTollItem*)(getLaneCtrl()->getTollItem());
        exit->ClearSavedInfo();
        ::showWarning(tr(""),true);
        *keyType = KC_Func;
    }

}

void TranMotor::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getLaneCtrl()->DoMotor(true);
    getLaneCtrl()->DoLaneEvent(LET_Motor);
    FormVehQueBegin* pMotor = static_cast<FormVehQueBegin*>(getTargetWidget());
    pMotor->clearVehCount();
}

bool TranMotorPass::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }

    QStateMachine::SignalEvent* signal = static_cast<QStateMachine::SignalEvent*>(event);
    //qDebug() << "signal count" << signal->arguments().count();
    if(signal->arguments().count() != 2)
    {
        return false;
    }
    DIGITAL_IN digital = (DIGITAL_IN)signal->arguments().value(0).toInt();
    quint8 nStatus = signal->arguments().value(1).toUInt();
    //qDebug() << "dig" << digital << "nstatus" << nStatus;
    if(digital == DI_LoopBack && nStatus == 1)
    {
        return true;
    }
    return false;
}

void TranMotorPass::onTransition(QEvent *event)
{

    FormVehQueBegin* pVeh = (FormVehQueBegin*)getSourceWidget();
    pVeh->setVehCount();
    getLaneCtrl()->MotorPass();
    ExitTollItem* exit = (ExitTollItem*)(getLaneCtrl()->getTollItem());
    exit->ClearSavedInfo();
    getDeviceFactory()->getVpr()->ClearVPRResult();
    getWtDataMgr()->Delete(1);
}

void TranUTypeFree::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if((keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm) ||
            (keyPropertity.isNumKey() &&
             keyPropertity.getLogicKeyName(KC_Number).toInt() == pTable->currentValue().toInt()))
    {
        if(pTable->currentValue().toInt() == 2)
        {
            //监控授权
            getOrdinaryWidget()->hide();
            getSourceWidget()->hide();
            getAuth()->setMonType(mqUFree);
            int nRlt = getAuth()->doModalShow();
            if(nRlt)
            {
                *keyType = KC_Func;
            }
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
        }
    }

}

void TranUTypeFree::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->setUFree(true);
}

void TranUTypeChsSta::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if((keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm) ||
            (keyPropertity.isNumKey() &&
             keyPropertity.getLogicKeyName(KC_Number).toInt() == pTable->currentValue().toInt()))
    {
        if(pTable->currentValue().toInt() == 1)
        {
            *keyType = KC_Func;
        }
    }
}

void TranUTypeChsSta::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //加载显示项
    //这里在目标窗口中加载中心名称,中心从QOrgCodeParamFile类中获取
    FormTableWidget * pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setTitle(tr("[U型车]选择入口路段"));
    pTable->setCol(2);
    pTable->setRow(5);
    QOrgCodeParamFile* pFile =(QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    QVector<SOrgCodeValue> centerList;
    pFile->getOrgList(ORG_TYPE_ROAD, 0, centerList);
    foreach(SOrgCodeValue value, centerList)
    {
        pTable->insertItem(value.sOrgSName, value.sOrgCode);
    }
}

void TranUTypeAuto::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if((keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm) ||
            (keyPropertity.isNumKey() &&
             keyPropertity.getLogicKeyName(KC_Number).toInt() == pTable->currentValue().toInt()))
    {
        if(pTable->currentValue().toInt() == 3)
        {
            *keyType = KC_Func;
        }
    }
}

void TranUTypeAuto::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
}

void TranFreePass::onTransition(QEvent *event)
{
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    //liujian
    //    pVeh->clearData();
    //    //抬杆操作
    //    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    //    getLaneCtrl()->ConfirmComplete();
    //    //lane info / ctrl
    //    getLaneInfo()->clearCurVehInfo();
    //    getLaneCtrl()->getTollItem()->ClearSavedInfo();
    TTollMoneyInfo tollMoneyInfo;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    tollMoneyInfo.clear();
    tollMoneyInfo.nCashMoney = 0;
    pExit->SetMoneyInfo(tollMoneyInfo, *getLaneInfo()->getLastMoneyInfo());
    TInvPrintInfo m_invPrtInfo;
    memset(&m_invPrtInfo, 0, sizeof(TInvPrintInfo));
    pExit->SetInvPrintInfo(m_invPrtInfo);
    //modify by wxk 免费车设置免费类型
    pExit->setPayType(PYT_Free);
    getLaneCtrl()->Complete(QDateTime::currentDateTime(),true);
    //表格显示
    FormRecordTable* pRecord = NULL;
    getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    int cardType = pExit->getPhysicalCardType();
    QString cardNum;
    if(cardType == TYPE_PRO)
    {
        cardNum = QString::number(QString::fromLocal8Bit(pExit->getProCardBasicInfo().szCardNo,16).toInt());
    }
    else
    {
        cardNum = QString::number(pExit->getProCardBasicInfo().dwCardId);
    }
    QString vehType = GetVehClassName((CVehClass)(getLaneInfo()->getVehClass()));
    QString vehPlate = getLaneInfo()->getVehPlate();
    QString vehEnStaName = QString::number(pExit->getVehEntryInfo().dwEnStationID);

    pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
    //liujian
    pVeh->clearData();
    //抬杆操作
    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    getLaneCtrl()->ConfirmComplete();
    //lane info / ctrl
    getLaneInfo()->clearCurVehInfo();
    ExitTollItem* exit = (ExitTollItem*)(getLaneCtrl()->getTollItem());
    exit->ClearSavedInfo();

    //qDebug()<<"!!!!!!!!!!get here!!!!!";
    getOrdinaryWidget()->clearInfo();
}

bool TranFreePass::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranControlCanLight::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    static bool flag = false;
    getDeviceFactory()->getIOBoard()->SetCanopy(flag);
    flag = !flag;
}

void TranControlCanLight::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyCanopyLight)
    {
        *keyType = KC_Func;
    }
}

void TranOffical::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(!keyPropertity.isFuncKey() || keyPropertity.func != KeyMonthPass)
    {
        return;
    }
    if(!getLaneCtrl()->CanDoNextCar())
    {
        ::showWarning(tr("前车未离开，无法操作!"));
        return;
    }
    if(TYPE_PRO == getLaneCtrl()->getTollItem()->getPhysicalCardType())
    {
        ::showWarning(tr("云通卡与包交卡\n不能同时使用！"));
        return;
    }
    getSourceWidget()->hide();
    getOrdinaryWidget()->hideEntryInfo();
    int nRet = getPassCardWgt()->doModalShow();
    getOrdinaryWidget()->showEntryInfo();
    getSourceWidget()->show();
    getSourceWidget()->setFocus();
    //用户按返回键返回
    if(nRet == 0)
    {
        return;
    }
    else if(nRet == 1)
    {
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        //使用包交证付费，车种为公务车
        pExit->m_VehInfo.VehType = VT_Servant;
        //清除使用包交卡信息，在某些包交卡不可读时，收费员按包交证收费。
        pExit->ClearMonthCard();
        QString sError;
        if(!pExit->CalcTollFare(sError))
        {
            ::showWarning(sError);
            return;
        }
        getLaneInfo()->getTollMoneyInfo()->nCashMoney = pExit->getFareQryResult().m_dwDiscountedDue;
        getLaneInfo()->getTollMoneyInfo()->nCashCardMoney = pExit->getCardCost();
        getLaneInfo()->getTollMoneyInfo()->nFreeMoney = pExit->getFareQryResult().m_dwDiscountedFree;
        *keyType = KC_Func;
    }

}

void TranOffical::onMtcKeyTransition(const MtcKeyPropertity &/*keyPropertity*/, int /*keyType*/)
{
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //qDebug() << "offical money:" << getLaneInfo()->getTollMoneyInfo()->nCashMoney;
    if(getLaneInfo()->getTollMoneyInfo()->nCashMoney == 0)
    {
        pExit->setPayType(PYT_Free);
        getLaneCtrl()->Complete(QDateTime::currentDateTime(), true);
        getLaneCtrl()->ConfirmComplete();
    }
    getOrdinaryWidget()->clearInfo();
    //表格显示
    FormRecordTable* pRecord = NULL;
    getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
    int cardType = pExit->getPhysicalCardType();
    QString cardNum;
    if(cardType == TYPE_PRO)
    {
        cardNum = QString::number(QString::fromLocal8Bit(pExit->getProCardBasicInfo().szCardNo,16).toInt());
    }
    else
    {
        cardNum = QString::number(pExit->getTollBasicInfo().M1BasicInfo.dwCardNum);
    }
    QString vehType = GetVehClassName((CVehClass)(getLaneInfo()->getVehClass()));
    QString vehPlate = getLaneInfo()->getVehPlate();
    QString vehEnStaName = QString::number(pExit->getVehEntryInfo().dwEnStationID);

    pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
    //清除laneinfo保存
    getLaneInfo()->clearCurVehInfo();
    //抬杆操作
    //getDeviceFactory()->getIOBoard()->SetOneVehPass();
    pExit->ClearSavedInfo();
    getLaneInfo()->addAllowPass();
}

void TranTract::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyHaul)
    {
        if(!getLaneCtrl()->IsAllowTract())
        {
            return;
        }
        //确认是否需要牵引
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle(tr("拖车"));
        if(!getLaneCtrl()->IsTract())
        {
            getMsgWidget()->setMessage(tr("请确认是否进行拖车操作？"));
        }
        else
        {
            getMsgWidget()->setMessage(tr("请确认是否结束拖车操作？"));
        }
        int nRet = getMsgWidget()->doModalShow();
        if(nRet == 1)
        {
            *keyType = KC_Func;
        }
        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
    }
}

void TranTract::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getLaneCtrl()->DoTract();
}

void TranViolation::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyViolation)
    {
        *keyType = KC_Func;
    }
}

void TranViolation::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getTargetWidget());
    pTable->clearData();
    pTable->setCol(1);
    pTable->setTitle(tr("违章"));
    pTable->insertItem(tr("误报警"), 0);
    pTable->insertItem(tr("闯关"), 1);
}

void TranFinViolation::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        *keyType = KC_Func;
    }
    if(keyPropertity.isNumKey())
    {
        int nNum = keyPropertity.getLogicKeyName(KC_Number).toInt() - 1;
        //qDebug() << "nNum =" << nNum;
        FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
        if(nNum == pTable->currentIndex())
        {
            *keyType = KC_Number;
        }
    }
}

void TranFinViolation::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    //qDebug() << "current index----" << pTable->currentIndex();
    if(pTable->currentIndex() == 0)
    {
        //误报警
        getLaneCtrl()->DoViolate(31);

    }
    else if(pTable->currentIndex() == 1)
    {
        //闯关
        getLaneCtrl()->DoViolate(21);
        getLaneCtrl()->getTollItem()->SetViolateHeldBack(LST_TYPE_VIOLATE);
        getLaneCtrl()->getTollItem()->SetOccurTime(QDateTime::currentDateTime());
        getLaneCtrl()->getTollItem()->SendRushMsg(LST_TYPE_VIOLATE);
        //TODO_liujian
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        getLaneInfo()->clearCurVehInfo();
        getLaneInfo()->setVehType(VT_Normal);
        pExit->ClearSavedInfo();
    }

}

bool TranFinRead::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranFinRead::onTransition(QEvent *event)
{
    //显示入口站
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    QOrgCodeParamFile* pOrg = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue orgValue;
    pOrg->queryOrgCodeValue(orgValue, pExit->getVehEntryInfo().dwEnStationID);
    QString sStation = tr("%1(%2)").arg(orgValue.sOrgSName).arg(pExit->getVehEntryInfo().dwEnStationID);
    //获取通行时间
    int nPassTime = pExit->getVehEntryInfo().nPassTime;
    int hh = nPassTime / 3600;
    int mm = (nPassTime % 3600) / 60;
    int ss = nPassTime % 60;
    QString sCard;
    //获取卡号
    if(pExit->getPhysicalCardType() == TYPE_PRO)
    {
        sCard = QString::number(pExit->getProCardBasicInfo().dwCardId);
    }
    else
    {
        sCard = QString::number(pExit->getTollBasicInfo().M1BasicInfo.dwCardNum);
    }
    //显示入口信息
    getOrdinaryWidget()->setEntryInfo(pExit->getVehEntryInfo().EnTime,
                                      GB2312toUtf8(pExit->getVehEntryInfo().szEnVLP),
                                      sStation,
                                      sCard,
                                      QString::number(pExit->getVehEntryInfo().dwEnOper),
                                      tr("%1时%2分%3秒").arg(hh).arg(mm).arg(ss),
                                      GB2312toUtf8(pExit->getVehEntryInfo().FlagInfo)
                                      );
}

void TranChangeCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyChangeCard)
    {
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        if(pExit->getVehEntryInfo().nEntryType == Entry_None ||
                pExit->getVehEntryInfo().nEntryType == Entry_ByManual)
        {
            ::showWarning(tr("【换卡】键无效"));
            return;
        }
        if(getLaneInfo()->getSubPay())
        {
            ::showWarning(tr("复合交易中，换卡键无效"));
            return;
        }
        if(pExit->getChangeEntryFlag())
        {
            ::showWarning(tr("已清除卡内入口信息，按【换卡】键无效！"));
            return;
        }
        if(IsFullFreeVehTollMode(pExit->m_VehInfo.VehType))
        {
            ::showWarning(tr("免费车辆，按【换卡】键无效！"));
            return;
        }
        if(IsYTCard(pExit->getProCardBasicInfo()) &&
                pExit->getProCardBasicInfo().bUserType == CARD_UserType_FullFree)
        {
            ::showWarning(tr("免费卡，按【换卡】键无效！"));
            return;
        }
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getAuth()->setMonType(mqChangeCard);
        int nRet = getAuth()->doModalShow();
        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        if(nRet == 0)
        {
            return;
        }
        *keyType = KC_Func;
    }

}

void TranChangeCard::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    pExit->setChangedCard(true);
    getLaneInfo()->getTollMoneyInfo()->nCashMoney = pExit->getFareQryResult().m_dwDiscountedDue * 2;
    getLaneInfo()->getTollMoneyInfo()->nCashCardMoney = pExit->getCardCost();
    getLaneInfo()->getTollMoneyInfo()->nFreeMoney = pExit->getFareQryResult().m_dwDiscountedFree;
    //发送通行费用到监控
    pExit->SendMoneyInfo(*getLaneInfo()->getTollMoneyInfo(), pExit->getCalculateMoneyInfo());
    int nMoney = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
    //显示收费金额
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(true);
    ::showWarning(tr("请收取通行费，并选择付款方式!"), false, FormWarning::MESSAGE);
    //应收金额
    pVeh->setTollFee(getLaneInfo()->getTollMoneyInfo()->nCashMoney / 100,
                     getLaneInfo()->getTollMoneyInfo()->nCashCardMoney / 100);
    //免收金额
    pVeh->setTollFreeFee(pExit->getFareQryResult().m_dwDiscountedFree / 100);
    //费显显示
    getDeviceFactory()->getFD()->ShowFare(nMoney * 100);
}

void TranRePrint::timeOutToPrint()
{
    //qDebug() << "tran re PRINT";
    RePrint(m_nIndex);
}

bool TranRushLogout::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    QStateMachine::SignalEvent* signalEvent = static_cast<QStateMachine::SignalEvent*>(event);
    if(signalEvent->arguments().count() != 2)
    {
        return false;
    }
    DIGITAL_IN dig = (DIGITAL_IN)signalEvent->arguments().value(0).toUInt();
    quint8 nStatus = signalEvent->arguments().value(1).toUInt();
    if(dig == DI_LoopBack && nStatus == 1)
    {
        //后线圈有车
        return true;
    }
    return false;
}

void TranRushLogout::onTransition(QEvent *event)
{
    getLaneCtrl()->SavePic(QDateTime::currentDateTime(), 0);
    getLaneCtrl()->SavePic(QDateTime::currentDateTime(), 1);
    //黄闪报警TODO:定时器
    if(getLaneInfo()->getControlDevType() == 0)
    {
        if(getDeviceFactory()->getIOBoard() != NULL)
        {
            getDeviceFactory()->getIOBoard()->SetAlarm(1);
        }
    }
    else if(getLaneInfo()->getControlDevType() == 1)
    {
        if(getDeviceFactory()->getFD() != NULL)
        {
            getDeviceFactory()->getFD()->Alarm(true);
        }
    }
    //界面显示

    //发送报文
    getLaneCtrl()->DoViolate(LST_TYPE_VIOLATE_LOGOUT);

    getLaneCtrl()->getTollItem()->SetViolateHeldBack(LST_TYPE_VIOLATE_LOGOUT);
    getLaneCtrl()->getTollItem()->SetOccurTime(QDateTime::currentDateTime());
    getLaneCtrl()->getTollItem()->SendRushMsg(LST_TYPE_VIOLATE_LOGOUT);
}

bool TranRushLogin::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    QStateMachine::SignalEvent* signalEvent = static_cast<QStateMachine::SignalEvent*>(event);
    if(signalEvent->arguments().count() != 2)
    {
        return false;
    }
    //如果当前为车队，或者有允许通过车辆，则没有闯关
    if(getLaneCtrl()->IsMotoring() || getLaneInfo()->getAllowPass() > 0)
    {
        return false;
    }
    DIGITAL_IN dig = (DIGITAL_IN)signalEvent->arguments().value(0).toUInt();
    quint8 nStatus = signalEvent->arguments().value(1).toUInt();
    if(dig == DI_LoopBack && nStatus == 1)
    {
        //后线圈有车
        return true;
    }
    return false;
}

void TranRushLogin::onTransition(QEvent */*event*/)
{
    getLaneCtrl()->SavePic(QDateTime::currentDateTime(), 0);
    getLaneCtrl()->SavePic(QDateTime::currentDateTime(), 1);
    //黄闪报警TODO:定时器
    if(getLaneInfo()->getControlDevType() == 0)
    {
        getDeviceFactory()->getIOBoard()->SetAlarm(1);
    }
    else if(getLaneInfo()->getControlDevType() == 1)
    {
        getDeviceFactory()->getFD()->Alarm(true);
    }
    //界面显示
    getSourceWidget()->hide();
    int ret = 1;
    if(getTableWidget()->isHidden())
    {
        getTableWidget()->clearData();
        getTableWidget()->setTitle(tr("闯关确认"));
        getTableWidget()->insertItem(tr("误报警"), 0);
        getTableWidget()->insertItem(tr("闯关"), 1);
        ret = getTableWidget()->doModalShow();
    }
    if(ret != -1 && ret != 0)
    {
        //发送报文
        getLaneCtrl()->DoViolate(LST_TYPE_VIOLATE);
        getLaneCtrl()->getTollItem()->SetViolateHeldBack(LST_TYPE_VIOLATE);
        getLaneCtrl()->getTollItem()->SetOccurTime(QDateTime::currentDateTime());
        getLaneCtrl()->getTollItem()->SendRushMsg(LST_TYPE_VIOLATE);
        //TODO_liujian
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        getLaneInfo()->clearCurVehInfo();
        getLaneInfo()->setVehType(VT_Normal);
        pExit->ClearSavedInfo();
    }
    getSourceWidget()->show();
    getSourceWidget()->setFocus();

}

void TranLogOutFunc::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    if(m_nType < 2)
    {
        getSourceWidget()->hide();
        getFuncForm()->doModalShow();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
    }
    else
    {
        getSourceWidget()->hide();
        getLoadParam()->ReLoadAll();
        getLoadParam()->doModalShow();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
    }
}

void TranLogOutFunc::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyFunc)
    {
        getTableWidget()->clearData();
        getTableWidget()->setTitle("软件维护");
        getTableWidget()->insertItem(tr("导出全部报文数据"), 0);
        getTableWidget()->insertItem(tr("导出未上传报文数据"), 1);
        getTableWidget()->insertItem(tr("更新参数文件"), 2);
        getSourceWidget()->hide();
        int nRet = getTableWidget()->doModalShow();
        if(nRet != -1)
        {
            *keyType = KC_Func;
        }
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        getFuncForm()->setFuncType(nRet);
        m_nType = nRet;
    }
}

void TranLongVeh::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //显示确认超长车
    getMsgWidget()->setTitle(tr("超长车处理"));
    if(!m_bIsLongVeh)
    {
        getMsgWidget()->setMessage(tr("您确定要进行超长车处理吗？"));
    }
    else
    {
        getMsgWidget()->setMessage(tr("您确定要取消超长车处理吗？"));
    }
    getOrdinaryWidget()->hide();
    int ret = getMsgWidget()->doModalShow();
    getOrdinaryWidget()->show();
    getTargetWidget()->setFocus();
    if(ret == 0)
    {
        return;
    }
    if(ret == 1)
    {
        if(m_bIsLongVeh)
        {
            //判断是否解除锁杆
            if(getDeviceFactory()->getIOBoard()->IsLockBar())
            {
                getOrdinaryWidget()->hide();
                getMsgWidget()->setTitle(tr("解除锁杆确认"));
                getMsgWidget()->setMessage(tr("是否要解除锁杆操作"));
                ret = getMsgWidget()->doModalShow();
                if(ret == 1)
                {
                    //监控授权
                    getAuth()->setMonType(mqUnLockBar);
                    ret = getAuth()->doModalShow();
                    if(ret == 1)
                    {
                        getLaneCtrl()->DoLockBar();
                        FormDeviceState* pDev = NULL;
                        getWidgetByType(emFormDevice, (QWidget**)&pDev);
                        pDev->LockBar(getDeviceFactory()->getIOBoard()->IsLockBar());
                    }
                }
                getOrdinaryWidget()->show();
                getTargetWidget()->setFocus();
            }
        }
        else
        {
            //判断是否要锁杆
            if(!getDeviceFactory()->getIOBoard()->IsLockBar())
            {
                getOrdinaryWidget()->hide();
                getMsgWidget()->setTitle(tr("锁杆确认"));
                getMsgWidget()->setMessage(tr("是否要进行锁杆操作"));
                ret = getMsgWidget()->doModalShow();
                if(ret == 1)
                {
                    //监控授权
                    getAuth()->setMonType(mqLockBar);
                    ret = getAuth()->doModalShow();
                    if(ret == 1)
                    {
                        getLaneCtrl()->DoLockBar();
                        FormDeviceState* pDev = NULL;
                        getWidgetByType(emFormDevice, (QWidget**)&pDev);
                        pDev->LockBar(getDeviceFactory()->getIOBoard()->IsLockBar());
                    }
                }
                getOrdinaryWidget()->show();
                getTargetWidget()->setFocus();
            }
        }
        m_bIsLongVeh = !m_bIsLongVeh;
    }
    getOrdinaryWidget()->show();
}

void TranLongVeh::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    FormTableWidget* pTable = static_cast<FormTableWidget*>(getSourceWidget());
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm &&
            pTable->currentValue() == 10)
    {
        *keyType = KC_Func;
    }
}

void TranWeightDetail::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getSourceWidget()->hide();
    getOrdinaryWidget()->hideEntryInfo();
    getWtDetail()->doModalShow();
    getOrdinaryWidget()->showEntryInfo();
    getSourceWidget()->show();
    getSourceWidget()->setFocus();
}

void TranWeightDetail::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyDetailWeight)
    {
        *keyType = KC_Func;
    }
}

void TranDisInvoice::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    if(!(getLaneCtrl()->GetVehAllowPass() > 0) && !getLaneInfo()->IsLaptop())
    {
        ::showWarning(tr("车辆已离开，【废票】键无效!"));
        return;
    }
    if(getLaneCtrl()->getTollItem()->GetMsgToSend().TollInfo.LastMoney <= 0 ||
            getLaneCtrl()->getTollItem()->GetMsgToSend().TollInfo.PayType != 0)
    {
        ::showWarning(tr("【废票】键无效！"));
        return;
    }
    if(getDataFileMgr()->GetInvInfo().InvBeginNo <= 0)
    {
        ::showWarning(tr("票据已用完，请下班更换票据！"));
        return;
    }
    getSourceWidget()->hide();
    getOrdinaryWidget()->hide();
    getMsgWidget()->setTitle(tr("废票确认"));
    getMsgWidget()->setMessage(tr("请确认是否重打发票"));
    ::showWarning(tr("【确认】键重新打印，【取消】键返回"));
    int nRet = getMsgWidget()->doModalShow();
    getOrdinaryWidget()->show();
    getSourceWidget()->show();
    getSourceWidget()->setFocus();
    if(!nRet)
    {
        return;
    }
    int nPrintStatus = getDeviceFactory()->getPrinter()->GetPrintStatus();
    if(nPrintStatus < 0)
    {
        LogMsg("lane", tr("打印机故障或缺纸，请检查！"));
        ::showWarning(tr("打印机故障或缺纸，请检查！"));
        return;
    }
    getLaneCtrl()->DoReprintInvoice();
}

void TranDisInvoice::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyBadInvoice)
    {
        if(getLaneInfo()->getVehClass() != 0)
        {
            ::showWarning(tr("请取消车型输入!"));
            return;
        }
        if(!getLaneCtrl()->IsAllowReprint())
        {
            ::showWarning(tr("【废票】键无效！"));
            return;
        }
        *keyType = KC_Func;
    }
}

bool TranErrorReInit::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        //qDebug() << "get fail signal , false";
        return false;
    }
    //qDebug() << "get success signal";
    return true;
}

void TranErrorReInit::onTransition(QEvent *event)
{
    FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
    pVeh->showResultMode(false);
    pVeh->clearData();
    //清除读卡入口信息
    getOrdinaryWidget()->clearInfo();
    //清除费显
    if(getDeviceFactory()->getFD() != NULL)
    {
        getDeviceFactory()->getFD()->ClearAll();
    }
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    //清空金额信息
    pExit->ClearSavedInfo();
    getLaneInfo()->clearCurVehInfo();
    getOrdinaryWidget()->clearInfo();
}

void TranChangeVpr::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormVpr* pVpr = NULL;
    getWidgetByType(emFormVpr, (QWidget**)&pVpr);
    if(pVpr != NULL)
    {
        pVpr->changePage();
    }

}

void TranChangeVpr::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyAppExit)
    {
        if(getLaneInfo()->getShiftState() == lsNormalWorking)
        {
            *keyType = KC_Func;
        }
    }
}
//入口纸卡
void TranEntryPaperCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyPaperCard)
    {
        if(!getLaneCtrl()->CanDoNextCar())
        {
            ::showWarning(tr("前车未离开，无法操作"),true,FormWarning::WARNING);
            return;
        }
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getMsgWidget()->setTitle("纸卡操作");
        getMsgWidget()->setMessage(tr("确定要进行纸卡操作吗?"));
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"));
        int nRlt = getMsgWidget()->doModalShow();
        getSourceWidget()->show();
        getOrdinaryWidget()->show();
        getSourceWidget()->setFocus();
        if(!nRlt)
        {
            return;
        }
        *keyType = KC_Func;

    }
}
void TranEntryPaperCard::onMtcKeyTransition(const MtcKeyPropertity &, int)
{
    //TODO纸卡未完成操作
    getLaneCtrl()->DoLaneEvent(LET_PaperCard);
    TollItem* pExit = (TollItem*)getLaneCtrl()->getTollItem();
    pExit->setCardType(CARD_TYPE_PAPER);

}

bool TranEntryAllowPass::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranEntryAllowPass::onTransition(QEvent *event)
{
    getEntryCardProcessor()->EntryWriteCardSucComplate();

    //清除页面显示
    if(getLaneInfo()->isAutoLane())
    {
        //自动发卡暂时不清除车辆通过信息，直到车辆通过
    }
    else
    {
        //表格显示
        FormRecordTable* pRecord = NULL;
        getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
        pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
        FormEntryVehInfo* pVeh = static_cast<FormEntryVehInfo*>(getTargetWidget());
        pVeh->showResultMode(false);
        pVeh->clearData();
    }
    TollItem * pExit = (TollItem *)getLaneCtrl()->getTollItem();
    pExit->ClearSavedInfo();
    getLaneInfo()->clearCurVehInfo();
    getLaneInfo()->addAllowPass();

}

bool TranWriteCard::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //此处处理云通卡确认界面
    if(getLaneCtrl()->getTollItem()->getPhysicalCardType() == TYPE_PRO)
    {
        getSourceWidget()->hide();
        int nRet = getShowProInfo()->doModalShow();
        FormEntryVehInfo* pVehInfo = static_cast<FormEntryVehInfo*>(getSourceWidget());
        pVehInfo->clearData();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        if(nRet == 1)
        {
            //操作员确认信息无误，写卡
            nRet = getEntryCardProcessor()->WriteProCard();
            if(nRet == 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        //通行卡直接放行
        return true;
    }
}

void TranWriteCard::onTransition(QEvent *event)
{
}
//倒车事件
void TranEntryReverse::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyReversing)
    {
        if(getLaneCtrl()->GetVehAllowPass() <= 0)
        {
            ::showWarning(tr("车辆已离开，无法倒车"),true);
            return;
        }
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getMsgWidget()->setTitle("倒车处理");
        getMsgWidget()->setMessage("请确认是否要进行倒车操作");
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"),true);
        int nRlt = getMsgWidget()->doModalShow();
        getSourceWidget()->show();
        getOrdinaryWidget()->show();
        getSourceWidget()->setFocus();
        if(!nRlt)
        {
            return;
        }
        else
        {
            getSourceWidget()->hide();
            nRlt = getEntryReverse()->doModalShow();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            //TODO:判断是否倒车成功
            if(nRlt == 1)
            {
                //表格显示
                FormRecordTable* pRecord = NULL;
                getWidgetByType(emFormRecordTable, (QWidget**)&pRecord);
                pRecord->setCardAndVehNum(getShiftSum()->GetTotalCardVCNt(),getShiftSum()->GetNormalVCnt());
            }
        }
        *keyType = KC_Func;
    }
}

void TranEntryReverse::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

}

void TranRepairCard::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    MtcLaneState *tmpstate=(MtcLaneState*)sourceState();
    FormTableWidget *PTable=(FormTableWidget*)tmpstate->GetWindowPtr();
    getLaneCtrl()->DoLaneEvent(LET_RepairCard);
    if(keyPropertity.isFuncKey()&&keyPropertity.func==KeyConfirm&&PTable->currentValue()==2)
    {
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        //监控授权
        getAuth()->setMonType(mqRepairCard);
        int nRlt = getAuth()->doModalShow();
        //        ::showWarning(tr("请刷班长卡，或输入工号密码授权，【取消】键返回！"), true);
        if(nRlt == 1)
        {
            getOrdinaryWidget()->show();
            getOrdinaryWidget()->hideEntryInfo();
            //getModifyWeight()->doModalShow();
            //补卡标志---记得清空(reset已清空)
            getLaneCtrl()->setRepairCard(true);
            *keyType=KC_Func;
        }
        else
        {
            getSourceWidget()->show();
            getOrdinaryWidget()->show();
            getSourceWidget()->setFocus();
            *keyType = KC_Func;
        }
    }
}

void TranRepairCard::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
}

bool TranAutoException::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranAutoException::onTransition(QEvent *event)
{
}


bool TranAutoNormal::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);
}

void TranAutoNormal::onTransition(QEvent *event)
{
}



bool TranAutoLockBar::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);

}

void TranAutoLockBar::onTransition(QEvent *event)
{
    //发送锁杆报文
    getLaneCtrl()->DoLockBar();
    //控制外设锁杆
    FormDeviceState* pDev = NULL;
    getWidgetByType(emFormDevice, (QWidget**)&pDev);
    pDev->LockBar(getDeviceFactory()->getIOBoard()->IsLockBar());
}


bool TranAutoWriteSuccess::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    QStateMachine::SignalEvent* signalEvent = static_cast<QStateMachine::SignalEvent*>(event);
    if(signalEvent->arguments().count() != 1)
    {
        return false;
    }
    int index = signalEvent->arguments().at(0).toInt();
    ::showWarning(tr("%1号卡机出卡成功，等待用户取卡").arg(index + 1));
    return true;

}

void TranAutoWriteSuccess::onTransition(QEvent *event)
{
}


bool TranAutoWriteFail::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);

}

void TranAutoWriteFail::onTransition(QEvent *event)
{
}


bool TranAutoAllowPass::eventTest(QEvent *event)
{
    return QSignalTransition::eventTest(event);

}

void TranAutoAllowPass::onTransition(QEvent *event)
{
}


void TranAutoLogOut::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyShift)
    {
        //如果车辆未离开或者线圈有车，则不能下班
        if(!getLaneCtrl()->GetAllowLogOut())
        {
            ::showWarning(tr("车辆未离开，不能下班！"));
            return;
        }
        getOrdinaryWidget()->hide();
        getSourceWidget()->hide();
        getMsgWidget()->setMessage(tr("请确认是否下班！"));
        //提示操作
        int nRlt = getMsgWidget()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            return;
        }

        //验证密码
        getPasswordVerify()->showType(FormPasswordVerify::LOGOUT);
        nRlt = getPasswordVerify()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();

            return;
        }

        getSourceWidget()->show();
        getOrdinaryWidget()->show();
        getSourceWidget()->setFocus();

        *keyType = KC_Func;
    }
}

void TranAutoLogOut::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    FormMTCTitle* pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);
    pTitle->clearInfo();
    //雨棚灯切换
    getDeviceFactory()->getIOBoard()->SetCanopy(0);
    //发送下班报文
    getLaneCtrl()->getTollItem()->SendLogoutMsg();
    //标题栏切换
    pTitle->setWorkState(lsUnlogin);
    //改变上班状态
    getLaneInfo()->setShiftState(lsUnlogin);
    FormBottomBar* pBottom = NULL;
    getWidgetByType(emFormBottomBar, (QWidget**)&pBottom);
    pBottom->showOperNo(0);
}

void TranAutoMotor::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyMotorcade)
    {
        if(getLaneCtrl()->GetVehAllowPass() > 0)
        {
            ::showWarning(tr("请等待车辆离开"));
            return;
        }
        //显示用户确认
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle(tr("车队确认"));
        getMsgWidget()->setMessage(tr("车队开始\n请确认是否进行车队操作?"));
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"));
        int nRlt = getMsgWidget()->doModalShow();
        if(!nRlt)
        {
            getOrdinaryWidget()->show();
            getSourceWidget()->show();
            getSourceWidget()->setFocus();
            return;
        }

        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        ExitTollItem* exit = (ExitTollItem*)(getLaneCtrl()->getTollItem());
        exit->ClearSavedInfo();
        ::showWarning(tr(""),true);
        *keyType = KC_Func;
    }
}

void TranAutoMotor::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    getLaneCtrl()->DoMotor(true);
    getLaneCtrl()->DoLaneEvent(LET_Motor);
    FormVehQueBegin* pMotor = static_cast<FormVehQueBegin*>(getTargetWidget());
    pMotor->clearVehCount();
}
bool TranAutoMotorPass::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }

    QStateMachine::SignalEvent* signal = static_cast<QStateMachine::SignalEvent*>(event);
    //qDebug() << "signal count" << signal->arguments().count();
    if(signal->arguments().count() != 2)
    {
        return false;
    }
    DIGITAL_IN digital = (DIGITAL_IN)signal->arguments().value(0).toInt();
    quint8 nStatus = signal->arguments().value(1).toUInt();
    //qDebug() << "dig" << digital << "nstatus" << nStatus;
    if(digital == DI_LoopBack && nStatus == 1)
    {
        return true;
    }
    return false;
}
void TranAutoMotorPass::onTransition(QEvent *event)
{
    FormVehQueBegin* pVeh = (FormVehQueBegin*)getSourceWidget();
    pVeh->setVehCount();
    getLaneCtrl()->MotorPass();
    ExitTollItem* exit = (ExitTollItem*)(getLaneCtrl()->getTollItem());
    exit->ClearSavedInfo();
    getDeviceFactory()->getVpr()->ClearVPRResult();
    getWtDataMgr()->Delete(1);
}
void TranAutoVehQueEnd::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyMotorcade)
    {
        getSourceWidget()->hide();
        getOrdinaryWidget()->hide();
        getMsgWidget()->setTitle("退出车队确认");
        getMsgWidget()->setMessage(tr("确认要退出车队操作?"));
        ::showWarning(tr("请按【确认】键继续，【取消】键返回"));
        int nRlt = getMsgWidget()->doModalShow();
        getOrdinaryWidget()->show();
        getSourceWidget()->show();
        getSourceWidget()->setFocus();
        if(!nRlt)
        {
            return;
        }
        *keyType = KC_Func;
    }
}

void TranAutoVehQueEnd::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //清空报文中车队过车数量
    getLaneInfo()->setVehCount(0);
    //切换页面
    FormLog* pLog = NULL;
    getWidgetByType(emFormLog, (QWidget**)&pLog);
    //清空报文中车队过车数量
    getLaneInfo()->setVehCount(0);
    FormAutoVehInfo * pVeh = static_cast<FormAutoVehInfo*>(getTargetWidget());
    pVeh->clearData();
    FormVehQueBegin* pQue = static_cast<FormVehQueBegin*>(getSourceWidget());
    getLaneCtrl()->DoMotor(false, pQue->getVehCount());
}

void TranConfirmDev::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
}

void TranConfirmDev::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        //检查是否有不能启动的设备,如果有则进行提示
        if(getLaneInfo()->isAutoLane())
        {
            if(getDeviceFactory()->getCardMgr()->GetStatus() != DEV_STATUS_OK)
            {
                getSourceWidget()->hide();
                getMsgWidget()->setTitle(tr("确认设备状态"));
                getMsgWidget()->setMessage(tr("卡机设备故障\n请确认设备是否打开\n如已打开，请联系维护人员"));
                getMsgWidget()->doModalShow();
                ::showWarning(tr("请按【确认】键继续"));
                getSourceWidget()->show();
                getSourceWidget()->setFocus();
            }
            else
            {
                *keyType = KC_Func;
            }
        }
        //mtc程序判断
        else if(getLaneInfo()->isMtcLane())
        {
            //获取lane.log的最后的修改时间，如果当前时间小于该时间，则说明时间被修改，此时需要收费员手动确认时间
            QFileInfo fileInfo(qApp->applicationDirPath() + "/RunTime.ini");
            QSettings settings(fileInfo.fileName() , QSettings::IniFormat);
            quint64 time = settings.value("/runTime").toULongLong();
            if( time >= QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toULongLong())
            {
                getSourceWidget()->hide();
                getRegulateTimeForm()->doModalShow();
                getSourceWidget()->show();
                getSourceWidget()->setFocus();
            }
            *keyType = KC_Func;
        }
        //etc程序判断
        else
        {
            *keyType = KC_Func;
        }

    }
}


void TranEntryConfirmShift::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        //将用户选择的工班信息保存
        FormTableWidget* pTable = (FormTableWidget*)((MtcLaneState*)sourceState())->GetWindowPtr();
        quint16 wShiftNo =  pTable->currentValue().toUInt();
        QShiftParamFile* pShiftFile = (QShiftParamFile*)getParamMgr()->GetParamFile(cfShift);
        //保存工班信息
        getLaneInfo()->setShiftInfo( pShiftFile->getShiftInfoByNo(wShiftNo));
        //保存登陆时间
        getLaneInfo()->setLogTime(QDateTime::currentDateTime());

        //处理卡盒
        if(getLaneInfo()->isAutoLane())
        {
            //根据频道号，设置卡盒编号
            *keyType = KC_Func;
        }
        else
        {
            getSourceWidget()->hide();
            //显示卡盒换上页面

            getCardBox()->CardBoxRegister(true);
            *keyType = KC_Func;
        }
    }
}

void TranEntryConfirmShift::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{

}

bool TranAutoWriteCard::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    //此处处理云通卡确认界面
    if(getLaneCtrl()->getTollItem()->getPhysicalCardType() == TYPE_PRO)
    {

        //操作员确认信息无误，写卡
        int nRet = getEntryCardProcessor()->WriteProCard();
        if(nRet == 0)
        {
            return false;
        }
    }
    //放行
    return true;
}

void TranAutoWriteCard::onTransition(QEvent *event)
{
}


bool TranCheckFreeDisk::eventTest(QEvent *event)
{
    bool result = QSignalTransition::eventTest(event);
    if(result)
    {
        result = !isDiskSpaceEnough();
    }
    return result;
}

void TranCheckFreeDisk::onTransition(QEvent *event)
{

}
