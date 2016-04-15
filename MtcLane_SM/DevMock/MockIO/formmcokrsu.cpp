#include "formmcokrsu.h"
#include "ui_formmcokrsu.h"

FormMcokRsu::FormMcokRsu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMcokRsu)
{
    ui->setupUi(this);
}

FormMcokRsu::~FormMcokRsu()
{
    delete ui;
}
