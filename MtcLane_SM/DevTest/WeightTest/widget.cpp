#include "widget.h"
#include "ui_widget.h"
#include "WtArm/wtdatamgrunit.h"
#include "FareCalc/FareCalcFunc.h"
#include <QMessageBox>
#include <QInputDialog>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new TWtDev("WeightDev");
    tmpDev = m_mapDevice.begin().value();
    wtDev = (TWtDev*)m_mapDevice.value(0, NULL);
    ui->tableWidget_wt->resizeColumnsToContents();
    ui->tableWidget_wt->resizeRowsToContents();
    ui->tableWidget_wt->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget_wt->setShowGrid(true);
    ui->label_message->setFixedHeight(30);
    DeleteRow = -1;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_init_clicked()
{
    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->label_message->setText(tr("计重设备初始化失败"));
        return;
    }
    ui->label_message->setText(tr("计重设备初始化成功,等待数据传入"));
    getWtDataMgr()->setWtDev(wtDev);
    connect(getWtDataMgr(), SIGNAL(WtDataStatusChanged(int,int)),
            this, SLOT(WtDataChanged(int,int)));
    connect(ui->tableWidget_wt,SIGNAL(itemClicked(QTableWidgetItem*)),
            this,SLOT(GetDeleteRow(QTableWidgetItem*)));
}

void Widget::on_pushButton_close_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->label_message->setText(tr("设备关闭失败"));
        return;
    }
    ui->label_message->setText("设备关闭成功！");
    disconnect(getWtDataMgr(), SIGNAL(WtDataStatusChanged(int,int)),
            this, SLOT(WtDataChanged(int,int)));
    disconnect(ui->tableWidget_wt,SIGNAL(itemClicked(QTableWidgetItem*)),
            this,SLOT(GetDeleteRow(QTableWidgetItem*)));

}

void Widget::changeData()
{
    TWtVehicle* wt = NULL;
    for(int i = 0; i < getWtDataMgr()->GetVehicleCount(); i++)
    {
        wt = getWtDataMgr()->GetVehicle(i + 1);
        if(wt != NULL)
        {
            ui->tableWidget_wt->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget_wt->setItem(i, 1, new QTableWidgetItem(QString::number(wt->GetAxisCount())));
            ui->tableWidget_wt->setItem(i, 2, new QTableWidgetItem(QString::number(wt->GetLimitWeight() / 1000.0,'f' ,2)));
            ui->tableWidget_wt->setItem(i, 3, new QTableWidgetItem(QString::number(wt->GetWeight() / 1000.0,'f' ,2)));
            ui->tableWidget_wt->setItem(i, 4, new QTableWidgetItem(QString::number(wt->GetOverloadRate() / 10.0)));
            ui->tableWidget_wt->setItem(i, 5, new QTableWidgetItem(QString::number(wt->getSpeed(), 'f', 2)));
            ui->tableWidget_wt->setItem(i, 6, new QTableWidgetItem(wt->GetDataStatusName()));
        }
    }
    ui->tableWidget_wt->scrollToTop();
}

void Widget::WtDataChanged(int nEvent, int nId)
{
    int nIndex = ui->tableWidget_wt->currentRow();
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
        ui->tableWidget_wt->insertRow(ui->tableWidget_wt->rowCount());
        ui->tableWidget_wt->selectRow(nIndex);
        ui->label_message->setText(tr("插入一条新的计重信息"));
        changeData();
        break;
    case dccInsertVeh:
        ui->tableWidget_wt->insertRow(nId - 1);
        ui->tableWidget_wt->selectRow(nIndex);
        ui->label_message->setText(tr("人工插入一条新的计重信息"));
        changeData();
        break;
    case dccSpitVeh:
        ui->tableWidget_wt->insertRow(nId);
        ui->tableWidget_wt->selectRow(nIndex);
        ui->label_message->setText(tr("人工拆分计重信息"));
        changeData();
        break;
    case dccModifyVeh:
        ui->tableWidget_wt->selectRow(nIndex);
        ui->label_message->setText(tr("人工修改计重信息"));
        changeData();
        break;
    case dccBackVeh:
    case dccDeleteVeh:
    case dccMergeVeh:
        for(i = 0; i < ui->tableWidget_wt->rowCount(); i++)
        {
            if(ui->tableWidget_wt->item(i, 0)->text().toInt() == nId)
            {
                ui->tableWidget_wt->removeRow(i);
                break;
            }
        }
        changeData();
        break;
    default:
        break;
    }
    if(ui->tableWidget_wt->rowCount() > 9)
    {
        int ret = QMessageBox::warning(this,tr("警告"),tr("设备中计重信息已为10条，请尽快删除部分数据，\n否则设备将自动删除部分数据，不再发送给主程序"),QMessageBox::Ok);
        if(ret == QMessageBox::Ok)
            ui->label_message->setText(tr("请选择【清空计重信息】按钮清空计重信息，或选中其中一条按【删除计重信息】进行删除"));
    }
}


void Widget::on_pushButton_ClearOne_clicked()
{
    if(DeleteRow == -1)
    {
        ui->label_message->setText(tr("请选中一条计重信息"));
        return;
    }
    getWtDataMgr()->Delete(DeleteRow,true);
}

void Widget::on_pushButton_ClearAll_clicked()
{
    int row = ui->tableWidget_wt->rowCount();
    for(int i = 0; i < row;i++)
    {
        getWtDataMgr()->Delete(1,true);
    }
}

void Widget::GetDeleteRow(QTableWidgetItem *item)
{
    DeleteRow = item->row()+1;
}

void Widget::on_pushButton_MergeWt_clicked()
{
    if(DeleteRow == -1)
    {
        ui->label_message->setText(tr("请选中一条计重信息"));
        return;
    }
    getWtDataMgr()->Merge(DeleteRow);
}

void Widget::on_pushButton_splitWt_clicked()
{
    if(DeleteRow == -1)
    {
        ui->label_message->setText(tr("请选中一条计重信息"));
        return;
    }
    bool ok;
    int ret = QInputDialog::getInt(this,tr("轴数"),tr("请输入一个正整数："),0,0,20,1,&ok);
    if(!ok)
    {
        ui->label_message->setText("无法得到轴数，请重新输入");
        return;
    }
    if(ui->tableWidget_wt->item(DeleteRow,1)->text().toInt() <= ret)
    {
        ui->label_message->setText(tr("输入轴数大于计重信息的轴数"));
    }
    getWtDataMgr()->Split(DeleteRow,ret);
}
