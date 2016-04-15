#ifndef FORMIDTICKET_H
#define FORMIDTICKET_H

#include <QWidget>
#include"MtcKey/MtcOpWidget.h"
#include"MtcKey/MtcKeySignalTransition.h"
#include"MtcKey/MtcLaneState.h"
#include"QAbstractState"
namespace Ui {
class FormIdTicket;
}

class FormIdTicket : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit FormIdTicket(QWidget *parent = 0,QLayout *layout=0);
    ~FormIdTicket();

private:
    Ui::FormIdTicket *ui;
};
class LoadIdTkTration :public MtcKeySignalTransition
{Q_OBJECT
public:
    LoadIdTkTration(QAbstractState *target):MtcKeySignalTransition( target)
    {

    }

};

#endif // FORMIDTICKET_H
