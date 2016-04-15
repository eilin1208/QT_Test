#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

}

Widget::~Widget()
{
    delete ui;
}

//void Widget::on_pushButton_LaneConfig_clicked()
//{
//   ui->contentWidget->setCurrentIndex(1);
//}

//void Widget::on_pushButton_LogCat_clicked()
//{
//    ui->page_Log->setShown(true);

//}

//void Widget::on_pushButton_DevTest_clicked()
//{
//    ui->contentWidget->setCurrentIndex(3);
//}
