#include "formordinartprocess.h"
#include "ui_formordinartprocess.h"
#include <QTextEdit>
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include "devicefactory.h"
#include "formmainwindow.h"
#include "MtcKey/MtcKeySignalTransition.h"
#include "MtcKey/MtcLaneState.h"
#include "statemachinetransition.h"
#include "dialogshowproinfo.h"
#include "formwtdetail.h"
FormOrdinaryProcess* g_ord = NULL;
FormOrdinaryProcess* getOrdinaryWidget()
{
    return g_ord;
}
FormOrdinaryProcess::FormOrdinaryProcess(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormOrdinartProcess)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    buildChildrenWidget();
    g_ord = this;
    //    characterList.clear();
    //    m_inPut.clear();
    //ui->labCharacter->clear();
    //连接卡信号
    //connect(getCardReader(),SIGNAL(hasOneCard()),this,SLOT(ReceiveCardsignal()));
    //连接io线圈信号
    //connect(getDeviceFactory()->getIOBoard(),SIGNAL(BackLoopChanged(int)),this,SLOT(ReceiveRingsignal(int)));
}

FormOrdinaryProcess::~FormOrdinaryProcess()
{
    //qDebug() << "release ....... " << objectName();
    delete ui;
}

////清除界面所有信息
void FormOrdinaryProcess::clearInfo()
{
    //车型窗口信息清空
    ui->labEnterTime->clear();
    ui->labEnterPlate->clear();
    ui->labEnterStation->clear();
    ui->labCardNum->clear();
    ui->labWorkNum->clear();
    ui->labDriveTime->clear();
    ui->labFlagStation->clear();
}

void FormOrdinaryProcess::setEntryInfo(const QDateTime &entryTime, const QString &sPlate, const QString &sStation, const QString &sCardNo, const QString &sWorkNo, const QString &driveTime, const QString &sFlag)
{
    ui->labEnterTime->setText(entryTime.toString("yyyy-MM-dd hh:mm:ss"));
    ui->labDriveTime->setText(driveTime);
    ui->labEnterPlate->setText(sPlate);
    ui->labWorkNum->setText(sWorkNo);
    ui->labCardNum->setText(sCardNo);
    ui->labFlagStation->setText(sFlag);
    ui->labEnterStation->setText(sStation);
}

void FormOrdinaryProcess::hideEntryInfo()
{
    ui->widget->hide();
}

void FormOrdinaryProcess::showEntryInfo()
{
    ui->widget->show();
}

void FormOrdinaryProcess::buildSubStateMachine(QState *parent, QState* next)
{
    //上班后初始状态，等待用户输入信息
    QState* initState = new MtcLaneState(parent, m_pVehInfo);
    initState->assignProperty(this, "entryInfo", true);

    //用户输入完成所有车辆信息后正常计算费率
    QState* finEnterState = new MtcLaneState(parent, m_pVehInfo);
    finEnterState->assignProperty(this, "entryInfo", true);

    //用户输入完成所有车辆信息后超时选择超时类型
    QState* chooseOverTimeState = new MtcLaneState(parent, m_pTable);
    chooseOverTimeState->assignProperty(this, "entryInfo", false);

    //显示功能状态,功能页面
    QState * showfunction=new MtcLaneState(parent,m_pTable);
    showfunction->assignProperty(this, "entryInfo", false);
    TranFunction *TurnTofunction=new TranFunction(showfunction);
    initState->addTransition(TurnTofunction);
    showfunction->addTransition(new TranLongVeh(initState));
    //返回
    showfunction->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //功能页面跳转到设置页面
    QState *Installstate=new MtcLaneState(parent,m_pTable);
    TranIstallToTable *ToInstall=new TranIstallToTable(Installstate);
    showfunction->addTransition(ToInstall);
    //8更换票据流程-------------------------------------------

    TranRepBill *repbill = new TranRepBill(initState);
    showfunction->addTransition(repbill);

    //返回
    Installstate->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //设置内容页面跳转到车辆信息页面
    TranSetToCarInfo *settocar=new TranSetToCarInfo(initState);
    Installstate->addTransition(settocar);
    //设置内容页跳转到设置页面
    TranIstallToTable *TurnInstall=new TranIstallToTable(showfunction);
    Installstate->addTransition(TurnInstall);



    //跳转到编辑计重页面
    //    QState *showAcceditTransition=new MtcLaneState(parent,m_pmendweight);
    //    showAcceditTransition->assignProperty(this, "entryInfo", false);
    //    TranAccedit *enterAccedit=new TranAccedit(showAcceditTransition);
    //    showfunction->addTransition(enterAccedit);
    //TODO_liubox
    TranAccedit * enterAccedit = new TranAccedit(initState);
    showfunction->addTransition(enterAccedit);
    //返回TODO_liubo
    //   showAcceditTransition->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //计重页面跳转到车辆信息页TODO_liubo
    //    QState * showAcceditTransition = new MtcLaneState(parent,getAmendweight());
    //    showAcceditTransition->assignProperty(this, "entryInfo", false);
    //    showAcceditTransition->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));
    //    TranWeightToTable *WeightToTable=new TranWeightToTable(initState);
    //    showAcceditTransition->addTransition(WeightToTable);
    //重连读卡器
    TranCardToTable * reconnectcard = new TranCardToTable(initState);
    showfunction->addTransition(reconnectcard);
    //更换卡盒
    //    repcardboxconfirm->assignProperty(this,"entryInfo",true);
    TranRepCardBox * repcardbox = new TranRepCardBox(initState);
    showfunction->addTransition(repcardbox);
    //显示通行费状态，限时通行费页面
    QState* showFeeState = new MtcLaneState(parent, m_pVehInfo);
    showFeeState->assignProperty(this, "entryInfo", true);
    //初始状态选择其他免征
    QState* initOtherState = new MtcLaneState(parent,m_pTable);
    initOtherState->assignProperty(this, "entryInfo", false);
    //无卡状态，显示无卡选择界面
    QState* showNoCardState = new MtcLaneState(parent, m_pTable);
    showNoCardState->assignProperty(this, "entryInfo", false);
    //无卡选择中心状态
    QState* showCenterState = new MtcLaneState(parent, m_pTable);
    showCenterState->assignProperty(this, "entryInfo", false);
    //无卡选择收费站状态
    QState* showStaState = new MtcLaneState(parent, m_pTable);
    showStaState->assignProperty(this, "entryInfo", false);
    //    //放行状态
    //    QState* waitPassState = new MtcLaneState(parent, m_pVehInfo);
    //    waitPassState->assignProperty(this, "entryInfo", true);

    //票据打印失败状态
    QState* printFailState = new MtcLaneState(parent, m_pTable);
    printFailState->assignProperty(this, "entryInfo", false);
    //打印状态
    QState* printingState = new MtcLaneState(parent, m_pVehInfo);
    //选择其他免征
    QState* chooseOtherState = new MtcLaneState(parent, m_pTable);
    chooseOtherState->assignProperty(this, "entryInfo", false);
    //纸卡处理
    QState* paperCardState = new MtcLaneState(parent, m_pPaperCard);
    paperCardState->assignProperty(this, "entryInfo", true);

    //坏卡类型选择
    QState* chooseBadTypeState = new MtcLaneState(parent, m_pTable);
    chooseBadTypeState->assignProperty(this, "entryInfo", false);
    //坏卡卡号输入
    QState* enterBadNoState = new MtcLaneState(parent, m_pBadCard);
    enterBadNoState->assignProperty(this, "entryInfo", true);

    //找零计算
    QState* calChangeState = new MtcLaneState(parent, m_pCalChange);
    calChangeState->assignProperty(this, "entryInfo", false);
    //init->enterplate
    TranEnterPlate* tEnterPlate = new TranEnterPlate(initState);
    initState->addTransition(tEnterPlate);

//    //用户F6跳转到车牌输入页面
//    TranFinEnterPlate* tFinishPlate = new TranFinEnterPlate(initState);
//    enterPlateState->addTransition(tFinishPlate);

    //init->showFee
    //    ShowFeeTransition* tShowFee = new ShowFeeTransition(getCardReader(), SIGNAL(hasOneCard()));
    //    tShowFee->setTargetState(showFeeState);
    //    initState->addTransition(tShowFee);
    //用户刷卡
    QState* finRead = new MtcLaneState(parent, NULL);
    TranReadCard *tReadCard = new TranReadCard(m_pVehInfo, SIGNAL(detectCardSuccess()));
    tReadCard->setTargetState(finRead);
    initState->addTransition(tReadCard);

    //用户刷卡完成判断U型
    QState* uTypeState = new MtcLaneState(parent, m_pTable);
    uTypeState->assignProperty(getOrdinaryWidget(), "entryInfo", false);
    TranJudgeUType * tJudgeU = new TranJudgeUType(finRead, SIGNAL(entered()));
    tJudgeU->setTargetState(uTypeState);
    finRead->addTransition(tJudgeU);

    //U型免费
    TranUTypeFree* tUFree = new TranUTypeFree(finRead);
    uTypeState->addTransition(tUFree);

    //U型选站
    TranUTypeChsSta* tUChs = new TranUTypeChsSta(showCenterState);
    uTypeState->addTransition(tUChs);

    //U型按时间计算
    TranUTypeAuto* tUAuto = new TranUTypeAuto(finRead);
    uTypeState->addTransition(tUAuto);

    //用户刷卡首先判断是否为U型车，如果不为U型车，则按照普通车辆计算
    QSignalTransition* tNormal = new TranFinRead(finRead, SIGNAL(entered()));
    tNormal->setTargetState(finEnterState);
    finRead->addTransition(tNormal);

    //用户坏卡键跳转选择坏卡类型状态
    initState->addTransition(new TranChooseBadCardType(chooseBadTypeState));

    //用户选择完成坏卡类型后，回车跳转输入坏卡卡号
    chooseBadTypeState->addTransition(new TranEnterCardNum(enterBadNoState));

    //输入完成坏卡卡号后，用户回车验证通过后跳转选择入口站
    enterBadNoState->addTransition(new TranChooseEntrySta(showCenterState));

    //初始化跳转到纸卡操作
    initState->addTransition(new TranPaperCard(paperCardState));
    //纸卡界面取消跳到初始界面
    paperCardState->addTransition(new SpecialKeySignalTransition(initState,KeyEsc,KC_Func));
    //纸卡卡号输入错误，跳转到初始页面
    TranPaperCardErr* tPaperError = new TranPaperCardErr(m_pPaperCard,
                                                         SIGNAL(entryStationWrong()));

    tPaperError->setTargetState(initState);
    paperCardState->addTransition(tPaperError);

    //纸卡卡号输入正确，跳转到显示收费状态
    TranPaperCardSuccess * tPaperSuccess = new TranPaperCardSuccess(m_pPaperCard,
                                                                    SIGNAL(entryStationRight()));
    tPaperSuccess->setTargetState(finRead);
    paperCardState->addTransition(tPaperSuccess);

    //初始化跳转无卡操作
    TranNoCard * tBadCard = new TranNoCard(showNoCardState);
    initState->addTransition(tBadCard);
    //返回
    showNoCardState->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //无卡选择收费站
    TranChooseCenter * tChooseCenter = new TranChooseCenter(showCenterState);
    showNoCardState->addTransition(tChooseCenter);

    //无卡选择站
    TranChooseSta * tChooseSta = new TranChooseSta(showStaState);
    showCenterState->addTransition(tChooseSta);

    //无卡选择完成收费战后，跳转收费显示状态
    showStaState->addTransition(new TranShowFeeByConfirm(finRead));

    //NoCard-> showFee
    //showNoCardState->addTransition(new NoCardMaxDisTransition(showFeeState));

    //收费界面，用户选择云通卡过车，车道放行，等待过车
    showFeeState->addTransition(new TranYunTongCard(initState));

    //收费界面，用户选择通行卡过车，打印
    TranCashHandle* tCash = new TranCashHandle(printingState);
    showFeeState->addTransition(tCash);

    //收费页面按特殊车辆进行减免
    showFeeState->addTransition(new TranSpecialVeh(finEnterState));

    //收费页面按其他免征选择其他免征
    showFeeState->addTransition(new TranOtherFree(chooseOtherState));

    //返回
    chooseOtherState->addTransition(new SpecialKeySignalTransition(showFeeState, KeyEsc, KC_Func));

    //初始化页面按特殊车辆进行减免
    initState->addTransition(new TranSpecialVeh(initState));

    //初始化页面按其他免征选择其他免征
    initState->addTransition(new TranOtherFree(initOtherState));
    //返回
    initOtherState->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    QState* farmTypeState = new MtcLaneState(parent, m_pTable);
    //初始化页面按绿通卡键选择绿通卡类型
    initState->addTransition(new TranHandleFarm(farmTypeState));
    farmTypeState->assignProperty(getOrdinaryWidget(), "entryInfo", false);

    //绿通卡界面跳转到初始界面
    farmTypeState->addTransition(new TranConfirmFarm(initState));

    //收费页面按绿通卡键选择绿通卡类型
    QState* farmTypeFeeState = new MtcLaneState(parent, m_pTable);

    showFeeState->addTransition(new TranHandleFarm(farmTypeFeeState));
    farmTypeFeeState->assignProperty(getOrdinaryWidget(), "entryInfo", false);

    //绿通卡界面跳转到收费界面
    farmTypeFeeState->addTransition(new TranConfirmFarm(finEnterState));

    //选择其他免征跳回初始化页面
    initOtherState->addTransition(new TranUpdateFeeByOtherFree(initState));

    //收费页面按ESC重新返回初始状态
    showFeeState->addTransition(new TranReturnInit(initState));

    //选择其他免征跳回收费页面
    chooseOtherState->addTransition(new TranUpdateFeeByOtherFree(finEnterState));

    //打印成功，抬杆放行，车道等待过车
    TranPrintSuccess * tPrintSuccess = new TranPrintSuccess(getDeviceFactory()->getPrinter(),SIGNAL(printSuccess()));
    tPrintSuccess->setTargetState(initState);
    printingState->addTransition(tPrintSuccess);

    TranFreePass * tFree = new TranFreePass(tCash, SIGNAL(freePass()));
    tFree->setTargetState(initState);
    printingState->addTransition(tFree);


    //缺票状态，当打印完成后，会判断是否还有发票，如果没有发票了则进入该状态，等待下班换上发票
    QState* noInvoiceState = new MtcLaneState(parent, m_pMessage);
    //    TranCheckInvNum* tCheck = new TranCheckInvNum(getDeviceFactory()->getPrinter(),
    //                                                                          SIGNAL(printSuccess()));
    //    tCheck->setTargetState(noInvoiceState);
    //    waitPassState->addTransition(tCheck);

    //打印失败，选择打印失败对应操作
    TranPrintFail* tPrintFail = new TranPrintFail(getDeviceFactory()->getPrinter(),SIGNAL(printFail()));
    tPrintFail->setTargetState(printFailState);
    printingState->addTransition(tPrintFail);

    //打印发票失败，不再打印，等待过车
    printFailState->addTransition(new TranCancelPrint(initState));

    //打印失败重新打印
    printFailState->addTransition(new TranRePrint(printingState));

    //收费页面到计算找零
    SpecialKeySignalTransition* tCalChange = new SpecialKeySignalTransition(calChangeState, KeyRight, KC_Func);
    showFeeState->addTransition(tCalChange);

    //计算找零到收费页面
    SpecialKeySignalTransition* tReturnShowFee = new SpecialKeySignalTransition(showFeeState, KeyConfirm, KC_Func);
    calChangeState->addTransition(tReturnShowFee);

    //模拟完成过车
    //    TranFinPassVehSim* tFinishPassSim = new TranFinPassVehSim(initState);
    //    waitPassState->addTransition(tFinishPassSim);

    //后线圈无车，返回初始状态
    //    TranFinPassVeh* tFinishPass = new TranFinPassVeh(getDeviceFactory()->getIOBoard(),
    //                                                     SIGNAL(InputChanged(DIGITAL_IN,quint8)));
    //    tFinishPass->setTargetState(initState);
    //    waitPassState->addTransition(tFinishPass);
    //坏卡类型选择 -> 返回初始状态
    chooseBadTypeState->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //纸卡输入 -> 到初始状态
    paperCardState->addTransition(new SpecialKeySignalTransition(initState, KeyEsc, KC_Func));

    //坏卡输入 返回初始状态
    enterBadNoState->addTransition(new TranReturnInit(initState));
    //车队流程
    //车队开始状态
    QState *queStart = new MtcLaneState(parent,m_QueBegin);
    queStart->assignProperty(getOrdinaryWidget(), "entryInfo", false);
    TranMotor * tMotor = new TranMotor(queStart);
    initState->addTransition(tMotor);
    QSignalTransition* tMotorPass = new TranMotorPass(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)));
    tMotorPass->setTargetState(queStart);
    queStart->addTransition(tMotorPass);
    //结束车队操作
    TranVehQueEnd *tend = new TranVehQueEnd(initState);
    queStart->addTransition(tend);

    //确认下班状态
    QState* confirmLogOutState = new MtcLaneState(parent, m_pMessage);
    confirmLogOutState->assignProperty(this, "entryInfo", false);
    initState->addTransition(new TranLogOut(next));

    //缺票时可以跳转下班状态
    noInvoiceState->addTransition(new TranLogOut(next));

    //用户输入完成后判断是否超时
    MtcKeySignalTransition* tOverTime = new TranJudgeUType(finEnterState, SIGNAL(entered()));
    tOverTime->setTargetState(chooseOverTimeState);
    finEnterState->addTransition(tOverTime);

    //    //用户选择完超时类型后，根据用户选择跳转正常超时计算，显示收费
    //    MtcKeySignalTransition* tNormalOverTime = new TranCalcOverTimeNormal(showFeeState);
    //    chooseOverTimeState->addTransition(tNormalOverTime);

    //    //用户选择可疑超时后，跳转监控授权界面等待授权
    //    MtcKeySignalTransition* tExcOverTime = new TranExcOverTime(showFeeState);
    //    chooseOverTimeState->addTransition(tExcOverTime);

    //用户完成输入后自动计算金额
    TranCalcFee* tCalcFee = new TranCalcFee(finEnterState, SIGNAL(entered()));
    tCalcFee->setTargetState(showFeeState);
    finEnterState->addTransition(tCalcFee);

    //计算错误，返回初始
    MtcKeySignalTransition* tErrorCalc = new TranErrorReInit(tCalcFee, SIGNAL(calcFail()));
    tErrorCalc->setTargetState(initState);
    showFeeState->addTransition(tErrorCalc);


    parent->setInitialState(initState);
    //锁杆流程
    //锁杆/解锁
    //    TranLockBar * lockbarTransConfirm = new TranLockBar(waitPassState);
    //    waitPassState->addTransition(lockbarTransConfirm);
    TranLockBar * lockbarTrans = new TranLockBar(initState);
    initState->addTransition(lockbarTrans);
    TranLockBar * lockbarShowfeeTrans = new TranLockBar(showFeeState);
    showFeeState->addTransition(lockbarShowfeeTrans);

    //上班状态下调整雨棚灯
    initState->addTransition(new TranControlCanLight(initState));
    //waitPassState->addTransition(new TranControlCanLight(waitPassState));
    showFeeState->addTransition(new TranControlCanLight(showFeeState));
    //包交证过车
    showFeeState->addTransition(new TranOffical(initState));
    //拖车处理
    initState->addTransition(new TranTract(initState));
    showFeeState->addTransition(new TranTract(showFeeState));

    MtcLaneState* vioState = new MtcLaneState(parent, m_pTable);
    vioState->assignProperty(getOrdinaryWidget(), "entryInfo", false);
    MtcLaneState* feeVioState = new MtcLaneState(parent, m_pTable);
    feeVioState->assignProperty(getOrdinaryWidget(), "entryInfo", false);
    initState->addTransition(new TranViolation(vioState));
    showFeeState->addTransition(new TranViolation(feeVioState));
    vioState->addTransition(new TranFinViolation(initState));
    feeVioState->addTransition(new TranFinViolation(showFeeState));
    //换卡处理
    showFeeState->addTransition(new TranChangeCard(showFeeState));
    initState->addTransition(new TranWeightDetail(initState));
    //废票处理
    initState->addTransition(new TranDisInvoice(initState));
}

void FormOrdinaryProcess::buildChildrenWidget()
{
    m_layout = new QVBoxLayout(ui->widget_2);
    //m_pPlate = new FormEntryPlate(ui->widget_2, m_layout);
    getShowEnPlate(ui->widget_2,m_layout);
    m_pVehInfo = new FormEntryVehInfo(ui->widget_2, m_layout);
    m_pPaperCard = new FormPaperCard(ui->widget_2, m_layout);
    m_pTable = new FormTableWidget(ui->widget_2, m_layout);
    m_pBadCard = new FormBadCard(ui->widget_2, m_layout);
    m_pCalChange = new FormCalChange(ui->widget_2, m_layout);
    // m_pAccedit=new Formaccredit(ui->widget_2,m_layout);
    //TODO_liubo 注掉
    //    m_pmendweight=new Formamendweight(ui->widget_2,m_layout);
    getModifyWeight(ui->widget_2,m_layout);
    //车队
    m_pMessage = new FormMessage(ui->widget_2,m_layout);
    m_pAuthByCap = new FormPasswordVerify(ui->widget_2,m_layout);
    m_QueBegin = new FormVehQueBegin(ui->widget_2,m_layout);
    //m_informTK = new ForminformId(ui->widget_2,m_layout);
    getPassCardWgt(ui->widget_2, m_layout);
    getShowProInfo(ui->widget_2, m_layout);
    getWtDetail(ui->widget_2, m_layout);
}


