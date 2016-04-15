#ifndef FORMACCREDIT_H
#define FORMACCREDIT_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
    class Formaccredit;
}

class Formaccredit : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit Formaccredit(QWidget *parent = 0,QLayout *layout=0);
    ~Formaccredit();

private:
    Ui::Formaccredit *ui;
};

#endif // FORMACCREDIT_H
