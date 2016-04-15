#include "formbottombar.h"
#include "ui_formbottombar.h"
#include <QDateTime>
#include <QDebug>
#include "PacketSendModule/sqlitedatasaver.h"
#include "laneinfo.h"
#include "CommonModule/laneutils_new.h"
FormBottomBar::FormBottomBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBottomBar)
{
    ui->setupUi(this);
    //显示版本号
    ui->labVersion->setText("V" + QApplication::instance()->applicationVersion());
    m_nShowTimer = startTimer(1000);
    m_nUnUpLoadTimer = startTimer(60000);
    m_nShowDiskSize = startTimer(10000);
}

FormBottomBar::~FormBottomBar()
{
    delete ui;
}

void FormBottomBar::setRateVersion(const QString &sVersion)
{
    ui->labRateVersion->setText(tr("费率版本：%1").arg(sVersion));
}

void FormBottomBar::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nShowTimer)
    {
        ui->labTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if(event->timerId() == m_nUnUpLoadTimer)
    {
        ShowUnUpWaste();
    }
    if(event->timerId() == m_nShowDiskSize)
    {
        ShowDiskSize();
        showMemoryInfo();
    }
}
void FormBottomBar::ShowDiskSize()
{
    int total,avail;
    getLaneInfo()->getDiskSize(total,avail);
    if(total == -1 && avail == -1)
    {
        ui->labelDiskSize->setText("总容量:未知 / 剩余空间:未知");
    }
    else
    {
        ui->labelDiskSize->setText(tr("总容量:%1M / 剩余空间:%2M").arg(total).arg(avail));
        if(avail <= getLaneInfo()->getDiskSizeLimit())
        {
            ui->labelDiskSize->setStyleSheet("color:red");
        }
        else
        {
            ui->labelDiskSize->setStyleSheet("color:white");
        }
    }
}
void FormBottomBar::ShowUnUpWaste()
{
    //获取未上传数据
    int nNum = getDataSaver()->GetUnDispatchCount(dtSocket);
    ui->labWaste->setText(tr("未上传数据：%1条").arg(nNum));
}

void FormBottomBar::showOperNo(quint32 dwOperNo)
{
    if(dwOperNo == 0)
    {
        ui->labOperNo->setText("");
    }
    else
    {
        ui->labOperNo->setText(tr("操作员工号:%1").arg(dwOperNo, 8, 10, QLatin1Char('0')));
    }
}

void FormBottomBar::showMemoryInfo()
{
    quint32 totalSize = 0;
    quint32 freeSize = 0;
    getMemoryInfo(&totalSize, &freeSize);
#ifndef Q_OS_WIN
    ui->labMemory->setText(tr("内存：%1M/%2M").arg(freeSize / 1024 / 1024).arg(totalSize / 1024 / 1024));
#endif
}
