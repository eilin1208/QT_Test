#include "formshowweight.h"
#include "ui_formshowweight.h"
#include <QDebug>
Formshowweight::Formshowweight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Formshowweight)
{
    ui->setupUi(this);
}

Formshowweight::~Formshowweight()
{
    delete ui;
}
