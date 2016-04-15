#include "iocardtest.h"
#include "ui_iocardtest.h"

IoCardTest::IoCardTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IoCardTest)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new IOBoardArm("IOCard");
    tmpDev = m_mapDevice.begin().value();
    m_ioBoard = (IOBoardArm*)m_mapDevice.value(0, NULL);
    ui->label->setStyleSheet("border-style: outset;"
                                "border-width: 2px;"
                                "border-color: beige;");
    ui->label->setFixedHeight(30);
    m_Inited = false;
}

IoCardTest::~IoCardTest()
{
    delete ui;
}

void IoCardTest::on_pBtn_Init_clicked()
{
    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->label->setText(tr("设备初始化失败！"));
        return;
    }
    m_Inited = true;
    ui->label->setText(tr("设备初始化成功！"));
    //连接设备状态信号，更新界面
    connect(m_ioBoard, SIGNAL(InputChanged(quint32, quint8)),
            this, SLOT(IOInputChanged(quint32,quint8)));
}

void IoCardTest::on_pushButton_CloseDev_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->label->setText(tr("设备关闭失败！"));
        return;
    }
    m_Inited = false;
    disconnect(m_ioBoard, SIGNAL(InputChanged(quint32, quint8)),
            this, SLOT(IOInputChanged(quint32,quint8)));
    ui->label->setText(tr("设备关闭成功！"));
}

void IoCardTest::on_pushButton_CanopyGreen_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetCanopy(1);
}

void IoCardTest::on_pushButton_PassGreen_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetLaneLamp(1);
}

void IoCardTest::on_pushButton_CanopyRed_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetCanopy(0);
}

void IoCardTest::on_pushButton_PassRed_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetLaneLamp(0);
}

void IoCardTest::on_pushButton_BarriarUp_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetBar(1);
}

void IoCardTest::on_pushButton_BarriarDown_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetBar(0);
}


void IoCardTest::IOInputChanged(quint32 dev, quint8 nStatus)
{
    switch(dev)
    {
    case DI_LoopFront:
        {
            if(nStatus == 0)
                ui->checkBox_front->setChecked(false);
            if(nStatus == 1)
                ui->checkBox_front->setChecked(true);
            break;
        }
    case DI_LoopBack:
        {
        if(nStatus == 0)
            ui->checkBox_back->setChecked(false);
        if(nStatus == 1)
            ui->checkBox_back->setChecked(true);
        break;
        }
    case DI_BarrierUp:
        {
            ui->checkBox_up->setChecked(true);
            ui->checkBox_down->setChecked(false);
            break;
        }
    case DI_BarrierDown:
        {
            ui->checkBox_down->setChecked(true);
            ui->checkBox_up->setChecked(false);
            break;
        }
    default:
        break;
    }
}
void IoCardTest::on_pushButton_StartAlarm_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetAlarm(1);
    ui->label->setText(tr("IO开始进行警报，请自动选择警告【取消警报】按钮，关闭警报！"));
}

void IoCardTest::on_pushButton_EndAlarm_clicked()
{
    if(!m_Inited)
        return;
    m_ioBoard->SetAlarm(0);
    ui->label->setText(tr("警告已关闭"));
}
