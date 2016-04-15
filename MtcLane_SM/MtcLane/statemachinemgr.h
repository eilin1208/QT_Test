#ifndef STATEMACHINEMGR_H
#define STATEMACHINEMGR_H

#include <QObject>
#include <QStateMachine>
#include "MtcKey/MtcLaneState.h"
#include "MtcKey/MtcKeySignalTransition.h"
#include "formmessage.h"
#include "formloadparam.h"
#include "formpasswordverify.h"
#include <QEventLoop>
#include <QTimerEvent>
#include "ParamModule/qparammgr.h"
#include "formtablewidget.h"
#include"forminformid.h"
#include "formbase.h"
#include "formvehqueprocess.h"
#include "formfunction.h"
#include "formdevicetable.h"
class StateMachineMgr : public QObject
{
    Q_OBJECT
public:
    explicit StateMachineMgr(QObject *parent = 0);
    virtual ~StateMachineMgr();
signals:
    void beginState();
public slots:
    void buildStateMachine(QLayout* layout, bool bIsEntryLane);
    void buildEntryWindows(QLayout* layout);
    void buildExitWindows(QLayout* layout);
   // void buildIdTicket();
    void beginInitState();
protected:
    void addRushTranLogOut(QState* state);
private:
    QStateMachine m_machine;
    FormMessage* m_pFormMsg;
    FormLoadParam* m_pFormLoadParam;
    FormPasswordVerify* m_pPassword;
    FormTableWidget* m_pTableWidget;
    //ForminformId *m_informTK;
    FormBase* m_pOrdWidget;
    //车队流程处理状态
    FormVehqueProcess *m_QueProcess;
    FormDeviceTable* m_devTable;
};
StateMachineMgr* getStateMgr();
class LoadParamTransition : public MtcKeySignalTransition
{
 public:
    LoadParamTransition(QObject * sender, const char * signal):MtcKeySignalTransition(sender,signal)
    {

    }
    virtual bool eventTest(QEvent *event);
    virtual void onTransition(QEvent */*event*/);
};
#endif // STATEMACHINEMGR_H
