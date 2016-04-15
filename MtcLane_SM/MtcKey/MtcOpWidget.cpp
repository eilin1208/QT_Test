#include "MtcOpWidget.h"
int MtcOpWidget::doModalShow()
{
    if(m_pEventLoop)
    {
        qWarning() << "exec: Recursive call detected";
        return -1;
    }

    //设置模态显示
    setAttribute(Qt::WA_ShowModal, true);
    show();
    setFocus();
    //开启事件循环
    QEventLoop eventLoop;
    m_pEventLoop = &eventLoop;
    eventLoop.exec();
    m_pEventLoop = 0;
    hide();
    return result();
}

void MtcOpWidget::setCloseTimer(quint32 nUSec)
{
    m_nCloseTimer = startTimer(nUSec);
}

int MtcOpWidget::result()
{
    return m_nResult;
}

void MtcOpWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nCloseTimer)
    {
        if(m_pEventLoop)
        {
            m_pEventLoop->quit();
            m_pEventLoop = 0;
            m_nResult = 0;
        }
        m_nCloseTimer = -1;
    }
}


MtcOpWidget::~MtcOpWidget()
{
    if(m_pEventLoop)
    {
        m_pEventLoop->quit();
        m_pEventLoop = 0;
        m_nResult = -1;
    }
}
