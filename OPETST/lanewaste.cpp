#include "lanewaste.h"
#include "ui_lanewaste.h"

LaneWaste::LaneWaste(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LaneWaste)
{
    ui->setupUi(this);
}

LaneWaste::~LaneWaste()
{
    delete ui;
}
