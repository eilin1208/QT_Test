#include "formaccredit.h"
#include "ui_formaccredit.h"

Formaccredit::Formaccredit(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::Formaccredit)
{
    ui->setupUi(this);
    this->setTitle(tr("编辑计重信息请求班长授权"));
}

Formaccredit::~Formaccredit()
{
    delete ui;
}
