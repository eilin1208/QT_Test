#include "formcarqueue.h"
#include "ui_formcarqueue.h"

FormCarQueue::FormCarQueue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCarQueue)
{
    ui->setupUi(this);
}

FormCarQueue::~FormCarQueue()
{
    delete ui;
}
