#include "errortransition.h"
#include "MtcKey/MtcLaneState.h"
#include "formwarning.h"
bool ErrorTransition::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
        return false;
    }
    QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);

    if (se->arguments().count()!=1)
    {
        qDebug() << "  "+this->objectName()+": se->arguments().count()!=1";
        return false;
    }
    m_sError = se->arguments().at(0).value<QString>();
    return true;
}
void ErrorTransition::onTransition(QEvent *event)
{
    //((MtcLaneState*)targetState())->GetWindowPtr()->setProperty("error", m_sError);
    //FormWarning *pWarning =(FormWarning*)FormMgr::getFormMgr()->getForm(FormMgr::FormWarning);
    //pWarning->setError(m_sError);
}
