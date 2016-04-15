#include "formpublish.h"
#include "ui_formpublish.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "CommonModule/MsgDefine.h"
#include <QDateTime>
FormPublish::FormPublish(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPublish)
{
    ui->setupUi(this);
    CheckAndDisplayNew();
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(CheckAndDisplayNew()));
    timer->start(5000);
}

FormPublish::~FormPublish()
{
    delete ui;
}

void FormPublish::CheckAndDisplayNew()
{
    QString dirName = "./msg";
    QString fileName = "save.msg";
    QFile msgFile(dirName+"/"+fileName);
    if(!msgFile.exists())
    {
        return;
    }
    msgFile.open(QIODevice::ReadWrite);
    TJKMsgInfo recInfo;
    memcpy(&recInfo,msgFile.readAll().data(),sizeof(TJKMsgInfo));

//    memcpy(&recInfo,Utf8toGB2312(sTmp,sizeof(sTmp)),sizeof(TJKMsgInfo));
    //qDebug()<<sTmp;
    //qDebug()<<QDateTime::currentDateTime();
    //qDebug()<<QString::fromLocal8Bit(recInfo.UseTime, 14),"yyyyMMddhhmmss";
    //qDebug()<<QString::fromLocal8Bit(recInfo.ExpireTime, 14),"yyyyMMddhhmmss";
    if((QDateTime::currentDateTime().toTime_t() >=
            QDateTime::fromString(QString::fromLocal8Bit(recInfo.UseTime, 14),"yyyyMMddhhmmss").toTime_t()) &&
            (QDateTime::currentDateTime().toTime_t()
             < QDateTime::fromString(QString::fromLocal8Bit(recInfo.ExpireTime, 14),"yyyyMMddhhmmss").toTime_t()))
    {
        ui->label->setText(GB2312toUtf8(recInfo.MsgDetail));
    }
    else
    {
        ui->label->clear();
    }
}
