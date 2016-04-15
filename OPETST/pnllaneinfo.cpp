#include "pnllaneinfo.h"
#include "ui_pnllaneinfo.h"

CPnlLaneInfo::CPnlLaneInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPnlLaneInfo)
{
    ui->setupUi(this);
}

CPnlLaneInfo::~CPnlLaneInfo()
{
    delete ui;
}
