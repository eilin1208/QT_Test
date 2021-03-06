#include "testvideocard.h"
#include "ui_testvideocard.h"
#include <QPixmap>
TestVideoCard::TestVideoCard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestVideoCard)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new VideoCard_ARM("VideoCard");
    tmpDev = m_mapDevice.begin().value();
    videocard = (VideoCard_ARM*)m_mapDevice.value(0, NULL);
    videocard->SetVideoUi(ui->label_video);
    ui->label_capture->setStyleSheet("border-style: outset;"
                                "border-width: 2px;"
                                "border-color: beige;");
    ui->label_video->setStyleSheet("border-style: outset;"
                                "border-width: 2px;"
                                "border-color: beige;");
}

TestVideoCard::~TestVideoCard()
{
    delete ui;
}

void TestVideoCard::on_pushButton_capture_clicked()
{
    m_filename.clear();
    if(!videocard->Prepare())
    {
        ui->label_capture->setText(tr("抓拍失败，请查看设备是否存在问题"));
        return;
    }
    m_filename = videocard->getFileName();
    ui->label_capture->clear();
    QPixmap pixmap(m_filename);
    pixmap = pixmap.scaled(ui->label_capture->width(),ui->label_capture->height());
    ui->label_capture->setPixmap(pixmap);
}

void TestVideoCard::on_pushButton_init_clicked()
{
    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->label_video->setText(tr("设备初始化失败，无法显示视频图像"));
        return;
    }
    ui->label_video->clear();
    videocard->InitVideoCap();
    videocard->Display();
    connect(ui->checkBox_auto,SIGNAL(clicked(bool)),this,SLOT(AutoCapture(bool)));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(on_pushButton_Capture_clicked()));
}

void TestVideoCard::on_pushButton_close_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->label_video->setText(tr("设备关闭失败"));
    }
    else
    {
        ui->label_video->clear();
        ui->label_video->setText("设备关闭成功！");
        ui->label_capture->clear();
        disconnect(ui->checkBox_auto,SIGNAL(clicked(bool)),this,SLOT(AutoCapture(bool)));
    }
}

void TestVideoCard::AutoCapture(bool clicked)
{
    if(clicked)
    {
        if(m_timer->isActive())
        {
            m_timer->stop();
            m_timer->start(ui->lineEdit_timer->text().toInt());
        }
        else
        {
            m_timer->start(ui->lineEdit_timer->text().toInt());
        }
    }
    else
    {
        if(m_timer->isActive())
            m_timer->stop();
    }
}
