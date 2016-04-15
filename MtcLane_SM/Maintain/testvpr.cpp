#include "testvpr.h"
#include "ui_testvpr.h"
#include <QPixmap>
#include <QPalette>
#include "CommonModule/LaneType.h"
TestVpr::TestVpr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestVpr)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("车牌识别测试"));
 //    this->setFixedSize(500,300);
     //打开设备
     QMap< int, DevBaseInterface* > m_mapDevice;
     m_mapDevice[0] = new VPRArm("VPR");
     tmpDev = m_mapDevice.begin().value();
     m_qVpr = (VPRArm*)m_mapDevice.value(0, NULL);
     ui->labmessage->setStyleSheet("border-style: outset;"
                                 "border-width: 2px;"
                                 "border-color: beige;");
     ui->labPictrue->setStyleSheet("border-style: outset;"
                                 "border-width: 2px;"
                                 "border-color: beige;");
     ui->labPlate->setStyleSheet("border-style: outset;"
                                 "border-width: 2px;"
                                 "border-color: beige;");
     ui->labcolor->setStyleSheet("border-style: outset;"
                                 "border-width: 2px;"
                                 "border-color: beige;");
     ui->labPictrue->setFixedWidth(this->width());
     ui->labPictrue->setFixedHeight(200);
     ui->label->setFixedHeight(30);
     ui->label_2->setFixedHeight(30);
     ui->labPlate->setFixedHeight(30);
     ui->labmessage->setFixedHeight(30);
     ui->label_3->setFixedHeight(30);
     ui->labcolor->setFixedHeight(30);
}

TestVpr::~TestVpr()
{
    delete ui;
}

void TestVpr::GetPlateColor(int nColor)
{
    switch(nColor)
        {
            case VP_COLOR_BLUE:
            {
                m_palette.setColor(QPalette::Background,Qt::blue);
                m_palette.setColor(QPalette::Foreground,Qt::black);
                ui->labcolor->setText(tr("车牌颜色为蓝色"));
                break;
            }
            case VP_COLOR_YELLOW:
            {
                m_palette.setColor(QPalette::Background,Qt::yellow);
                m_palette.setColor(QPalette::Foreground,Qt::black);
                ui->labcolor->setText(tr("车牌颜色为黄色"));
                break;
            }
            case VP_COLOR_BLACK:
            {
                m_palette.setColor(QPalette::Background,Qt::black);
                m_palette.setColor(QPalette::Foreground,Qt::white);
                ui->labcolor->setText(tr("车牌颜色为黑色"));
                break;
            }
            case VP_COLOR_WHITE:
            {
                m_palette.setColor(QPalette::Background,Qt::white);
                m_palette.setColor(QPalette::Foreground,Qt::black);
                ui->labcolor->setText(tr("车牌颜色为白色"));
                break;
            }
            default:
            {
                break;
            }
        }
}

void TestVpr::customEvent(QEvent *event)
{
    if(event->type() == 2002)
       {
           const CVPRResult* result;
           VPREVENT* pEvent = dynamic_cast<VPREVENT*>(event);
           bool nRet = m_qVpr->SaveVPRResult(pEvent->m_nIndex);
           if(nRet)
           {
               qDebug() << tr("车牌结果保存成功");
               result = m_qVpr->GetCurrentResult();
               //识别结果
               qDebug() << tr("车牌识别结果：Index[%1]，车牌颜色[%2]，车牌号码[%3]，可信度[%4]，大图大小[%5]，小图大小[%6]，二值化图大小[%7]")
                           .arg(result->nIndex).arg(result->nColor).arg(result->sVehPlate).arg(result->nRating)
                           .arg(result->nBigImgSize).arg(result->nSmallImgSize).arg(result->nBinImgSize);
               GetPlateColor(result->nColor);
               if(result->nColor == VP_COLOR_NONE)
               {
                   ui->labPlate->setText(tr("无法显示车牌%1，车牌颜色为-1").arg(result->sVehPlate));
                   return;
               }
               ui->labPlate->setText(tr("%1").arg(result->sVehPlate));
               ui->labPlate->setAutoFillBackground(true);
               ui->labPlate->setPalette(m_palette);
               if(result->HasBigImg())
               {
                   QPixmap model;
                   model.loadFromData(result->lpBigImgBuf, result->nBigImgSize);
                   model = model.scaled(ui->labPictrue->width(),ui->labPictrue->height());
                   ui->labPictrue->setPixmap(model);
                   ui->labmessage->setText(tr("获取一张大图"));
                   return;
               }
               if(result->HasSmallImg())
               {
                   QPixmap model;
                   model.loadFromData(result->lpSmallImgBuf, result->nSmallImgSize);
                   model = model.scaled(ui->labPictrue->width(),ui->labPictrue->height());
                   ui->labPictrue->setPixmap(model);
                   ui->labmessage->setText(tr("获取一张小图"));
                   return;
               }

           }
       }
}

void TestVpr::on_pushButton_OpenDev_clicked()
{
    bool ok = tmpDev->StartDev();
       if(!ok)
       {
           LogMsg("lane", QObject::tr("设备初始化失败！"));
           ui->labmessage->setText(tr("设备初始化失败！"));
           return;
       }
       ui->labmessage->setText(tr("设备初始化成功！"));
       m_qVpr->SetManualCapture(true);
       m_qVpr->SetEventProcWnd(this,2002);
}

void TestVpr::on_pushButton_CloseDev_clicked()
{
    bool ok = tmpDev->StopDev();
       if(!ok)
       {
           LogMsg("lane", QObject::tr("设备关闭失败！"));
           ui->labmessage->setText(tr("设备关闭失败！"));
       }
       else
       {
           ui->labmessage->setText(tr("设备关闭成功！"));
       }
}

void TestVpr::on_pushButton_Capture_clicked()
{
    m_qVpr->Capture();
    ui->labmessage->setText(tr("进行图像抓拍"));
}
