#include "formwtinfo.h"
#include "ui_formwtinfo.h"
#include "FareCalc/FareCalcFunc.h"
#include "formamendweight.h"
#include "devicefactory.h"
FormWtInfo::FormWtInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormWtInfo)
{
    ui->setupUi(this);
    connect(getWtDataMgr(), SIGNAL(WtDataStatusChanged(int,int)),
            this, SLOT(WtDataChanged(int,int)));
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    //TODO_liubo
    /*ui->tableWidget->horizontalHeader()->resizeSection(0,75);
    ui->tableWidget->horizontalHeader()->resizeSection(1,50);
    ui->tableWidget->horizontalHeader()->resizeSection(2,65);
    ui->tableWidget->horizontalHeader()->resizeSection(3,85);
    ui->tableWidget->horizontalHeader()->resizeSection(4,85);
    ui->tableWidget->horizontalHeader()->resizeSection(5,105);
    ui->tableWidget->horizontalHeader()->resizeSection(6,75);*/
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setShowGrid(true);

}

FormWtInfo::~FormWtInfo()
{
    delete ui;
}

void FormWtInfo::changeData()
{
    TWtVehicle* wt = NULL;
    for(int i = 0; i < getWtDataMgr()->GetVehicleCount(); i++)
    {
        wt = getWtDataMgr()->GetVehicle(i + 1);
        if(wt != NULL)
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(wt->GetAxisCount())));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(wt->GetLimitWeight() / 1000.0,'f' ,2)));
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(wt->GetWeight() / 1000.0,'f' ,2)));
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(wt->GetOverloadRate() / 10.0)));
            ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(wt->getSpeed(), 'f', 2)));
            ui->tableWidget->setItem(i, 6, new QTableWidgetItem(wt->GetDataStatusName()));
        }
    }
    ui->tableWidget->scrollToTop();
}
void FormWtInfo::showHeadData()
{
    TWtVehicle* wt = getWtDataMgr()->GetVehicle(1);
    if(wt == NULL)
    {
        ui->lbAxisCount->clear();
        ui->lbLimit->clear();
        ui->lbOverRate->clear();
        ui->lbWeight->clear();
        return;
    }
    ui->lbAxisCount->setText(QString::number(wt->GetAxisCount()));
    ui->lbWeight->setText(QString::number(wt->GetWeight() / 1000.0, 'f', 2));
    ui->lbOverRate->setText(QString::number(wt->GetOverloadRate() / 10.0, 'f',1).append("%"));
    ui->lbLimit->setText(QString::number(wt->GetLimitWeight() / 1000.0, 'f', 2));
}

void FormWtInfo::WtDataChanged(int nEvent, int nId)
{
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    int i =0;
    //qDebug() << "id is " << nId;
    switch(nEvent)
    {
    case dccNewVeh:
        //在底部插入一行
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccInsertVeh:
        ui->tableWidget->insertRow(nId - 1);
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccSpitVeh:
        ui->tableWidget->insertRow(nId);
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccModifyVeh:
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccBackVeh:
    case dccDeleteVeh:
    case dccMergeVeh:
        for(i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if(ui->tableWidget->item(i, 0)->text().toInt() == nId)
            {
                ui->tableWidget->removeRow(i);
                break;
            }
        }
        changeData();
        break;
    default:
        break;

    }
    showHeadData();

}

