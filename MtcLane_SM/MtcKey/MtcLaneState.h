#ifndef MTC_LANE_STATE_H
#define MTC_LANE_STATE_H

#include <QState>
#include <QWidget>
#include <QDebug>
class MtcLaneState : public QState
{
    Q_OBJECT

public:
    MtcLaneState(QState *parent, QWidget *widget) : QState(parent)
    {
        associateWidget=widget;
    }
signals:

public:
    QWidget *GetWindowPtr()
    {
        return associateWidget;
    }
protected:
    virtual void onEntry(QEvent *event)
    {
        if (NULL!=associateWidget)
        {
            associateWidget->show();
            associateWidget->setFocus();
        }
        QState::onEntry(event);

    }

    virtual void onExit(QEvent *event)
    {
        if (NULL!=associateWidget)
        {
            associateWidget->hide();
        }
        QState::onExit(event);
    }
protected:
    QWidget *associateWidget;
};

#endif // MTC_LANE_STATE_H
