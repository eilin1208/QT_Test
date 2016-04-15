#ifndef FORMAUTH_H
#define FORMAUTH_H

#include <QWidget>
#include "CommonModule/LaneType.h"
#include "MtcKey/MtcOpWidget.h"
#include "PacketSendModule/monclient.h"
#include "devicefactory.h"
#include "CardReaderArm/qcardreader.h"
namespace Ui {
class FormAuth;
}

class FormAuth : public MtcOpWidget
{
    Q_OBJECT
signals:
    void authFinished();
public:
    explicit FormAuth(QWidget *parent = 0, QLayout* layout = 0);
    virtual ~FormAuth();
    void setMonType(TMonReqID monType);
    void setAuthType(int nType);
public slots:
    void AuthResponse(THKResultInfo info);
    void handleOperCard(quint32 nReadId,int nCardType);
protected:
    void focusInEvent(QFocusEvent *);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *event);
    void clearResult();
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void handleFuncKey(MtcKeyPressedEvent *mtcKeyEvent);
    void handleNumberKey(MtcKeyPressedEvent *mtcKeyEvent);
    bool VerifyAuth(const QString& sOper, const QString& sPwd);
    bool VerifyOperType(const QString& sOper, QString& sError);
    bool VerifyPwd(const QString &sOperName, const QString &sPassword, QString& sError);
    //关闭
    void AuthClose();
    bool verifyBasicInfo(const TOpCardBasicInfo& opCardBasicInfo);
    bool verifyOperCard(quint32 dwOperId, quint32 dwOperCardId);
    void hideEvent(QHideEvent *);
private slots:
    void on_stackedWidget_currentChanged(int arg1);

private:
    int m_nAuthTimerId;     //授权定时器ID
    int m_nCountTimerId;    //秒表倒数定时器ID；
    int m_nCount;
    TMonReqID m_monType;
    int m_nAuthType;
    //liujian
    QString m_nAuthName;
private:
    Ui::FormAuth *ui;
};

FormAuth* getAuth(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMAUTH_H
