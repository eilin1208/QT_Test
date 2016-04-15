#ifndef ERRORTRANSITION_H
#define ERRORTRANSITION_H

#include <QObject>
#include <QString>
#include "MtcKey/MtcKeySignalTransition.h"
class ErrorTransition : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    ErrorTransition(QObject * sender, const char * signal) : MtcKeySignalTransition(sender, signal)
    {
    }
    ErrorTransition(QAbstractState *target) : MtcKeySignalTransition(target)
    {
    }
signals:

public slots:

protected:
    virtual bool eventTest(QEvent *event);
    virtual void onTransition(QEvent *event);
private:
    QString m_sError;
};

#endif // ERRORTRANSITION_H
