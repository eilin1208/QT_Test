#include "formmockio.h"
#include "ui_formmockio.h"
#include <QCoreApplication>
#include "datadefine.h"
#include "formserver.h"
FormMockIO::FormMockIO(QWidget *parent) :
    IDisplay(parent),
    ui(new Ui::FormMockIO)
{
    ui->setupUi(this);
}
FormMockIO::~FormMockIO()
{
    delete ui;

}
void FormMockIO::on_DI_LoopTrig1_clicked(bool checked)
{
    if(checked)
    {
        qDebug()<<"DI_LoopTrig1 = 1";
        outstatus.bit_data.e = 1;
        outstatus.bit_data.m = 0;
    }
    else
    {
        qDebug("DI_LoopTrig1 = 0");
        outstatus.bit_data.m = 1;
        outstatus.bit_data.e = 0;
    }
    emit readySend();
}
void FormMockIO::on_DI_LoopTrig2_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_LoopTrig2 = 1");
        outstatus.bit_data.n = 0;
        outstatus.bit_data.f = 1;
    }
    else
    {
        qDebug("DI_LoopTrig2 = 0");
        outstatus.bit_data.f = 0;
        outstatus.bit_data.n = 1;
    }
    emit readySend();
}

void FormMockIO::on_DI_LoopFront_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_LoopFront = 1");

        outstatus.bit_data.a = 1;
        outstatus.bit_data.i = 0;

    }
    else
    {
        qDebug("DI_LoopFront = 0");
        outstatus.bit_data.i = 1;
        outstatus.bit_data.a = 0;
    }
    qDebug()<<QDateTime::currentDateTime().toString("hh-mm-ss-zzz")<<" LOOPf";
    emit readySend();
}



void FormMockIO::on_DI_LoopBack_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_LoopBack = 1");
        outstatus.bit_data.b = 1;
        outstatus.bit_data.j = 0;
    }
    else
    {
        qDebug("DI_LoopBack = 0");
        outstatus.bit_data.j = 1;
        outstatus.bit_data.b = 0;
    }
    qDebug()<<QDateTime::currentDateTime().toString("hh-mm-ss-zzz")<<" LOOPBack";
    emit readySend();
}

void FormMockIO::on_DI_BarrierDown_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_BarrierDown = 1");
        outstatus.bit_data.c = 1;
        outstatus.bit_data.l = 0;
        qDebug()<<QDateTime::currentDateTime().toString("hh-mm-ss-zzz")<<" BarrierDown";
        emit readySend();
        on_DI_BarrierUp_clicked(false);
    }
    else
    {
        qDebug("DI_BarrierDown = 0");
        outstatus.bit_data.l = 1;
        outstatus.bit_data.c = 0;
    }

}

void FormMockIO::on_DI_BarrierUp_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_BarrierUp = 1");
        outstatus.bit_data.d = 1;
        outstatus.bit_data.k = 0;
        qDebug()<<QDateTime::currentDateTime().toString("hh-mm-ss-zzz")<<" BarrierUp";
        emit readySend();
        on_DI_BarrierDown_clicked(false);
    }
    else
    {
        qDebug("DI_BarrierUp = 0");
        outstatus.bit_data.k = 1;
        outstatus.bit_data.d = 0;
    }

}
void FormMockIO::on_DI_AlarmDetector_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DI_AlarmDetector = 1");
    }
    else
    {
        qDebug("DI_AlarmDetector = 0");
    }
}

void FormMockIO::on_DO_CanopyLightGreen_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_CanopyLightGreen = 1");
    }
    else
    {
        qDebug("DO_CanopyLightGreen = 0");
    }
}

void FormMockIO::on_DO_CanopyLightRed_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_CanopyLightRed = 1");
    }
    else
    {
        qDebug("DO_CanopyLightRed = 0");
    }
}

void FormMockIO::on_DO_PassLightGreen_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_PassLightGreen = 1");
    }
    else
    {
        qDebug("DO_PassLightGreen = 0");
    }
}

void FormMockIO::on_DO_PassLightRed_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_PassLightRed = 1");
    }
    else
    {
        qDebug("DO_PassLightRed = 0");
    }
}

void FormMockIO::on_DO_BarrierDown_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_BarrierDown = 1");
    }
    else
    {
        qDebug("DO_BarrierDown = 0");
    }
}

void FormMockIO::on_DO_BarrierUp_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_BarrierUp = 1");
    }
    else
    {
        qDebug("DO_BarrierUp = 0");
    }
}

void FormMockIO::on_DO_AlarmLight_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_AlarmLight = 1");
    }
    else
    {
        qDebug("DO_AlarmLight = 0");
    }
}

void FormMockIO::on_DO_AlarmSound_clicked(bool checked)
{
    if(checked)
    {
        qDebug("DO_AlarmSound = 1");
    }
    else
    {
        qDebug("DO_AlarmSound = 0");
    }
}
void FormMockIO::beginDisplay()
{
    instatus.data = 0;
    outstatus.data = 0;
    this->setFocus();
    this->show();
    instatus.data = m_io.toUInt();
    if(instatus.bit_data.i == 1)
    {
        ui->DO_CanopyLightGreen->setChecked(true);
        ui->DO_CanopyLightRed->setChecked(false);
    }
    if(instatus.bit_data.j == 1)
    {
        ui->DO_CanopyLightGreen->setChecked(false);
        ui->DO_CanopyLightRed->setChecked(true);
    }
    if(instatus.bit_data.k == 1)
    {
        ui->DO_PassLightGreen ->setChecked(true);
        ui->DO_PassLightRed->setChecked(false);
    }
    if(instatus.bit_data.l == 1)
    {
        ui->DO_PassLightGreen->setChecked(false);
        ui->DO_PassLightRed->setChecked(true);
    }
    if(instatus.bit_data.n == 1)
    {
        ui->DO_BarrierUp->setChecked(true);
        ui->DO_BarrierDown->setChecked(false);
        on_DI_BarrierUp_clicked(true);
    }
    if(instatus.bit_data.m == 1)
    {
        ui->DO_BarrierUp->setChecked(false);
        ui->DO_BarrierDown->setChecked(true);
        ui->DI_BarrierDown->setChecked(true);
        on_DI_BarrierDown_clicked(true);
    }
    if(instatus.bit_data.p == 1)
    {
        ui->DO_AlarmLight->setChecked(true);
        ui->DO_AlarmSound->setChecked(true);
        ui->DI_AlarmDetector->setChecked(true);
        qDebug()<<"Alarm true";
    }
    if(instatus.bit_data.o  == 1)
    {
        ui->DO_AlarmLight->setChecked(false);
        ui->DO_AlarmSound->setChecked(false);
        ui->DI_AlarmDetector->setChecked(false);
        qDebug()<<"Alarm false";
    }
}
QByteArray FormMockIO::getBuffer()
{
    m_io.clear();
    if(outstatus.data == 0)
    {
        return m_io;
    }
    m_io = QByteArray::number(outstatus.data);
    outstatus.data = 0;
    return m_io;
}

void FormMockIO::GetData(QByteArray &data)
{
    m_io = data;
}
