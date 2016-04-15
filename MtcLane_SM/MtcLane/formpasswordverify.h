#ifndef FORMPASSWORDVERIFY_H
#define FORMPASSWORDVERIFY_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "ParamModule/qoperparamfile.h"
#include "devicefactory.h"
#include "formmessage.h"
namespace Ui {
    class FormPasswordVerify;
}

class FormPasswordVerify : public MtcOpWidget
{
    Q_OBJECT
public:
    explicit FormPasswordVerify(QWidget *parent=0, QLayout *layout=0);
    virtual ~FormPasswordVerify();
    void clearOperInfo();
    enum emShiftType
    {
        LOGIN,
        LOGOUT
    };

signals:
    //验证失败发送错误信号
    void verifyFail();
public slots:
    void handleOperCard(quint32 /*nCardId*/ ,int nCardType);
    void showType(emShiftType type);
protected:
    void focusInEvent(QFocusEvent *);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void handleNumberKey(MtcKeyPressedEvent* mtcKeyEvent);
    void handleEraserKey(MtcKeyPressedEvent* mtcKeyEvent);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    bool verifyPassword();
    bool verifyBasicInfo(const TOpCardBasicInfo &opCardBasicInfo);
    bool verifyOperCard(quint32 dwOperId, quint32 dwOperCardId);
private:
    SOperInfoValue m_operValue;
    emShiftType m_shiftType;
    Ui::FormPasswordVerify *ui;
    bool operTypeFalse;
    int m_bCount;
};

FormPasswordVerify* getPasswordVerify(QWidget *widget = NULL, QLayout* layout = NULL);
#endif // FORMPASSWORDVERIFY_H
