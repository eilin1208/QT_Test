#ifndef PASSWORDVERIFYTRANSITION_H
#define PASSWORDVERIFYTRANSITION_H

#include <QObject>
#include "MtcKey/MtcKeySignalTransition.h"
#include "ParamModule/qoperparamfile.h"

class PasswordVerifyTransition : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    explicit PasswordVerifyTransition(QObject* sender, const char* signal);
    explicit PasswordVerifyTransition(QState* state);

signals:

public slots:

protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
private:
    bool verifyPassword(const QString& sOperName, const QString& sPassword,
                        QString& sError, SOperInfoValue& operValue);
    void showShiftInfo();
    QString getShiftString(const QString& sShiftName, const QDate &workDate);
};

#endif // PASSWORDVERIFYTRANSITION_H
