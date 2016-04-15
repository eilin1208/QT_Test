#ifndef FORMAUTOCARD_H
#define FORMAUTOCARD_H
#include "formbase.h"
#include "formautovehinfo.h"
class FormAutoCard : public FormBase
{
    Q_OBJECT
public:
    explicit FormAutoCard(QWidget *parent = 0, QLayout* layout = 0);
    
signals:
    
public slots:
public:
    void showLogin(bool isLogin)
    {
        m_pAutoInfo->login(isLogin);
    }

protected:
    virtual void createStates();
    virtual void createChildrenWidget();
    virtual void buildSubStateMachine();

private:
    //写卡状态
    QState* m_pWriting;
    //等待取卡
    QState* m_pWaiting;
    //卡机异常状态
    QState* m_pError;
private:
    FormAutoVehInfo* m_pAutoInfo;
};

#endif // FORMAUTOCARD_H
