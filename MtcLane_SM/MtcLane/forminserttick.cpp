#include "forminserttick.h"
#include "ui_forminserttick.h"

ForminsertTick::ForminsertTick(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::ForminsertTick)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

ForminsertTick::~ForminsertTick()
{
    qDebug() << "release ....... " << objectName();
    delete ui;
}

QString ForminsertTick::getEnterPassword()
{
    return ui->lineEdit->text();
}

void ForminsertTick::clearPassword()
{
    ui->lineEdit->clear();
}

void ForminsertTick::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
    }
}

void ForminsertTick::showEvent(QShowEvent *event)
{
    ui->lineEdit->setFocus();
}
