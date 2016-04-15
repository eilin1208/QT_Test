#include "formautocard.h"
#include "devicefactory.h"
#include "formlog.h"
FormAutoCard::FormAutoCard(QWidget *parent, QLayout *layout) :
    FormBase(parent, layout)
{
}

void FormAutoCard::createStates()
{
    FormBase::setShowEntry(false);
    m_pWriting = new MtcLaneState(m_parentState, m_pAutoInfo);
    m_pWaiting = new MtcLaneState(m_parentState, m_pAutoInfo);
    m_pError = new MtcLaneState(m_parentState, m_pMessage);
    m_initState = new MtcLaneState(m_parentState, m_pAutoInfo);
    m_motorState = new MtcLaneState(m_parentState, m_pMotor);
    m_funcState = new MtcLaneState(m_parentState, m_pTable);
    m_finState = new MtcLaneState(m_parentState, m_pAutoInfo);
    m_parentState->setInitialState(m_initState);

}

void FormAutoCard::createChildrenWidget()
{
    FormBase::createChildrenWidget();
    m_pAutoInfo = new FormAutoVehInfo(m_widget, m_layout);

}

void FormAutoCard::buildSubStateMachine()
{
    FormLog* pLog = NULL;
    getWidgetByType(emFormLog, (QWidget**)&pLog);
    //车队开始状态
    m_initState->addTransition(new TranAutoMotor(m_motorState));
    //车队过车
    QSignalTransition* tMotorPass = new TranAutoMotorPass(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)));
    tMotorPass->setTargetState(m_motorState);
    m_motorState->addTransition(tMotorPass);
    //结束车队操作
    m_motorState->addTransition(new TranAutoVehQueEnd(m_initState));


    CardMgrArm* pCardMgr = getDeviceFactory()->getCardMgr();

    //正常状态帧
    TranAutoNormal* pNormal = new TranAutoNormal(pCardMgr, SIGNAL(cmNormal()));
    pNormal->setTargetState(m_initState);
    m_pError->addTransition(pNormal);
    //异常状态帧
    TranAutoException* pException = new TranAutoException(pCardMgr, SIGNAL(cmException()));
    pException->setTargetState(m_pError);
    m_initState->addTransition(pException);
    //收到按键事件
    TranAutoWriteSuccess* pWrite = new TranAutoWriteSuccess(pCardMgr, SIGNAL(cardOut(int)));
    pWrite->setTargetState(m_pWaiting);
    m_initState->addTransition(pWrite);
    //用户取走卡片流程
    TranAutoAllowPass* pAllowPass = new TranAutoAllowPass(pCardMgr, SIGNAL(cardAway(int)));
    pAllowPass->setTargetState(m_finState);
    m_pWaiting->addTransition(pAllowPass);

    //写卡处理
    TranAutoWriteCard* nWrite = new TranAutoWriteCard(m_pAutoInfo, SIGNAL(handleProCardSuccess()));
    nWrite->setTargetState(m_finState);
    m_initState->addTransition(nWrite);
    //完成状态放行
    TranEntryAllowPass* tPass = new TranEntryAllowPass(m_finState, SIGNAL(entered()));
    tPass->setTargetState(m_initState);
    m_finState->addTransition(tPass);
    //下班
    m_initState->addTransition(new TranAutoLogOut(m_nextState));
}
