#include "formrecordtable.h"
#include "ui_formrecordtable.h"
#include <QDateTime>
#include <QDebug>
#include "laneinfo.h"
#include "shiftsum.h"
#include "formmtctitle.h"
#include "formmainwindow.h"
FormRecordTable::FormRecordTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRecordTable)
{
    ui->setupUi(this);
    //根据车道类型加载不同的表头
    if(getLaneInfo()->isEntryLane())
    {
        ui->tableWidget->setColumnCount(4);
        ui->tableWidget->setHorizontalHeaderLabels(getEntryHeader());
    }
    else
    {
        ui->tableWidget->setColumnCount(8);
        ui->tableWidget->setHorizontalHeaderLabels(getExitHeader());
    }
    //    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //去掉自动序号
    QHeaderView *headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);
    ui->tableWidget->setShowGrid(true);
    //设置表头宽度
    if(!getLaneInfo()->isEntryLane())
    {
        //TODO_liubo
        /*ui->tableWidget->horizontalHeader()->resizeSection(0,72);
        ui->tableWidget->horizontalHeader()->resizeSection(1,89);
        ui->tableWidget->horizontalHeader()->resizeSection(2,40);
        ui->tableWidget->horizontalHeader()->resizeSection(3,90);
        ui->tableWidget->horizontalHeader()->resizeSection(4,70);
        ui->tableWidget->horizontalHeader()->resizeSection(5,60);
        ui->tableWidget->horizontalHeader()->resizeSection(6,60);
        ui->tableWidget->horizontalHeader()->resizeSection(7,60);*/
        ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    }
    else
    {
        ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    }
}

FormRecordTable::~FormRecordTable()
{
    ////qDebug() << "release ....... " << objectName();
    delete ui;
}

void FormRecordTable::insertEntryInfo(const QDateTime &iBusTime, const QString &sCardNum, const QString &sVehType, const QString &sVehPlate)
{
    //    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    //TODO_liubo    int row = ui->tableWidget->rowCount() - 1;
    ui->tableWidget->insertRow(0);
    int row = 0;
    QString nTemprotry = sCardNum;
    if(nTemprotry.toInt() == 0)
    {
        nTemprotry.clear();
        //qDebug()<< "Entry CardNum of nTemprotry is" <<nTemprotry;
    }
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(iBusTime.toString("MM-dd hh:mm:ss")));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(nTemprotry));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(sVehType));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(sVehPlate));
    ui->label_3->setText(QString::number(getShiftSum()->GetTotalCardVCNt()));
    ui->label_6->setText(QString::number(getShiftSum()->GetNormalVCnt()));

    if(ui->tableWidget->rowCount() == 100)
    {
        ui->tableWidget->removeRow(99);
    }
    ui->tableWidget->scrollToTop();
}

void FormRecordTable::insertExitInfo(const QDateTime &iBusTime, const QString &sCardNum, const QString &sVehType, const QString &sVehPlate, const QString &sEnStation, double dMoney, double dFinal, double dRest)
{
    ui->tableWidget->insertRow(0);
    int row = 0;
    QString nTemprotry = sCardNum;
    if(nTemprotry.toInt() == 0)
    {
        nTemprotry.clear();
    }
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(iBusTime.time().toString("hh:mm:ss")));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(nTemprotry));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(sVehType));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(sVehPlate));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(sEnStation));
    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(dMoney, 'f', 2)));
    ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(dFinal, 'f', 2)));
    ui->tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(dRest, 'f', 2)));
    ui->label_3->setText(QString::number(getShiftSum()->GetTotalCardVCNt()));
    ui->label_6->setText(QString::number(getShiftSum()->GetNormalVCnt()));
    if(ui->tableWidget->rowCount() == 100)
    {
        ui->tableWidget->removeRow(99);
    }
    ui->tableWidget->scrollToTop();
}

QStringList FormRecordTable::getEntryHeader()
{
    return (QStringList() << tr("业务时间") << tr("卡号") << tr("车型") << tr("车牌"));
}

QStringList FormRecordTable::getExitHeader()
{
    return (QStringList() << tr("业务时间") << tr("卡号")<< tr("车型")
            << tr("车牌") << tr("入口站")<< tr("应收")<< tr("实收")<< tr("余额"));
}

void FormRecordTable::setCardAndVehNum(int card, int veh)
{
    ui->label_3->setText(QString::number(card));
    ui->label_6->setText(QString::number(veh));
    FormMTCTitle *pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);
    pTitle->setCardNum(getDataFileMgr()->getCardBoxInfo().nCardCount);
}

void FormRecordTable::clearExitInfo()
{
    while(ui->tableWidget->rowCount()>0)
    {
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
    }
}

void FormRecordTable::ReverseSet()
{

    int curNum = getShiftSum()->GetNormalVCnt();
    ui->label_6->setText(QString::number(curNum));

}
