#include "formwtdetail.h"
#include "ui_formwtdetail.h"
#include "WtArm/wtdatamgrunit.h"
#include "devicefactory.h"
const char* SZYES = "border-image: url(:/images/image/yes.png);";
const char* SZWRONG = "border-image: url(:/images/image/delete.png);";
FormWtDetail::FormWtDetail(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormWtDetail)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    connect(getWtDataMgr(), SIGNAL(WtDataStatusChanged(int,int)),
            this, SLOT(WtDataChanged(int,int)));
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget_2->resizeColumnsToContents();
    if(getDeviceFactory()->getWtDev() != NULL)
    {
        connect(getDeviceFactory()->getWtDev(), SIGNAL(StatusChanged(int)),
                this, SLOT(WtStatusChanged(int)));
    }
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->horizontalHeader()->resizeSection(0,50);
    ui->tableWidget->horizontalHeader()->resizeSection(1,50);
    ui->tableWidget->horizontalHeader()->resizeSection(2,75);
    ui->tableWidget->horizontalHeader()->resizeSection(3,75);
    ui->tableWidget->horizontalHeader()->resizeSection(4,103);
    ui->tableWidget->horizontalHeader()->resizeSection(5,115);
    ui->tableWidget->horizontalHeader()->resizeSection(6,53);


//    ui->tableWidget->resizeColumnsToContents();
//    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

FormWtDetail::~FormWtDetail()
{
    delete ui;
}

void FormWtDetail::changeData()
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
}

void FormWtDetail::WtDataChanged(int nEvent, int nId)
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
}

void FormWtDetail::WtStatusChanged(int nStatus)
{
    ui->lbAssist->setStyleSheet("");
    ui->lbAxis->setStyleSheet("");
    ui->lbCheng->setStyleSheet("");
    ui->lbConnect->setStyleSheet("");
    ui->lbLight->setStyleSheet("");
    if((nStatus & 0x16) != 16)
    {
        ui->lbConnect->setStyleSheet(SZYES);
    }
    else
    {
        ui->lbConnect->setStyleSheet(SZWRONG);
        return;
    }
    if((nStatus & 0x1) != 1)
    {
        ui->lbCheng->setStyleSheet(SZYES);
    }
    else
    {
        ui->lbCheng->setStyleSheet(SZWRONG);
    }
    if((nStatus & 0x2) != 2)
    {
        ui->lbLight->setStyleSheet(SZYES);
    }
    else
    {
        ui->lbLight->setStyleSheet(SZWRONG);
    }
    if((nStatus & 0x4) != 4)
    {
        ui->lbAssist->setStyleSheet(SZYES);
    }
    else
    {
        ui->lbAssist->setStyleSheet(SZWRONG);
    }
    if((nStatus & 0x8) != 8)
    {
        ui->lbAxis->setStyleSheet(SZYES);
    }
    else
    {
        ui->lbAxis->setStyleSheet(SZWRONG);
    }
}

void FormWtDetail::showEvent(QShowEvent *)
{
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex == -1)
    {
        ui->tableWidget->selectRow(0);
    }
}

void FormWtDetail::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        switch(mtcKeyEvent->func())
        {
        case KeyEsc:
            setResult(0);
            break;
        case KeyConfirm:
            setResult(1);
            break;
        case KeyUp:
            if(ui->tableWidget->currentRow() > 0)
            {
                ui->tableWidget->selectRow(ui->tableWidget->currentRow() - 1);
            }
            else
            {
                ui->tableWidget->selectRow(0);
            }
            break;
        case KeyDown:
            if(ui->tableWidget->currentRow() == -1)
            {
                ui->tableWidget->selectRow(0);
            }
            if(ui->tableWidget->currentRow() < ui->tableWidget->rowCount() - 1)
            {
                ui->tableWidget->selectRow(ui->tableWidget->currentRow() + 1);
            }
            break;
        }
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

void FormWtDetail::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    int tmpCount = ui->tableWidget_2->rowCount();
    for(int i = 0; i < tmpCount; i++)
    {
        ui->tableWidget_2->removeRow(0);
    }
    if(currentRow == -1)
    {
        currentRow = 0;
    }
    TWtVehicle* wt = getWtDataMgr()->GetVehicle(currentRow + 1);
    if(wt == NULL)
    {
        return;
    }
    for(int i = 0; i < wt->count(); i++)
    {
        ui->tableWidget_2->insertRow(ui->tableWidget_2->rowCount());
        ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(QString::number(wt->at(i).FWeight/ 1000.0, 'f', 2)));
    }
}


FormWtDetail* getWtDetail(QWidget *widget, QLayout *layout)
{
    static FormWtDetail* pDetail = NULL;
    if(pDetail == NULL)
    {
        pDetail = new FormWtDetail(widget, layout);
    }
    return pDetail;
}
