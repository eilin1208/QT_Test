#include "formentry.h"

FormEntry::FormEntry(QWidget *widget, QLayout *layout) :
    FormBase(widget, layout)
{
    m_Reverse = NULL;
}

void FormEntry::createStates()
{
    FormBase::setShowEntry(false);
    //调用基类构造
    FormBase::createStates();


}
void FormEntry::createChildrenWidget()
{
    FormBase::createChildrenWidget();
    //倒车
    getEntryReverse(m_widget,m_layout);

}
void FormEntry::buildSubStateMachine()
{
     FormBase::buildSubStateMachine();
     //完成状态放行
     TranEntryAllowPass* tPass = new TranEntryAllowPass(m_finState, SIGNAL(entered()));
     tPass->setTargetState(m_initState);
     m_finState->addTransition(tPass);
     //纸卡处理
     m_initState->addTransition(new TranEntryPaperCard(m_finState));
     //倒车处理
     m_initState->addTransition(new TranEntryReverse(m_Reverse));
     //写卡处理
     TranWriteCard* tWriteCard = new TranWriteCard(m_pVehInfo, SIGNAL(detectCardSuccess()));
     tWriteCard->setTargetState(m_finState);
     m_initState->addTransition(tWriteCard);
     //补卡处理
     m_funcState->addTransition(new TranRepairCard(m_initState));

}




