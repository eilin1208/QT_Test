#ifndef MTC_KEY_SIGNAL_TRANSITION_H
#define MTC_KEY_SIGNAL_TRANSITION_H

#include <QStateMachine>
#include <QSignalTransition>
#include <QVariant>
#include <QDebug>
#include "MtcKeyDef.h"
#include "MtcLaneState.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/ilogmsg.h"
class MtcKeySignalTransition : public QSignalTransition
{
    Q_OBJECT

public:
    MtcKeySignalTransition(QObject * sender, const char * signal) : QSignalTransition(sender, signal)
    {
    }

    MtcKeySignalTransition() : QSignalTransition(MtcKeyFilter::instance(), SIGNAL(keyPressed(const MtcKeyPropertity &)))
    {
    }

    MtcKeySignalTransition(QAbstractState *target) : QSignalTransition(MtcKeyFilter::instance(), SIGNAL(keyPressed(const MtcKeyPropertity &)))
    {
        setTargetState(target);
    }
    QWidget* getSourceWidget()
    {
        return ((MtcLaneState*)sourceState())->GetWindowPtr();
    }
    QWidget* getTargetWidget()
    {
        return ((MtcLaneState*)targetState())->GetWindowPtr();
    }

protected:
    virtual bool eventTest (QEvent *event)
    {
        quint32 total;
        getMemoryInfo(&total, &m_freeMemory);
        ////QDebug() << this->objectName()+": eventTest";
        if (!QSignalTransition::eventTest(event))
        {
            ////QDebug() << "  "+this->objectName()+": !QSignalTransition::eventTest(event)";
            return false;
        }
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);
        if (se->arguments().count()!=1)
        {
            ////QDebug() << "  "+this->objectName()+": se->arguments().count()!=1";
            return false;
        }
        m_keyPropertity=se->arguments().at(0).value<MtcKeyPropertity>();
        m_keyType=KC_NONE;
        mtcKeyEventTest(m_keyPropertity, &m_keyType);
        MtcKeyProcessResultRecorder::instance()->doKeyProcessed(m_keyPropertity, m_keyType);
        return (KC_NONE!=m_keyType);
    }

    virtual void onTransition (QEvent *event)
    {
        onMtcKeyTransition(m_keyPropertity, m_keyType);
        quint32 total = 0;
        quint32 freeMemory = 0;
        getMemoryInfo(&total, &freeMemory);
        LogMsg("memory", tr("%1 exec, use memory [%2], free memory is [%3]")
               .arg(metaObject()->className())
               .arg(qint64(m_freeMemory) - qint64(freeMemory))
               .arg(freeMemory));
    }
    // 如果测试成功，要设置keyType；否则认为不处理该按键转换
    virtual void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
    {
        ////QDebug() << "  "+this->objectName() << keyPropertity.key << keyPropertity.keyName;
    }

    virtual void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
    {
    }

private:
    MtcKeyPropertity m_keyPropertity;
    int m_keyType;
    quint32 m_freeMemory;
};


class SpecialKeySignalTransition: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    SpecialKeySignalTransition(QState *target, int key,int keyType, const QString& sError = ""):
        MtcKeySignalTransition(target), m_nKey(key), m_nKeyType(keyType), m_sError(sError)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
private:
    int m_nKey;
    int m_nKeyType;
    QString m_sError;
};





#endif // MTC_KEY_SIGNAL_TRANSITION_H
