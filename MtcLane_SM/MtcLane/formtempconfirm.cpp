#include "formtempconfirm.h"
#include "ui_formtempconfirm.h"

FormTempConfirm::FormTempConfirm(QWidget *parent, QLayout *layout)
    :MtcOpWidget(parent,layout),
    ui(new Ui::FormTempConfirm)
{
    ui->setupUi(this);
}

FormTempConfirm::~FormTempConfirm()
{
    delete ui;
}

void FormTempConfirm::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->func() == KeyConfirm)
    {
        emit transition();
    }
    if(mtcKeyEvent->func() == KeyEsc)
    {
        emit cancel();
    }
}
