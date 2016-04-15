#include "formenter.h"
#include "ui_formenter.h"

FormEnter::FormEnter(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormEnter)
{
    ui->setupUi(this);
}

FormEnter::~FormEnter()
{
    delete ui;
}
