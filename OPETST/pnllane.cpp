#include "pnllane.h"
#include "ui_pnllane.h"

const int H_LANEINFOSIZE=150;

CPnlLane::CPnlLane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPnlLane)
{
    ui->setupUi(this);
    this->setObjectName("CPnlLane");
    //this->setStyleSheet(QString::fromUtf8("border:1px solid black"));

    m_pnlDevStatus = new CPnlDevStatus(this);
    ui->horizontalLayout->insertWidget(5,m_pnlDevStatus);

    m_pnlLaneInfo = new CPnlLaneInfo(this);
    ui->verticalLayout->insertWidget(2, m_pnlLaneInfo);
    m_pnlLaneInfo->setMinimumHeight(H_LANEINFOSIZE);
    m_pnlLaneInfo->setMaximumHeight(H_LANEINFOSIZE);
}

CPnlLane::~CPnlLane()
{
    delete ui;
}

void CPnlLane::InitUI(int nIndex, int nStationID,int nLaneID, int nLaneType)
{
    m_nStationID = nStationID;
    m_nLaneID = nLaneID;
    m_nLaneType = nLaneType;

    m_pnlLaneInfo->setVisible(false);
    if(nIndex>1)
    {
        ui->lblTop->setVisible(false);
    }

    SetLaneInfo(nLaneID, nLaneType);
    m_pnlDevStatus->InitUI(nStationID,nLaneID,nLaneType);
}

void CPnlLane::SetLaneInfo(int nLaneID, int nLaneType)
{
    QString tmpTypeName;
    switch (nLaneType) {
    case 1:
        tmpTypeName = tr("MTC入口");
        break;
    case 2:
        tmpTypeName = tr("MTC出口");
        break;
    case 3:
        tmpTypeName = tr("ETC入口");
        break;
    case 4:
        tmpTypeName = tr("ETC出口");
        break;
    case 9:
        tmpTypeName = tr("自助发卡");
        break;
    default:
        break;
    }
    ui->lblLane->setText(tr("%1 %2").arg(tmpTypeName).arg(nLaneID));
}


void CPnlLane::AddImageToLbl(QString sImagePath, QLabel *pLable, int nWith,int nHight)
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

void CPnlLane::SpreadLane(bool bSpread)
{
    if(!m_pnlLaneInfo->isVisible() && bSpread)
    {
        this->setMinimumHeight(this->height()+H_LANEINFOSIZE);
        this->setMaximumHeight(this->height()+H_LANEINFOSIZE);
        m_pnlLaneInfo->setVisible(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Res/Gnome-Go-Last-32.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->btnSpread->setIcon(icon);
    }
    else if(m_pnlLaneInfo->isVisible() && !bSpread)
    {
        this->setMinimumHeight(this->height()-H_LANEINFOSIZE);
        this->setMaximumHeight(this->height()-H_LANEINFOSIZE);
        m_pnlLaneInfo->setVisible(false);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Res/Gnome-Go-Bottom-32.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->btnSpread->setIcon(icon);
    }
    emit spreadLaneInfo();
}

void CPnlLane::on_btnSpread_clicked()
{
    SpreadLane(!m_pnlLaneInfo->isVisible());
}
