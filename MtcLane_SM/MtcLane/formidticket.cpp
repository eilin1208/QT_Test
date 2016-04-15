#include "formidticket.h"
#include "ui_formidticket.h"

FormIdTicket::FormIdTicket(QWidget *parent,QLayout *layout) :
  MtcOpWidget(parent,layout),
    ui(new Ui::FormIdTicket)
{
    ui->setupUi(this);
}

FormIdTicket::~FormIdTicket()
{
    delete ui;
}



