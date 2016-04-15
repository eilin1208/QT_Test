#include "formmessage.h"
#include "ui_formmessage.h"
#include "formmainwindow.h"
FormMessage::FormMessage(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormMessage)
{
    ui->setupUi(this);
}

FormMessage::~FormMessage()
{
    delete ui;
}


void FormMessage::setMessage(const QString &msg)
{
    ui->lbMessage->setText(msg);
}

void FormMessage::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(m_pEventLoop == 0)
    {
        return;
    }
    if(mtcKeyEvent->isFuncKey())
    {
        if(mtcKeyEvent->func() == KeyConfirm)
        {
            handleConfirmKey();
        }
        if(mtcKeyEvent->func() == KeyEsc)
        {
            handleEscKey();
        }
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

void FormMessage::hideEvent(QHideEvent *)
{
    ui->containerTitle->clear();
    ::showWarning(tr(""));
}

void FormMessage::handleConfirmKey()
{
    //m_nResult = 1;
    setResult(1);
}

void FormMessage::handleEscKey()
{
    //m_nResult = 0;
    setResult(0);
}

QString FormMessage::getMessage()
{
    return ui->lbMessage->text();
}

FormMessage* getMsgWidget(QWidget* widget, QLayout* layout)
{
    static FormMessage* msg = NULL;
    if(msg == NULL)
    {
        msg = new FormMessage(widget, layout);
    }
    return msg;
}
