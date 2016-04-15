#ifndef FORMEDITWT_H
#define FORMEDITWT_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
    class FormEditWt;
}

class FormEditWt : public MtcOpWidget
{
    Q_OBJECT
signals:
    //获取用户输入的计重和轴数
    void getWeigetAndAxls(int nWeight, int nAxls);
    void getAmendWeigetAxls(int nWeight, int nAxls);
public:
    explicit FormEditWt(QWidget *parent = 0, QLayout* layout = 0);
    virtual ~FormEditWt();

    //处理按键事件
    void handleEvent(MtcKeyPressedEvent* mtcKeyEvent);

protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showText();
    void showEvent(QShowEvent *);
private:
    Ui::FormEditWt *ui;
    bool m_bStep;
    int m_nWeidget;
    int m_nAxlsNum;
};

#endif // FORMEDITWT_H
