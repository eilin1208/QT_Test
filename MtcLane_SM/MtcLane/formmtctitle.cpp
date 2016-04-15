#include "formmtctitle.h"
#include "ui_formmtctitle.h"
#include "laneinfo.h"
#include "CommonModule/LaneType.h"
FormMTCTitle::FormMTCTitle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMTCTitle)
{
    ui->setupUi(this);
    //显示车型收费还是计重收费
    if(getLaneInfo()->isAutoLane())
    {
        ui->lbTollByWt->setText("自动发卡");
        ui->label_14->setText("");
        ui->lbInvoiceNumber->setText("");
    }
    else
    {
        if(!getLaneInfo()->isEntryLane())
        {
            QString sType;
            if(getLaneInfo()->getUseWeight())
            {
                sType = tr("计重收费");
            }
            else
            {
                sType = tr("车型收费");
            }
            ui->lbTollByWt->setText(sType);
        }
        else
        {
            ui->lbTollByWt->setText("入口发卡");
            ui->label_14->setText("");
            ui->lbInvoiceNumber->setText("");
        }
    }

}

FormMTCTitle::~FormMTCTitle()
{
    delete ui;
}

void FormMTCTitle::showEvent(QShowEvent *event)
{
    QString sLane = tr("%1 %2")
            .arg(getLaneInfo()->isEntryLane()? QObject::tr("入口") : QObject::tr("出口"))
            .arg(getLaneInfo()->getLaneCode());
    ui->lbLaneNumber->setText(sLane);
}

void FormMTCTitle::setOperName(const QString &sName)
{
    ui->lbOperator->setText(sName);
}



void FormMTCTitle::setInvoice(qint64 nInvBeginNo, quint32 nInvCount)
{

    ui->lbInvoiceNumber->setText(tr("%1").arg(nInvBeginNo,12,10, QLatin1Char('0')));

}

void FormMTCTitle::setStaName(const QString &sName)
{
    ui->lbTollStationName->setText(sName);
}

void FormMTCTitle::setStaType(int nLaneType, int nLaneCode)
{
    QString sTmp = getLaneTypeName(nLaneType);
    sTmp.append(QString::number(nLaneCode));
}
void FormMTCTitle::setWorkState(int nState)
{
    switch(nState)
    {
    case lsNormalWorking:
        //显示界面切换
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case lsUnlogin:
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case lsRepare:
        break;
    case lsWaitCfg:
        break;
    default:
        break;
    }
}
void FormMTCTitle::setShift(const QString &sShift, const QDateTime &iDataTime)
{
    ui->lbWorkOrder->setText(sShift);
    ui->lbShiftTime->setText(iDataTime.toString("yyyy-MM-dd hh:mm:ss"));
}

void FormMTCTitle::clearInfo()
{
    //ui->lbInvoiceNumber->clear();
    ui->lbOperator->clear();
    ui->lbShiftTime->clear();
    ui->lbWorkOrder->clear();
    //ui->lbWorkStatus->setText("下班");
}

void FormMTCTitle::setCardBoxNum(const QString &num)
{
    ui->labCardBoxNum->setText(num);
}

void FormMTCTitle::setCardNum(quint32 num)
{
    //qDebug()<<"Title_labCardNum = "<<num;
    ui->labCardNum->setText(QString::number(num));
}
