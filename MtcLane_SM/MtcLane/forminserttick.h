#ifndef FORMINSERTTICK_H
#define FORMINSERTTICK_H

#include <QWidget>
#include"MtcKey/MtcKeySignalTransition.h"
#include"MtcKey/MtcOpWidget.h"
#include"QAbstractState"
namespace Ui {
    class ForminsertTick;
}

class ForminsertTick : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit ForminsertTick(QWidget *parent = 0,QLayout *layout=0);
    virtual ~ForminsertTick();
    QString getEnterPassword();
    void clearPassword();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *event);
private:
    Ui::ForminsertTick *ui;
};

#endif // FORMINSERTTICK_H
