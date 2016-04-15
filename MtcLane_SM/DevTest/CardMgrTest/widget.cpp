#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include "CardMgrArm/cardmgrarm.h"

QString getStatusName(char status)
{
    switch(status)
    {
    case '0':
        return "正常";
    case '1':
        return "故障";
    case '2':
        return "坏卡";
    default:
        return "";
    }
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget), m_cardMgr(new CardMgrArm("CardMgr")), m_isInit(false)
{
    ui->setupUi(this);
    ui->comboBox->addItem("1", 0);
    ui->comboBox->addItem("2", 1);
    ui->comboBox->addItem("3", 2);
    ui->comboBox->addItem("4", 3);
    connect(m_cardMgr, SIGNAL(buttonPush(int)), this, SLOT(cmPush(int)));
    connect(m_cardMgr, SIGNAL(cardAway(int)), this, SLOT(cmCardAway(int)));
    connect(m_cardMgr, SIGNAL(cardOut(int)), this, SLOT(cmCardOut(int)));
    connect(m_cardMgr, SIGNAL(freshInfo()), this, SLOT(cmUpdate()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadDriver()
{
}

void Widget::on_pushButton_4_clicked()
{
    if(!m_isInit)
    {
        if(m_cardMgr->StartDev())
        {
            showLog(QString::fromUtf8("卡机连接成功"));
            m_isInit = true;
        }
    }
    else
    {
        showLog(QString::fromUtf8("卡机已经连接！"));
    }
}

void Widget::on_pushButton_3_clicked()
{
    m_cardMgr->UpCard(ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt());
}

void Widget::on_pushButton_2_clicked()
{
    qDebug() << ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt();
    m_cardMgr->BadCard(ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt());
}

void Widget::cmPush(int index)
{
    showLog(tr("[%1]号卡机发出按钮事件").arg(index));
}

void Widget::cmCardOut(int index)
{
    showLog(tr("[%1]号卡机出卡成功").arg(index));
}

void Widget::cmCardAway(int index)
{
    showLog(tr("[%1]号卡机司机已经取走卡片").arg(index));
}


void Widget::cmUpdate()
{
    showLog(tr("频道状态更新"));
    int cardCount = 0;
    bool hasCardOn = false;
    char status = 0;

    for(int i = 0; i < 4; i++)
    {
        m_cardMgr->GetChannelInfo(i, cardCount, hasCardOn, status);
        showLog(tr("%1号卡机，状态为%2，卡片总数%3，当前是否有卡%4。")
                .arg(i + 1).arg(getStatusName(status)).arg(cardCount).arg(hasCardOn));
    }
}

void Widget::showLog(const QString& sLog)
{
    ui->plainTextEdit->appendPlainText(tr("%1 %2")
                                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                       .arg(sLog));
}
