#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include "../CommonModule/LaneType.h"
#include "../CommonModule/laneutils_new.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //打开设备
    QMap< int, DevBaseInterface* > m_mapDevice;
    m_mapDevice[0] = new PrinterArm("Printer");
    tmpDev = m_mapDevice.begin().value();
    printer = (PrinterArm*)m_mapDevice.value(0, NULL);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_openDev_clicked()
{
    bool ok = tmpDev->StartDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备初始化失败！"));
        ui->plainTextEdit->appendPlainText(tr("设备初始化失败！"));
    }
    else
    {
        ui->plainTextEdit->appendPlainText(tr("设备初始化成功！"));
    }
}

void Widget::on_pushButton_CloseDev_clicked()
{
    bool ok = tmpDev->StopDev();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("设备关闭失败！"));
        ui->plainTextEdit->appendPlainText(tr("设备关闭失败"));
    }
    else
    {
        ui->plainTextEdit->appendPlainText("设备关闭成功！");
    }
}

void Widget::on_pushButton_printer_clicked()
{
    SetNoteContent();
    bool ret = printer->Print();
    if(ret)
        ui->plainTextEdit->appendPlainText(tr("打印成功！"));
    else
        ui->plainTextEdit->appendPlainText(tr("打印失败！"));

}

void Widget::SetNoteContent()
{
    memset(&printer->m_NoteContent,0,sizeof(printer->m_NoteContent));
    //时间
    qstrncpy(printer->m_NoteContent.szDateTime,
             QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),
             32);
    //入口站
    qstrncpy(printer->m_NoteContent.EnStationName,Utf8toGB2312(tr("草铺")).data(),32);
    //出口站
    qstrncpy(printer->m_NoteContent.ExStationName,Utf8toGB2312(tr("昆明西")).data(),32);
    //收费员代码
    qstrncpy(printer->m_NoteContent.CollectorID,
             "00100729",
             9);
    //收费金额
    printer->m_NoteContent.Fare_Num = 1500;
    //是否为维护
    printer->m_NoteContent.sMark = 1;
    //收费类型
    if(ui->lineEdit_feetype->text().toInt() == 0)
    {
        printer->m_NoteContent.iFeeType = 0;
        qstrncpy(printer->m_NoteContent.VehClass, Utf8toGB2312(GetVehClassName_LPCSTR(VC_Car2)).data(), 4);
    }
    else if(ui->lineEdit_feetype->text().toInt() == 1)
    {
        printer->m_NoteContent.iFeeType = 1;
        //总重
        qstrncpy(printer->m_NoteContent.weight,
                 tr("%1").arg(45000/1000.00).toLocal8Bit().data(),
                 10);
        //限载
        qstrncpy(printer->m_NoteContent.LimitW,
                 tr("%1").arg(49000/1000.00).toLocal8Bit().data(),
                 10);
        //超限率
        qstrncpy(printer->m_NoteContent.OverP,
                 tr("%1").arg(0/1000.00).toLocal8Bit().data(),
                 10);
    }
}

