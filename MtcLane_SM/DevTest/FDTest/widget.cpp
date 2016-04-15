#include "widget.h"
#include "ui_widget.h"
#include <QSettings>
#include "../CommonModule/LaneType.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new FareDisplayArm("FD");
    tmpDev = m_mapDevice.begin().value();
    m_fd = (FareDisplayArm*)m_mapDevice.value(0, NULL);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_openDev_clicked()
{
    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->textEdit->append(tr("设备初始化失败！"));
    }
    else
    {
        ui->textEdit->append(tr("设备初始化成功！"));
    }
    //设置费显工作模式，直接从Lane.ini中读取
    int Lanetype;
    QSettings settings("./Lane.ini",QSettings::IniFormat);
    Lanetype = settings.value("LaneInfo/LaneType").toInt();
    m_fd->SetMode(Lanetype,true);
}

void Widget::on_pushButton_closeDev_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->textEdit->setText(tr("设备关闭失败"));
    }
    else
    {
        ui->textEdit->setText("设备关闭成功！");
    }
}

void Widget::on_pushButton_vehClass_clicked()
{
    bool ret;
    int vehclass = ui->comboBox_vheClass->currentText().toInt();
    ret = m_fd->ShowVehClass(vehclass,false,true);
    if(ret)
        ui->textEdit->append(tr("车型：%1显示成功").arg(vehclass));
    else
        ui->textEdit->append(tr("车型：%1显示失败").arg(vehclass));
}

void Widget::on_pushButton_Fare_clicked()
{
    bool ret;
    ret = m_fd->ShowFare(ui->lineEdit_fare->text().toInt()*100,true);
    if(ret)
        ui->textEdit->append(tr("金额：%1显示成功").arg(ui->lineEdit_fare->text()));
    else
        ui->textEdit->append(tr("金额：%1显示失败").arg(ui->lineEdit_fare->text()));
}

void Widget::on_pushButton_Before_clicked()
{
    bool ret;
    ret = m_fd->ShowBalanceBeforeDeduct(ui->lineEdit_before->text().toInt()*100,true);
    if(ret)
        ui->textEdit->append(tr("扣款前金额：%1显示成功").arg(ui->lineEdit_before->text().toInt()));
    else
        ui->textEdit->append(tr("扣款前金额：%1显示失败").arg(ui->lineEdit_before->text().toInt()));
}

void Widget::on_pushButton_After_clicked()
{
    bool ret;
    ret = m_fd->ShowBalanceAfterDeduct(ui->lineEdit_fared->text().toInt()*100,ui->lineEdit_after->text().toInt()*100,true);
    if(ret)
        ui->textEdit->append(tr("实收：%1扣款后金额：%2显示成功").arg(ui->lineEdit_fared->text()).arg(ui->lineEdit_after->text()));
    else
        ui->textEdit->append(tr("实收：%1扣款后金额：%2显示失败").arg(ui->lineEdit_fared->text()).arg(ui->lineEdit_after->text()));
}

void Widget::on_pushButton_Weight_clicked()
{
    int vehclass = ui->comboBox_vheClass->currentText().toInt();
    m_fd->ShowVehClass(vehclass,false,false);
    m_fd->ShowBalanceAfterDeduct(0,0,false,false);
    if(vehclass < 10)
    {
        ui->textEdit->append(tr("计重：车型小于10，为客车，无法显示计重信息，请重新选择车型！"));
        return;
    }
    bool ret;
    int overrate = 1000*(ui->lineEdit_totalwt->text().toInt() - ui->comboBox_limitw->currentText().toInt())/ui->comboBox_limitw->currentText().toInt();
    if(overrate < 0)
    {
        overrate = 0;
    }
    ui->lineEdit_overrate->setText(tr("%1").arg(overrate/10.0));
    ret = m_fd->ShowWeightInfo(ui->lineEdit_totalwt->text().toInt()*1000,ui->comboBox_limitw->currentText().toInt()*1000,
                               overrate,true);
    if(ret)
        ui->textEdit->append(tr("总重：%1 限载：%2 超限率：%3%显示成功").arg(ui->lineEdit_totalwt->text()).
                             arg(ui->comboBox_limitw->currentText().toInt()).arg(ui->lineEdit_overrate->text()));
    else
        ui->textEdit->append(tr("总重：%1 限载：%2 超限率：%3%显示失败").arg(ui->lineEdit_totalwt->text()).
                             arg(ui->comboBox_limitw->currentText().toInt()).arg(ui->lineEdit_overrate->text()));
}

void Widget::on_pushButton_Text_clicked()
{
    bool ret;
    ret = m_fd->ShowText(ui->lineEdit_text->text().toLocal8Bit().data(),SM_Welcome,true);
    if(ret)
        ui->textEdit->append(tr("文字：%1显示成功").arg(ui->lineEdit_text->text()));
    else
        ui->textEdit->append(tr("文字：%1显示失败").arg(ui->lineEdit_text->text()));
}

void Widget::on_pushButton_EnStation_clicked()
{
    bool ret;
    ret = m_fd->ShowEntrySta(ui->lineEdit_EnStation->text().toLocal8Bit().data(),true);
    if(ret)
        ui->textEdit->append(tr("入口站：%1显示成功").arg(ui->lineEdit_EnStation->text()));
    else
        ui->textEdit->append(tr("入口站：%1显示失败").arg(ui->lineEdit_EnStation->text()));

}

void Widget::on_pushButton_ClearAll_clicked()
{
    bool ret;
    ret = m_fd->ClearAll();
    if(ret)
        ui->textEdit->append(tr("清空费显成功"));
    else
        ui->textEdit->append(tr("清空费显失败"));

}

void Widget::on_pushButton_StarAlarm_clicked()
{
    bool ret;
    ret = m_fd->Alarm(true);
    if(ret)
        ui->textEdit->append(tr("启动报警成功"));
    else
        ui->textEdit->append(tr("启动报警失败"));
}

void Widget::on_pushButton_EndAlarm_clicked()
{
    bool ret;
    ret = m_fd->Alarm(false);
    if(ret)
        ui->textEdit->append(tr("停止报警成功"));
    else
        ui->textEdit->append(tr("停止报警失败"));
}
