#ifndef MTC_OP_WIDGET_H
#define MTC_OP_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QState>
#include <QDebug>
#include "MtcKeyDef.h"
#include <QLabel>
#include <QDebug>
#include <QEventLoop>
class MtcOpWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
public:
    MtcOpWidget(QWidget *parent=0, QLayout *layout=0) : QWidget(parent, 0)
    {
        //设置为无边框和透明
        setWindowOpacity(1);
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        hide();
        QVBoxLayout* vLayout = static_cast<QVBoxLayout*>(layout);
        if(vLayout != NULL)
        {
            vLayout->insertWidget(0, this);
        }
        //设置为无边框和透明-modify by xk
        m_pEventLoop = NULL;
        m_nResult = 0;
        m_nCloseTimer = -1;
    }
    virtual ~MtcOpWidget();


signals:
    //验证成功发送传递信号
    void transition();
public slots:
    void quitModal()
    {
        if(m_pEventLoop)
        {
            m_pEventLoop->quit();
            m_pEventLoop = 0;
            this->hide();
        }
    }

public:
    virtual void buildSubStateMachine(QState *parent, QState* next = NULL)
    {
    }
public:
    QString error()
    {
        QLabel* errorLabel = this->findChild<QLabel*>("containerBottom");
        if(errorLabel != NULL)
        {
            return errorLabel->text();
        }
        return "";
    }
    void setError(const QString& error)
    {
        QLabel* errorLabel = this->findChild<QLabel*>("containerBottom");
        if(errorLabel != NULL)
        {
            errorLabel->setText(error);
        }
    }

    QString title()
    {

        QLabel* titleLabel = this->findChild<QLabel*>("containerTitle");
        if(titleLabel != NULL)
        {
            return titleLabel->text();
        }
        return "";
    }
    void setTitle(const QString& title)
    {
        QLabel* titleLabel = this->findChild<QLabel*>("containerTitle");
        if(titleLabel != NULL)
        {
            titleLabel->setText(title);
        }
    }
    //******************
    //模态显示，模仿dialog，避免多重继承
    //返回处理结果，通过为1，不通过为0,其他异常可自己定义
    //modify by xk
    int doModalShow();
    //do modal show自动关闭
    void setCloseTimer(quint32 nUSec);
    //执行结果
    int result();
protected:
    void filterChildKeyEvent(QWidget *child)
    {
        child->installEventFilter(this);
    }

    void filterChildrenKeyEvent()
    {
         QList<QWidget *> widgets = this->findChildren<QWidget *>();

         for (int i=0; i<widgets.size(); i++)
         {
             widgets[i]->installEventFilter(this);
         }
    }
    void setResult(int nResult)
    {
        m_nResult = nResult;
        m_pEventLoop->quit();
        m_pEventLoop = 0;
        this->hide();
    }

    void timerEvent(QTimerEvent *event);
protected:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (event->type() == MtcKeyPressedEvent::getEventType())
        {
            MtcKeyPressedEvent *mtcKeyEvent = static_cast<MtcKeyPressedEvent *>(event);
            mtcKeyPressed(mtcKeyEvent);
            return true;
        }
        else
        {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }

protected:
    virtual void customEvent(QEvent *event)
    {
        if (event->type() == MtcKeyPressedEvent::getEventType())
        {
            MtcKeyPressedEvent *mtcKeyEvent = static_cast<MtcKeyPressedEvent *>(event);
            mtcKeyPressed(mtcKeyEvent);
        }
    }

    virtual void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
    {
        //QDebug() << this->objectName()+": key "+mtcKeyEvent->keyName().toAscii();
    }
protected:
    QEventLoop* m_pEventLoop;
    int m_nResult;
    int m_nCloseTimer;
};


#endif // MTC_OP_WIDGET_H
