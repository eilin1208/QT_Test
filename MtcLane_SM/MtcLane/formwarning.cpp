#include "formwarning.h"
#include "ui_formwarning.h"
const QString& sRedColor = "color: rgb(255, 0, 0);";
const QString& sBlackColor = "color: rgb(0, 0, 0);";
FormWarning::FormWarning(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormWarning)
{
    ui->setupUi(this);
    m_nTimerId = 0;
}

FormWarning::~FormWarning()
{
    //QDebug() << "release ....... " << objectName();
    delete ui;
}

void FormWarning::setWarning(const QString &sMsg, bool bOpenTimer, FormWarning::WARNINGTYPE type)
{
    ui->label_2->setText(sMsg);

    if(bOpenTimer)
    {
        if(m_nTimerId != 0)
        {
            killTimer(m_nTimerId);
            m_nTimerId = startTimer(5000);
        }
        else
        {
            m_nTimerId = startTimer(5000);
        }
    }
}

void FormWarning::clearText()
{
    ui->label_2->clear();
    ui->label_2->setStyleSheet("");
}

void FormWarning::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nTimerId)
    {
        clearText();
    }
}


