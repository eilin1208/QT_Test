#include "formcalchange.h"
#include "ui_formcalchange.h"
#include "laneinfo.h"
FormCalChange::FormCalChange(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormCalChange)
{
    ui->setupUi(this);
    ui->lineEdit->setFocus();
    filterChildrenKeyEvent();
}

FormCalChange::~FormCalChange()
{
    delete ui;
}

void FormCalChange::showEvent(QShowEvent *event)
{
    ui->lineEdit->clear();
    ui->label_5->clear();
    int value = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;
    ui->label_3->setText(QString::number(value).append("元"));
}

void FormCalChange::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyDel)
    {
        //删除最后一位
        ui->lineEdit->backspace();
    }
    //计算金额
    int value1 = ui->lineEdit->text().toInt();
    int value2 = (getLaneInfo()->getTollMoneyInfo()->nCashMoney +
                  getLaneInfo()->getTollMoneyInfo()->nCashCardMoney) / 100;//=QString::number(getLaneInfo()->getFinalMoney()).toInt();
    int value3=value1-value2;

    if(value3 > 0)
    {
        ui->label_5->setText(QString::number(value3));
    }
    else
    {
        ui->label_5->clear();
    }
    if(mtcKeyEvent->isFuncKey()&&((mtcKeyEvent->func()==KeyUp)||(mtcKeyEvent->func()==KeyDown)||(mtcKeyEvent->func()==KeyLeft)||(mtcKeyEvent->func()==KeyRight)))
    {
        ui->lineEdit->clear();
        ui->label_5->clear();
    }
}
