#include "formvehqueprocess.h"
#include "ui_formvehqueprocess.h"

FormVehqueProcess::FormVehqueProcess(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormVehqueProcess)
{
    ui->setupUi(this);
    //初始化计时参数
    count = 10;
    //设置显示字体
    //font-size:12pt; font-weight:600; color:#cb4820
    ui->lab_time->setStyleSheet("color:#cb4820");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    ui->lab_time->setFont(font);
    ui->lab_time->setText(tr("%1").arg(count));
    //初始化计时
    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(setCount()));
}

FormVehqueProcess::~FormVehqueProcess()
{
    //qDebug() << "release ....... " << objectName();
    delete ui;
}

void FormVehqueProcess::setCount()
{
    if(count > 1)
    {
        //TODO：添加接收到授权信号状态跳转处理
        count =  count -1;
        QString str = QString::number(count);
        ui->lab_time->setText(str);
        //qDebug()<<count;
    }
    else
        //TODO：添加在指定时间内没有接受到授权，返回到前状态
    {
        timer->stop();
        resetCount();
        emit monitor();
        return;
    }
}

void FormVehqueProcess::startCount()
{
    timer->start(1000);
    //qDebug()<<"开始计时";
}
//手动模拟发送信号
void FormVehqueProcess::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->getLogicKey() == KeyS)
    {
        //qDebug()<<"发送授权信号"<<mtcKeyEvent->getLogicKey();
        timer->stop();
        resetCount();
        emit begin();
        //TODO：过车准备，抬杠，灯绿

    }
}

void FormVehqueProcess::resetCount()
{
    count = 10;
    ui->lab_time->setText(tr("%1").arg(count));
}
