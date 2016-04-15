#include "formdevtest.h"
#include "ui_formdevtest.h"
#include "testfd.h"
#include "testprinter.h"
#include "testwt.h"
#include "iocardtest.h"
#include "testvpr.h"
#include "testcardreader.h"
#include "testvideocard.h"
#include <QGridLayout>
#include <QFont>
#include <QDebug>
FormDevTest::FormDevTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDevTest)
{
    ui->setupUi(this);
    createPages();
//    ui->toolBox->setItemText(0,tr("设备名称"));
//    ui->toolBox->setFixedWidth(200);
    ui->groupBox->setMinimumWidth(ui->pushButton_fdTest->width()+20);
}

FormDevTest::~FormDevTest()
{
    delete ui;
}

void FormDevTest::on_pushButton_fdTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_FareDisplayer));
}

void FormDevTest::on_pushButton_ioTest_clicked()
{
ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_IOCard));
}

void FormDevTest::on_pushButton_VprTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_VehPlateRecognizer));
}
void FormDevTest::on_pushButton_VedioCardTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_VideoCard));
}

void FormDevTest::on_pushButton_PrinterTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_SmallPrinter));
}

void FormDevTest::on_pushButton_WtTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_WEIGHT));
}

void FormDevTest::createPages()
{
    QGridLayout *layout = new QGridLayout();
    QLabel* devTitle = new QLabel();
    devinfo = new QLabel();
    QWidget *w = new QWidget(this);
    QFont font;
    qDebug()<<font.defaultFamily()<<"--feng";
    layout->addWidget(devTitle,0,0,1,5,Qt::AlignCenter);
    layout->addWidget(devinfo,1,1,4,4,Qt::AlignLeft|Qt::AlignTop);
    devTitle->setText(tr("设备测试"));
    devTitle->setStyleSheet(tr("font: bold 16px;"));
    devinfo->setStyleSheet(tr("font: 14px;"));
    devinfo->setText(tr("主要测试的设备包括：\n%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8")
                     .arg("费额显示器：主要显示欢迎语、车型、收费金额、报警、计重信息等信息的设备")
                     .arg("卡读写器：主要对收费软件涉及到各种IC卡，如云通卡、卡盒卡等进行读写的设备")
                     .arg("IO板：主要是对对栏杆、存在线圈、通过线圈、报警、通行灯、雨棚灯等设备进行控制")
                     .arg("计重设备：主要是获取通过货车的轴数及对应的重量以便计算货车收费")
                     .arg("车牌识别：主要是通过抓拍到的图像自动获得车牌，以便收费元进行对照")
                     .arg("摄像头：主要是获得车道的实时图像")
                     .arg("打印机：主要是打印收费票据")
                     .arg("请选择相应设备名称的按键开始测试选择的设备。"));
    devinfo->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    w->setLayout(layout);
    m_DevIndex[DEV_IOCard]=addPages(new IoCardTest);
    m_DevIndex[DEV_Info]=addPages(w);
    m_DevIndex[DEV_SmallPrinter]=addPages(new TestPrinter);
    m_DevIndex[DEV_FareDisplayer]=addPages(new TestFD);
    m_DevIndex[DEV_WEIGHT]=addPages(new TestWt);
    m_DevIndex[DEV_VehPlateRecognizer]=addPages(new TestVpr);
    m_DevIndex[DEV_VideoCard]=addPages(new TestVideoCard);
    m_DevIndex[DEV_CardReader]=addPages(new TestCardReader);
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_Info));
}

int FormDevTest::addPages(QWidget *page)
{
    return ui->stackedWidget->addWidget(page);
}

void FormDevTest::on_pushButton_cardreader_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_CardReader));
}

void FormDevTest::Init()
{
    ui->stackedWidget->setCurrentIndex(m_DevIndex.value(DEV_Info));
}
