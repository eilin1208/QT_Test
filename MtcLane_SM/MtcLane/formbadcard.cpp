#include "formbadcard.h"
#include "ui_formbadcard.h"

FormBadCard::FormBadCard(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormBadCard)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

FormBadCard::~FormBadCard()
{
    //QDebug() << "release ....... " << objectName();
    delete ui;
}

bool FormBadCard::lengthIsEnough()
{
    return ui->lineEdit->text().length() > 7;
}

QString FormBadCard::badCardNum()
{
    return ui->lineEdit->text();
}

void FormBadCard::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        if(ui->lineEdit->text().length()<20)
        {
            mtcKeyEvent->setKeyType(KC_Number);
            ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
        }
    }

    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyDel)
    {
        ui->lineEdit->setText(ui->lineEdit->text().left(ui->lineEdit->text().length() - 1));
    }
}

void FormBadCard::showEvent(QShowEvent *event)
{
    ui->lineEdit->clear();
}


