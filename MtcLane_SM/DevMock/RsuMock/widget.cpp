#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
bool g_read;

PortComm* getPort()
{
    static PortComm port;
    return &port;
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    inittimer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(SendAuto()));
    connect(inittimer,SIGNAL(timeout()),this,SLOT(SendInit()));
}

Widget::~Widget()
{
    disconnect(timer,SIGNAL(timeout()),this,SLOT(SendAuto()));
    disconnect(inittimer,SIGNAL(timeout()),this,SLOT(SendInit()));
    delete ui;
}

void Widget::on_push_auto_clicked()
{
    timer->start(3000);//添加多长时间发一次。
    ui->plainTextEdit->appendPlainText(QString("开始自动运行"));
    LogMsg(QString("开始自动运行"));
}

void Widget::on_push_one_clicked()
{
    getPort()->SetStatus(B2_frame);
    ReadySend();
}

void Widget::on_push_init_clicked()
{
    connect(getPort(),SIGNAL(StatusChange()),this,SLOT(ReadySend()));
    connect(getPort(),SIGNAL(ReadyString()),this,SLOT(ShowString()));
    getPort()->StartRsu();
    ui->push_auto->setEnabled(true);
    ui->push_closeauto->setEnabled(true);
    ui->push_one->setEnabled(true);  
    inittimer->start(3000);
}

void Widget::on_push_close_clicked()
{
    timer->stop();
    inittimer->stop();
    getPort()->CloseRsu();
    disconnect(getPort(),SIGNAL(StatusChange()),this,SLOT(ReadySend()));
    disconnect(getPort(),SIGNAL(ReadyString()),this,SLOT(ShowString()));
    ui->push_auto->setEnabled(false);
    ui->push_closeauto->setEnabled(false);
    ui->push_one->setEnabled(false);
    LogMsg(QString("RSU is closed! "));
    ui->plainTextEdit->appendPlainText(QString("RSU is closed! "));   
}

void Widget::on_push_closeauto_clicked()
{
    timer->stop();
    ui->plainTextEdit->appendPlainText(QString("自动运行关闭"));
    LogMsg(QString("自动运行关闭"));
}

void Widget::ShowString()
{
    QString string;
    string = getPort()->ReadStr();
    ui->plainTextEdit->appendPlainText(string);
}

void Widget::ReadySend()
{
    m_status = getPort()->GetStatus();
    if(m_status == B2_wait)
    {
        inittimer->stop();
        return;
    }
    if(m_status == B0_frame)
    {
        getPort()->SendFrame();
    }
    if(m_status == B2_frame)
    {
       getPort()->SendFrame();
    }
    if(m_status == B3_frame)
    {
        getPort()->SendFrame();
    }
    if(m_status == B4_frame)
    {
        getPort()->SendFrame();
    }
    if(m_status == B5_frame)
    {
        getPort()->SendFrame();
    }
}

void Widget::SendInit()
{
    ReadySend();
}

void Widget::SendAuto()
{
    getPort()->SetStatus(B2_frame);
    ReadySend();
}
void Widget::on_push_clear_clicked()
{
    if(m_status == B2_wait)
    {
        ui->plainTextEdit->clear();
    }
}
