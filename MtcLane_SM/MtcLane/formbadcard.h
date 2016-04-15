#ifndef FORMBADCARD_H
#define FORMBADCARD_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormBadCard;
}

class FormBadCard : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormBadCard(QWidget *parent = 0,QLayout * layout = 0);
    virtual ~FormBadCard();
    //用户输入的长度是否足够
    bool lengthIsEnough();
    //坏卡卡号
    QString badCardNum();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *event);
private:
    Ui::FormBadCard *ui;
};

#endif // FORMBADCARD_H
