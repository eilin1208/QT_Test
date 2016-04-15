#ifndef WIDGETBBS_H
#define WIDGETBBS_H

#include <QWidget>
#include <QStringList>
#include <QList>
#include <QString>
#include <QtGui>

#include "CommonModule/laneutils_new.h"
struct Message
{
    QString msgText;
    int msgPriority;
};
class WidgetBBS : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    WidgetBBS(QWidget *parent = 0);
    //设置文本内容
    void setText(const QString &newText);
    //返回文本内容
    QString text() const { return myText; }
    QSize sizeHint() const;
    //增加一条BBS信息
    void AddBbsMessage(QString message, int priority);
    QStringList CutString(QString  handledString);
    enum{ TEXTWIDTH=40};

   //bool GetPublishList( QList<SPublishValue> &PublishList );
   //bool sort(QList<SPublishValue> &m_list);
protected:
    //事件重载
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    QString myText;
    int offset;
    int myTimerId;
    QList<Message> messageList;
    int textWidth;
//    QList StringList;
};
#endif // WIDGETBBS_H
