#ifndef FORMPASSCARD_H
#define FORMPASSCARD_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormPassCard;
}

class FormPassCard : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormPassCard(QWidget *parent = 0, QLayout *layout = 0);
    virtual ~FormPassCard();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    void handleEscKey();
    void handleOfficalKey();
private:
    Ui::FormPassCard *ui;
};

FormPassCard* getPassCardWgt(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMPASSCARD_H
