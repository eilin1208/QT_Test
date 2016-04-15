#include "pnlstation.h"
#include "ui_pnlstation.h"
#include "common/unused.h"

const int H_LANEUISIZE = 35;

CPnlStation::CPnlStation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPnlStation)
{
    ui->setupUi(this);
}

CPnlStation::~CPnlStation()
{
    delete ui;
}

void CPnlStation::InitUI(int nOrgCode)
{
    ui->lblStationName->setText(tr("%1").arg(nOrgCode));
    int nKey;
    for(int i=0; i<20; i++)
    {
        CPnlLane *p = new CPnlLane(this);
        p->setMaximumHeight(H_LANEUISIZE);
        p->setMinimumHeight(H_LANEUISIZE);
        p->InitUI(i+1,nOrgCode, i, i%2+1);
        ui->verticalLayout_2->insertWidget(i+2, p, H_LANEUISIZE);
        connect(p,SIGNAL(spreadLaneInfo()),this,SLOT(spreadLaneInfo()));
        nKey = nOrgCode*1000 + i;
        m_pLaneMap[nKey] = p;
    }
//    verticalSpacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    ui->verticalLayout->addItem(verticalSpacer);
    this->resize(this->width(), GetUIHeight());
}


int CPnlStation::GetUIHeight()
{
    int nHeight = 0;
    QMap<int, CPnlLane*>::iterator it = m_pLaneMap.begin();
    for(; it!=m_pLaneMap.end(); ++it)
    {
        nHeight += it.value()->height();
    }
    nHeight += ui->lblStationName->height()+(m_pLaneMap.size()*2);
    return nHeight;
}

void CPnlStation::resizeEvent(QResizeEvent *event)
{
    UNUSED(event);
    //CPnlStation::resizeEvent(event);
    int nHeight = GetUIHeight();
    this->setMaximumHeight(nHeight);
    this->setMinimumHeight(nHeight);
    this->resize(this->width(), nHeight);
}

void CPnlStation::spreadLaneInfo()
{
    int nHeight = GetUIHeight();
    this->setMaximumHeight(nHeight);
    this->setMinimumHeight(nHeight);
    this->resize(this->width(), nHeight);
    emit ChangeStationSize();
}

void CPnlStation::on_pushButton_clicked()
{
    QMap<int, CPnlLane*>::iterator it = m_pLaneMap.begin();
    for(; it!=m_pLaneMap.end(); ++it)
    {
        it.value()->SpreadLane(true);
    }
}

void CPnlStation::on_pushButton_2_clicked()
{
    QMap<int, CPnlLane*>::iterator it = m_pLaneMap.begin();
    for(; it!=m_pLaneMap.end(); ++it)
    {
        it.value()->SpreadLane(false);
    }
}
