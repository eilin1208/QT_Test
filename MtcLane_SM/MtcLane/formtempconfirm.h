#ifndef FORMTEMPCONFIRM_H
#define FORMTEMPCONFIRM_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormTempConfirm;
}

class FormTempConfirm : public MtcOpWidget
{
    Q_OBJECT
signals:
    void cancel();
public:
    explicit FormTempConfirm(QWidget *parent=0, QLayout *layout=0);
    ~FormTempConfirm();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    Ui::FormTempConfirm *ui;
};

#endif // FORMTEMPCONFIRM_H
