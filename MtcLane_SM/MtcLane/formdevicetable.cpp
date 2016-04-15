#include "formdevicetable.h"
#include "ui_formdevicetable.h"
#include "laneinfo.h"
#include "formmainwindow.h"
FormDeviceTable::FormDeviceTable(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormDeviceTable)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QMap<DEVNO, DevBaseInterface* >::const_iterator it = getDeviceFactory()->getMapDevice()->begin();
    for(it; it != getDeviceFactory()->getMapDevice()->end(); it++)
    {
        connect(it.value(), SIGNAL(StatusChanged(int)), this, SLOT(showDevStatus(int)));
    }
}

FormDeviceTable::~FormDeviceTable()
{
    delete ui;
}

void FormDeviceTable::showDevStatus( int status)
{
    for(int i = 0; i < getDeviceFactory()->getMapDevice()->count(); i++)
    {
        if(ui->tableWidget->item(i, 0) == NULL)
        {
            return;
        }
        if(getDeviceName(getDeviceFactory()->getMapDevice()->key((DevBaseInterface*)sender()))
                == ui->tableWidget->item(i, 0)->text())
        {
            ui->tableWidget->setItem(i, 1,
                                  new QTableWidgetItem(status == DEV_STATUS_OK? tr("正常") : tr("异常")));
        }
    }
}

void FormDeviceTable::showEvent(QShowEvent *event)
{
    //获取所有设备状态
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->removeRow(0);
    }
    QMap< DEVNO, DevBaseInterface* >::const_iterator it = getDeviceFactory()->getMapDevice()->begin();
    for(it; it != getDeviceFactory()->getMapDevice()->end(); it++)
    {
        showDevInfo(getDeviceName(it.key()),
                    ((DevBaseInterface*)it.value())->GetStatus());
    }
    ::showWarning(tr("请检查设备状态，按【确认】键继续"));
}

void FormDeviceTable::showDevInfo(const QString &dev, int status)
{
    int index = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(index);
    ui->tableWidget->setItem(index, 0, new QTableWidgetItem(dev));
    ui->tableWidget->setItem(index, 1, new QTableWidgetItem(status == DEV_STATUS_OK? tr("正常") : tr("异常")));
}
