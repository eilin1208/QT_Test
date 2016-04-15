#ifndef LOADPARAMTRANSITION_H
#define LOADPARAMTRANSITION_H
#include "MtcKey/MtcKeySignalTransition.h"
class LoadParamTransition : public MtcKeySignalTransition
{
    Q_OBJECT

public:
    LoadParamTransition(QObject * sender, const char * signal) : MtcKeySignalTransition(sender, signal)
    {
    }
    LoadParamTransition(QAbstractState *target) : MtcKeySignalTransition(target)
    {
    }
signals:

public slots:

protected:
    virtual bool eventTest(QEvent *event);
    virtual void onTransition(QEvent *event);
};

#endif // LOADPARAMTRANSITION_H
