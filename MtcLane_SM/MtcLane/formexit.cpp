#include "formexit.h"
FormExit::FormExit(QWidget* widget, QLayout* layout) :
    FormBase(widget, layout)
{
}

void FormExit::createStates()
{
    //调用基类构造
    FormBase::createStates();

    m_initState->assignProperty(this, "entryInfo", true);
    m_finState->assignProperty(this, "entryInfo", true);
    m_funcState->assignProperty(this, "entryInfo", false);
    m_setupState->assignProperty(this, "entryInfo", false);
    m_otherState->assignProperty(this, "entryInfo", false);
    m_rushState->assignProperty(this, "entryInfo", false);

    m_overTimeState = new MtcLaneState(m_parentState, m_pTable);
    m_overTimeState->assignProperty(this, "entryInfo", false);

    m_noInvState = new MtcLaneState(m_parentState, m_pMessage);
    m_noInvState->assignProperty(this, "entryInfo", false);

    m_showFeeState = new MtcLaneState(m_parentState, m_pVehInfo);
    m_showFeeState->assignProperty(this, "entryInfo", true);

    m_noCardState = new MtcLaneState(m_parentState, m_pTable);
    m_noCardState->assignProperty(this, "entryInfo", false);

    m_centerState = new MtcLaneState(m_parentState, m_pTable);
    m_centerState->assignProperty(this, "entryInfo", false);

    m_stationState = new MtcLaneState(m_parentState, m_pTable);
    m_stationState->assignProperty(this, "entryInfo", false);

    m_prtFailState = new MtcLaneState(m_parentState, m_pTable);
    m_prtFailState->assignProperty(this, "entryInfo", false);

    m_prtingState = new MtcLaneState(m_parentState, m_pVehInfo);
    m_prtingState->assignProperty(this, "entryInfo", true);

    m_calChangeState = new MtcLaneState(m_parentState, m_pCalChange);
    m_calChangeState->assignProperty(this, "entryInfo", false);

    m_uTypeState = new MtcLaneState(m_parentState, m_pTable);
    m_uTypeState->assignProperty(this, "entryInfo", false);

    m_paperState = new MtcLaneState(m_parentState, m_pPaperCard);
    m_paperState->assignProperty(this, "entryInfo", true);

    m_badTypeState = new MtcLaneState(m_parentState, m_pTable);
    m_badTypeState->assignProperty(this, "entryInfo", false);

    m_badNumState = new MtcLaneState(m_parentState, m_pBadCard);
    m_badNumState->assignProperty(this, "entryInfo", true);

    m_finReadState = new MtcLaneState(m_parentState, NULL);

    m_otherFeeState = new MtcLaneState(m_parentState, m_pTable);
    m_otherFeeState->assignProperty(this, "entryInfo", false);

    m_rushFeeState = new MtcLaneState(m_parentState, m_pTable);
    m_rushFeeState->assignProperty(this, "entryInfo", false);

    m_farmState = new MtcLaneState(m_parentState, m_pTable);
    m_farmState->assignProperty(this, "entryInfo", false);

    m_farmFeeState = new MtcLaneState(m_parentState, m_pTable);
    m_farmFeeState->assignProperty(this, "entryInfo", false);

}

void FormExit::createChildrenWidget()
{
    FormBase::createChildrenWidget();
    m_pInvoice = new ForminformId(m_widget, m_layout);
    m_pModifyWeight = new FormModifyWeight(m_widget, m_layout);
    m_pPassCard = new FormPassCard(m_widget, m_layout);
    m_pBadCard = new FormBadCard(m_widget, m_layout);
    m_pCalChange = new FormCalChange(m_widget, m_layout);
    m_pPaperCard = new FormPaperCard(m_widget, m_layout);
    getModifyWeight(m_widget, m_layout);
    getPassCardWgt(m_widget, m_layout);
    getWtDetail(m_widget, m_layout);
}

void FormExit::buildSubStateMachine()
{
    FormBase::buildSubStateMachine();
    //修改车牌
    //用户刷卡
    TranReadCard *tReadCard = new TranReadCard(m_pVehInfo, SIGNAL(detectCardSuccess()));
    tReadCard->setTargetState(m_finReadState);
    m_initState->addTransition(tReadCard);
    //用户刷卡完成判断U型
    TranJudgeUType * tJudgeU = new TranJudgeUType(m_finReadState, SIGNAL(entered()));
    tJudgeU->setTargetState(m_uTypeState);
    m_finReadState->addTransition(tJudgeU);
    //U型免费
    m_uTypeState->addTransition(new TranUTypeFree(m_finReadState));
    //U型选站
    m_uTypeState->addTransition(new TranUTypeChsSta(m_centerState));
    //U型按时间计算
    m_uTypeState->addTransition(new TranUTypeAuto(m_finReadState));
    //用户刷卡首先判断是否为U型车，如果不为U型车，则按照普通车辆计算
    QSignalTransition* tNormal = new TranFinRead(m_finReadState, SIGNAL(entered()));
    tNormal->setTargetState(m_finState);
    m_finReadState->addTransition(tNormal);
    //用户坏卡键跳转选择坏卡类型状态
    m_initState->addTransition(new TranChooseBadCardType(m_badTypeState));
    //用户选择完成坏卡类型后，回车跳转输入坏卡卡号
    m_badTypeState->addTransition(new TranEnterCardNum(m_badNumState));
    //输入完成坏卡卡号后，用户回车验证通过后跳转选择入口站
    m_badNumState->addTransition(new TranChooseEntrySta(m_centerState));
    //初始化跳转到纸卡操作
    m_initState->addTransition(new TranPaperCard(m_paperState));
    //纸卡界面取消跳到初始界面
    m_paperState->addTransition(new SpecialKeySignalTransition(m_initState,KeyEsc,KC_Func));
    //纸卡卡号输入错误，跳转到初始页面
    TranPaperCardErr* tPaperError = new TranPaperCardErr(m_pPaperCard,
                                                         SIGNAL(entryStationWrong()));
    tPaperError->setTargetState(m_initState);
    m_paperState->addTransition(tPaperError);
    //纸卡卡号输入正确，跳转到显示收费状态
    TranPaperCardSuccess * tPaperSuccess = new TranPaperCardSuccess(m_pPaperCard,
                                                                    SIGNAL(entryStationRight()));
    tPaperSuccess->setTargetState(m_finReadState);
    m_paperState->addTransition(tPaperSuccess);
    //初始化跳转无卡操作
    m_initState->addTransition(new TranNoCard(m_noCardState));
    //返回
    m_noCardState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));
    //无卡选择收费站
    m_noCardState->addTransition(new TranChooseCenter(m_centerState));
    //无卡选择站
    m_centerState->addTransition(new TranChooseSta(m_stationState));
    //无卡选择完成收费战后，跳转收费显示状态
    m_stationState->addTransition(new TranShowFeeByConfirm(m_finReadState));
    //收费界面，用户选择云通卡过车，车道放行，等待过车
    m_showFeeState->addTransition(new TranYunTongCard(m_initState));
    //收费界面，用户选择通行卡过车，打印
    TranCashHandle* tCash = new TranCashHandle(m_prtingState);
    m_showFeeState->addTransition(tCash);
    //收费页面按特殊车辆进行减免
    m_showFeeState->addTransition(new TranSpecialVeh(m_finState));

    //收费界面其他免征
    m_showFeeState->addTransition(new TranOtherFree(m_otherFeeState));
    m_otherFeeState->addTransition(new TranUpdateFeeByOtherFree(m_finState));
    m_otherFeeState->addTransition(new SpecialKeySignalTransition(m_showFeeState, KeyEsc, KC_Func));

    //初始界面绿通
    m_initState->addTransition(new TranHandleFarm(m_farmState));
    m_farmState->addTransition(new TranConfirmFarm(m_initState));
    m_farmState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));

    //收费时绿通
    m_showFeeState->addTransition(new TranHandleFarm(m_farmFeeState));
//    m_farmFeeState->addTransition(new TranConfirmFarm(m_showFeeState));
     m_farmFeeState->addTransition(new TranConfirmFarm(m_finState));
    m_farmFeeState->addTransition(new SpecialKeySignalTransition(m_showFeeState, KeyEsc, KC_Func));

    //收费页面按ESC重新返回初始状态
    m_showFeeState->addTransition(new TranReturnInit(m_initState));

    //选择其他免征跳回收费页面
    //chooseOtherState->addTransition(new TranUpdateFeeByOtherFree(finEnterState));

    //打印成功，抬杆放行，车道等待过车
    TranPrintSuccess * tPrintSuccess = new TranPrintSuccess(getDeviceFactory()->getPrinter(),SIGNAL(printSuccess()));
    tPrintSuccess->setTargetState(m_initState);
    m_prtingState->addTransition(tPrintSuccess);

    TranFreePass * tFree = new TranFreePass(tCash, SIGNAL(freePass()));
    tFree->setTargetState(m_initState);
    m_prtingState->addTransition(tFree);

    //打印失败，选择打印失败对应操作
    TranPrintFail* tPrintFail = new TranPrintFail(getDeviceFactory()->getPrinter(),SIGNAL(printFail()));
    tPrintFail->setTargetState(m_prtFailState);
    m_prtingState->addTransition(tPrintFail);

    //打印发票失败，不再打印，等待过车
    m_prtFailState->addTransition(new TranCancelPrint(m_initState));

    //打印失败重新打印
    m_prtFailState->addTransition(new TranRePrint(m_prtingState));

    //收费页面到计算找零
    m_showFeeState->addTransition(new SpecialKeySignalTransition(m_calChangeState, KeyRight, KC_Func));

    //计算找零到收费页面
    m_calChangeState->addTransition(new SpecialKeySignalTransition(m_showFeeState, KeyConfirm, KC_Func));

    //坏卡类型选择 -> 返回初始状态
    m_badTypeState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));

    //纸卡输入 -> 到初始状态
    m_paperState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));

    //坏卡输入 返回初始状态
    m_badNumState->addTransition(new TranReturnInit(m_initState));

    //    //缺票时可以跳转下班状态
    //    noInvoiceState->addTransition(new TranLogOut(next));

    //用户输入完成后判断是否超时
    MtcKeySignalTransition* tOverTime = new TranJudgeUType(m_finState, SIGNAL(entered()));
    tOverTime->setTargetState(m_overTimeState);
    m_finState->addTransition(tOverTime);

    //用户完成输入后自动计算金额
    TranCalcFee* tCalcFee = new TranCalcFee(m_finState, SIGNAL(entered()));
    tCalcFee->setTargetState(m_showFeeState);
    m_finState->addTransition(tCalcFee);

    //计算错误，返回初始
    MtcKeySignalTransition* tErrorCalc = new TranErrorReInit(tCalcFee, SIGNAL(calcFail()));
    tErrorCalc->setTargetState(m_initState);
    m_showFeeState->addTransition(tErrorCalc);

    //锁杆流程
    //    m_initState->addTransition(new TranLockBar(m_initState));
    m_showFeeState->addTransition(new TranLockBar(m_showFeeState));

    //上班状态下调整雨棚灯
    m_initState->addTransition(new TranControlCanLight(m_initState));
    m_showFeeState->addTransition(new TranControlCanLight(m_showFeeState));
    //包交证过车
    m_showFeeState->addTransition(new TranOffical(m_initState));
    //拖车处理
    m_initState->addTransition(new TranTract(m_initState));
    m_showFeeState->addTransition(new TranTract(m_showFeeState));

    //显示收费时闯关
    m_showFeeState->addTransition(new TranViolation(m_rushFeeState));
    m_rushFeeState->addTransition(new TranFinViolation(m_showFeeState));
    m_rushFeeState->addTransition(new SpecialKeySignalTransition(m_showFeeState, KeyEsc, KC_Func));
    //换卡处理
    m_showFeeState->addTransition(new TranChangeCard(m_showFeeState));
    m_initState->addTransition(new TranWeightDetail(m_initState));
    //废票处理
    m_initState->addTransition(new TranDisInvoice(m_initState));
}
