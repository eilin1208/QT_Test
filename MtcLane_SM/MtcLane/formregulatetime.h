#ifndef FORMREGULATETIME_H
#define FORMREGULATETIME_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormRegulateTime;
}

class FormRegulateTime : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormRegulateTime(QWidget *parent = 0, QLayout *layout = 0);
    ~FormRegulateTime();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    Ui::FormRegulateTime *ui;
};

FormRegulateTime *getRegulateTimeForm(QWidget *widget = NULL, QLayout *layout = NULL);

#endif // FORMREGULATETIME_H
