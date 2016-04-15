#include "formlog.h"
#include "ui_formlog.h"
#include "devicefactory.h"
#include "lanectrl.h"
#include "laneinfo.h"
#include "shiftsum.h"
FormLog::FormLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLog)
{
    ui->setupUi(this);
}

FormLog::~FormLog()
{
    delete ui;
}

void FormLog::showLog(const QString &sLog)
{
    if(sLog.isEmpty())
    {
        return;
    }
    clearReset();
    ui->listWidget->addItem(sLog);
    ui->listWidget->setCurrentRow(ui->listWidget->count());
}

void FormLog::clearReset()
{
    if(ui->listWidget->count() > MAXROW)
    {
        ui->listWidget->takeItem(0);
    }
}


//接受过车信号,更新窗口过车信息
void FormLog::setVehCount()
{
    vehCount = vehCount + 1;
    //更新报文中车队车辆数量信息
    getLaneInfo()->setVehCount(vehCount);
    QString str = QString::number(vehCount);
    if(vehCount < 10)
        str = "0"+str;
    //qDebug() << "str"<<str;
//    ui->label_4->setText(str);
    getShiftSum()->AddMotor();
}

void FormLog::clearVehCount()
{
    vehCount = 0;
}
