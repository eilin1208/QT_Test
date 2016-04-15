#ifndef FORMENTER_H
#define FORMENTER_H
/*
  编写人：王祥凯
  编写时间：2012年11月16日
  类作用：mtc入口上班后主界面，用于操作员输入车型
  */
#include "MtcKey/MtcOpWidget.h"

namespace Ui {
class FormEnter;
}

class FormEnter : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormEnter(QWidget *parent = 0, QLayout* layout = 0);
    ~FormEnter();
    
private:
    Ui::FormEnter *ui;
};

#endif // FORMENTER_H
