

#include "formvehquebegin.h"
#include "ui_formvehquebegin.h"
#include "shiftsum.h"
FormVehQueBegin * begin = NULL;
FormVehQueBegin *getQueBegin()
{
    return begin;
}
FormVehQueBegin::FormVehQueBegin(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormVehQueBegin)
{
    ui->setupUi(this);
    QFont font;
    font.setPointSize(13);
    font.setBold(true);
    ui->lab_num->setFont(font);
}

FormVehQueBegin::~FormVehQueBegin()
{
    delete ui;
}
//接受过车信号,更新窗口过车信息
void FormVehQueBegin::setVehCount()
{
    ////qDebug() << "vehCount" << vehCount;
    vehCount = vehCount + 1;
    //更新报文中车队车辆数量信息
    getLaneInfo()->setVehCount(vehCount);
    QString str = QString::number(vehCount);
    if(vehCount < 10)
        str = "0"+str;
    ////qDebug() << "str" << str;
    ui->lab_num->setText(str);
    getShiftSum()->AddMotor();
}

void FormVehQueBegin::showEvent(QShowEvent *)
{
    //resetVehcount(0);
}

void FormVehQueBegin::resetVehcount(int count)
{
    vehCount = count;
    ui->lab_num->setText(tr("%1").arg(vehCount, 2,10, QLatin1Char('0')));
}

void FormVehQueBegin::clearVehCount()
{
    vehCount = 0;
    ui->lab_num->setText("00");
}
