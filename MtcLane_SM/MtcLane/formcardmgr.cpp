#include "formcardmgr.h"
#include "ui_formcardmgr.h"
#include "CardReaderArm/qcardreader.h"
#include "devicefactory.h"
#include "laneinfo.h"
const QString READEROK = QObject::tr("%1;%2;%3;%4;%5;%6")
.arg("border-image: url(:/images/image/Reader.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("color: white")
.arg("padding-left: 4px")
.arg("border: 1px solid #447684")
.arg("border-radius:8px");
const QString READERBAD = QObject::tr("%1;%2;%3;%4;%5;%6")
.arg("border-image: url(:/images/image/Readerr.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("color: white")
.arg("padding-left: 4px")
.arg("border: 1px solid #447684")
.arg("border-radius:8px");
const QString BUTTONNOPUSH = QObject::tr("%1;%2;%3;%4;%5;%6")
.arg("border-image: url(:/images/image/redbutton.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("color: white")
.arg("padding-left: 4px")
.arg("border: 1px solid #447684")
.arg("border-radius:8px");
const QString BUTTONPUSH =  QObject::tr("%1;%2;%3;%4;%5;%6")
.arg("border-image: url(:/images/image/greenbutton.png)")
.arg("background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #353B43, stop:1 #151A20)")
.arg("color: white")
.arg("padding-left: 4px")
.arg("border: 1px solid #447684")
.arg("border-radius:8px");
FormCardMgr::FormCardMgr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCardMgr), m_pushTimer(0), m_curLabel(NULL)
{
    ui->setupUi(this);
    connect(getDeviceFactory()->getCardReader(DEV_CardReader1), SIGNAL(StatusChanged(int)),
            this, SLOT(changeCardReader(int)));
    connect(getDeviceFactory()->getCardReader(DEV_CardReader2), SIGNAL(StatusChanged(int)),
            this, SLOT(changeCardReader(int)));
    connect(getDeviceFactory()->getCardMgr(), SIGNAL(buttonPush(int)),
            this, SLOT(changeButton(int)));
    connect(getDeviceFactory()->getCardMgr(), SIGNAL(channelException(int)),
            this, SLOT(cmException(int)));
    connect(getDeviceFactory()->getCardMgr(), SIGNAL(channelNormal(int)),
            this, SLOT(cmNormal(int)));
    connect(getDeviceFactory()->getCardMgr(), SIGNAL(freshInfo()),
            this, SLOT(refreshInfo()));
    m_mapStatus.insert(0, ui->cbException1);
    m_mapStatus.insert(1, ui->cbException2);
    m_mapStatus.insert(2, ui->cbException3);
    m_mapStatus.insert(3, ui->cbException4);
}

FormCardMgr::~FormCardMgr()
{
    delete ui;
}

void FormCardMgr::changeCardReader(int status)
{
    if(sender() == getDeviceFactory()->getCardReader(DEV_CardReader1))
    {
        if(status == DEV_STATUS_OK)
        {
            ui->label->setStyleSheet(READEROK);
        }
        else
        {
            ui->label->setStyleSheet(READERBAD);
        }
    }
    else if(sender() == getDeviceFactory()->getCardReader(DEV_CardReader2))
    {
        if(status == DEV_STATUS_OK)
        {
            ui->label_3->setStyleSheet(READEROK);
        }
        else
        {
            ui->label_3->setStyleSheet(READERBAD);
        }
    }
}

void FormCardMgr::changeButton(int pos)
{
    if(pos == 0)
    {
        m_curLabel = ui->label_2;
    }
    else if(pos == 1)
    {
        m_curLabel = ui->label_4;
    }
    if(m_pushTimer != 0)
    {
        killTimer(m_pushTimer);
        m_pushTimer = 0;
    }
    m_pushTimer = startTimer(500);
}

void FormCardMgr::cmException(int index)
{
    //index号卡机出现问题
    QCheckBox* tmpCheck = m_mapStatus.value(index, NULL);
    if(tmpCheck)
    {
        tmpCheck->setChecked(true);
    }
    else
    {
        //qDebug() << "上送卡机编号为" << index << "没有对应的checkbox";
    }
}

void FormCardMgr::cmNormal(int index)
{
    //index号卡机出现问题
    QCheckBox* tmpCheck = m_mapStatus.value(index, NULL);
    if(tmpCheck)
    {
        tmpCheck->setChecked(false);
    }
    else
    {
        //qDebug() << "上送卡机编号为" << index << "没有对应的checkbox";
    }
}

void FormCardMgr::refreshInfo()
{
    int cardCount;
    bool hasCardOn = false;
    char status = 0;
    int boxIndex = 0;
    getDeviceFactory()->getCardMgr()->GetChannelInfo(0, boxIndex, cardCount, hasCardOn, status);
    ui->cbHasCard1->setChecked(hasCardOn);
    ui->cbException1->setChecked(status == '1');
    ui->cbBadCard1->setChecked(status == '2');
    getLaneInfo()->setBoxIndex(0, cardCount);

    getDeviceFactory()->getCardMgr()->GetChannelInfo(1, boxIndex, cardCount, hasCardOn, status);
    ui->cbHasCard2->setChecked(hasCardOn);
    ui->cbException2->setChecked(status == '1');
    ui->cbBadCard2->setChecked(status == '2');
    getLaneInfo()->setBoxIndex(1, cardCount);


    getDeviceFactory()->getCardMgr()->GetChannelInfo(2, boxIndex, cardCount, hasCardOn, status);
    ui->cbHasCard3->setChecked(hasCardOn);
    ui->cbException3->setChecked(status == '1');
    ui->cbBadCard3->setChecked(status == '2');
    getLaneInfo()->setBoxIndex(2, cardCount);


    getDeviceFactory()->getCardMgr()->GetChannelInfo(3, boxIndex, cardCount, hasCardOn, status);
    ui->cbHasCard4->setChecked(hasCardOn);
    ui->cbException4->setChecked(status == '1');
    ui->cbBadCard4->setChecked(status == '2');
    getLaneInfo()->setBoxIndex(3, cardCount);

}

void FormCardMgr::timerEvent(QTimerEvent *event)
{
    static int counter = 0;
    if(counter % 2 == 0)
    {
        m_curLabel->setStyleSheet(BUTTONPUSH);
    }
    else
    {
        m_curLabel->setStyleSheet(BUTTONNOPUSH);
    }
    counter++;
    if(counter == 10)
    {
        killTimer(m_pushTimer);
        m_pushTimer = 0;
        counter = 0;
    }
}

void FormCardMgr::on_CardBox1ModifyButton_clicked()
{

}

void FormCardMgr::on_CardBox3ModifyButton_clicked()
{

}

void FormCardMgr::on_CardBox2ModifyButton_clicked()
{

}

void FormCardMgr::on_CardBox4ModifyButton_clicked()
{

}
