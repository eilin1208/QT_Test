#ifndef FORMENTRY_H
#define FORMENTRY_H
#include "formbase.h"
#include "formpapercard.h"
class FormEntry : public FormBase
{
    Q_OBJECT
public:
    FormEntry(QWidget* widget = 0, QLayout* layout = 0);
protected:
    virtual void createStates();
    virtual void createChildrenWidget();
    virtual void buildSubStateMachine();
protected:
    //倒车
    QState * m_Reverse;
private:
    //入口窗口
    //纸卡-entry
private:

};

#endif // FORMENTRY_H
