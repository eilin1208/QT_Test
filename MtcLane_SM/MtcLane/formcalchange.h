#ifndef FORMCALCHANGE_H
#define FORMCALCHANGE_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormCalChange;
}

class FormCalChange : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormCalChange(QWidget *parent = 0, QLayout* layout = 0);
    virtual ~FormCalChange();
protected:
    void showEvent(QShowEvent *event);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    Ui::FormCalChange *ui;
};

#endif // FORMCALCHANGE_H
