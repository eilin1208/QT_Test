#include "pnldevstatus.h"
#include "ui_pnldevstatus.h"

const int W_DEVICONSIZE = 32;	//图片宽度
const int H_DEVICONSIZE = 32;	//图片高度

CPnlDevStatus::CPnlDevStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPnlDevStatus)
{
    ui->setupUi(this);
}

CPnlDevStatus::~CPnlDevStatus()
{
    delete ui;
}


void CPnlDevStatus::InitUI(int nStationID,int nLaneID, int nLaneType)
{
    m_nStationID = nStationID;
    m_nLaneID = nLaneID;
    m_nLaneType = nLaneType;

    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevAlarm, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevAutoBar, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevCanopyLight, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevCardReader1, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevCardReader2, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevCardReader3, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevCardReader4, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop1, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop2, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop3, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop4, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop5, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevLoop6, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevNetStatus, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevPassLight, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevRSU, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevVLP, W_DEVICONSIZE, H_DEVICONSIZE);
    AddImageToLbl(tr(":/DevRes/未知.png"),ui->lblDevWeight, W_DEVICONSIZE, H_DEVICONSIZE);
    if(1 == nLaneType)
    {
        ui->lblDevWeight->setVisible(false);
        ui->lblDevRSU->setVisible(false);
    }
}

void CPnlDevStatus::AddImageToLbl(QString sImagePath, QLabel *pLable, int nWith,int nHight)
{
    pLable->setMinimumWidth(nWith);
    pLable->setMaximumWidth(nWith);

    pLable->setMinimumHeight(nHight);
    pLable->setMaximumHeight(nHight);

    pLable->resize(nWith, nHight);
    //生成图像objPixmap
    QPixmap objPixmap(sImagePath);
    pLable->setPixmap(objPixmap.scaled(QSize(nWith,nHight)));

}
