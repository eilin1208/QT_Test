#include "formcapture.h"
#include "ui_formcapture.h"
#include <QDebug>
FormCapture::FormCapture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCapture)
{
    ui->setupUi(this);
}

FormCapture::~FormCapture()
{
    delete ui;
}

QWidget * FormCapture::getVideoCardWidget()
{
    return ui->widget;
}
