#include "widgetbbs.h"
#include <QPainter>
#include <QTimerEvent>
#include <QDebug>

#include"CommonModule/laneutils_new.h"

#include <QTextCodec>
WidgetBBS::WidgetBBS(QWidget *parent)
    : QWidget(parent)
{
    offset = 0;
    myTimerId = 0;

    //QList<SPublishValue> PublishList;

    //if(GetPublishList(PublishList))
    if(false)
    {
//        if(sort(PublishList))
//        {
//            for(int i=0;i<PublishList.size();i++)
//            {
//                AddBbsMessage(QTextCodec::codecForName("GB2312")->toUnicode(PublishList[i].szInfo),3);
//            }
//        }
    }
    else
    {
        AddBbsMessage(tr("无有效公告"),1);
    }
}

void WidgetBBS::setText(const QString &newText)
{
    myText = newText;
    update();
    updateGeometry();
}

QSize WidgetBBS::sizeHint() const
{
    return fontMetrics().size(0, text());
}

void WidgetBBS::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    //设置重绘的字体和字号
    QFont font("微软雅黑",14);
    painter.setFont(font);
    //设置背景刷
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));

    //设置边框
    QRectF rectangle(0,0,this->width()-1,this->height()-1);
    painter.drawRect(rectangle);

    //设置刷新高度
    if (textWidth < 1)
        return;
    int x = -offset;
    while (x < height())
    {
        //设置重画的位置、大小、颜色
        for(int index = 0; index < messageList.count(); index++)
        {
            switch(messageList[index].msgPriority)
            {
            case 1:
                painter.setPen(Qt::red );
                break;
            case 2:
                painter.setPen(Qt::blue);
                break;
            case 4:
                painter.setPen(Qt::green);
                break;
            default:
                painter.setPen(Qt::yellow);
                break;
            }
            painter.drawText(10, x+(index-messageList.count()/2+1)* TEXTWIDTH,  width(), textWidth,
                             Qt::AlignLeft | Qt::AlignVCenter, messageList[index].msgText);
        }

        x += textWidth;
    }
}

void WidgetBBS::showEvent(QShowEvent * /* event */)
{
    myTimerId = startTimer(30);
}

void WidgetBBS::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == myTimerId)
    {
        //按像素移动文字内容
        ++offset;
        if (offset >= textWidth)
        {
            offset = 0;
        }
        scroll(0, -1);
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void WidgetBBS::hideEvent(QHideEvent * /* event */)
{
    qDebug() << "kill timer hide event";
    killTimer(myTimerId);
    myTimerId = 0;
}

void WidgetBBS::AddBbsMessage(QString message, int priority)
{
    QStringList strList;
    Message tempMsg;
    tempMsg.msgText = message;
    tempMsg.msgPriority = priority;
    if(messageList.count() == 0)
    {
        strList = CutString(message);
        foreach (QString msg, strList) {
            tempMsg.msgText = msg;
            tempMsg.msgPriority = priority;
            messageList.append(tempMsg);
        }
    }
    else
    {
        if(messageList[0].msgPriority <= tempMsg.msgPriority)
        {
            strList = CutString(message);
            for(int j = 0; j < strList.count(); j++)
            {
                if(j == 0)
                {
                    tempMsg.msgText = strList[j];
                    tempMsg.msgPriority = priority;
                    messageList.prepend(tempMsg);
                }
                else
                {
                    tempMsg.msgText = strList[j];
                    tempMsg.msgPriority = priority;
                    messageList.insert(messageList.begin() + j,tempMsg);
                }
            }
        }
        else
        {
            for(QList<Message>::iterator i = messageList.begin();
                i != messageList.end() - 1; i++)
            {
                if((*i).msgPriority > tempMsg.msgPriority && (*(i + 1)).msgPriority <= tempMsg.msgPriority)
                {
                    strList = CutString(message);
                    for(int j = 0; j < strList.count(); j++)
                    {
                        if(j == 0)
                        {
                            tempMsg.msgText = strList[j];
                            tempMsg.msgPriority = priority;
                            messageList.insert(i + 1,tempMsg);
                        }
                        else
                        {
                            tempMsg.msgText = strList[j];
                            tempMsg.msgPriority = priority;
                            messageList.insert(i + 1 + j,tempMsg);
                        }
                    }

                    textWidth = TEXTWIDTH * messageList.count();
                    offset = 0;
                    return;
                }
            }
            strList = CutString(message);
            foreach (QString msg, strList) {
                tempMsg.msgText = msg;
                tempMsg.msgPriority = priority;
                messageList.append(tempMsg);
            }
        }
    }
    textWidth = TEXTWIDTH * messageList.count();
    offset = 0;
}

QStringList WidgetBBS::CutString(QString handledString)
{
    QStringList tempList;
    while(handledString.length() > 25)
    {
        tempList << handledString.left(25);
        handledString = handledString.right(handledString.length() - 25);
    }
    tempList << handledString;
    return tempList;
}

//bool WidgetBBS::GetPublishList( QList<SPublishValue> &PublishList )
//{
   //实际用
//    QPublishParamFile *publishFile;
//    if(getParamFile(&publishFile,cfPublish)==true)
//    {
//        ParamMapContainer<quint16, SPublishValue>& container = publishFile->GetSPublishValue();
//        QMap<quint16,SPublishValue> temp=container.getMap();
//        QMap<quint16,SPublishValue>::iterator it = temp.begin();
//        char szLocalTime[15]={'\0'};
//        memcpy(szLocalTime,QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit(),15);
//        for (it;it != temp.end();++it)
//        {
//            SPublishValue pPublish=it.value();
//            if ((strncmp(szLocalTime,pPublish.szUseTime,14)>=0) &&
//                    (strncmp(szLocalTime,pPublish.szExpireTime,14)<0))
//            {
//                PublishList.append(pPublish);
//            }
//        }
//        return PublishList.size()>0;
//    }
//    return false;
////测试用
//    QPublishParamFile paramFile;
//    paramFile.InitParamFile();
//    if(paramFile.LoadParamFile(paramFile.GetFileName()) == true)
//    {
//        ParamMapContainer<quint16, SPublishValue> &container = paramFile.GetSPublishValue();
//        QMap<quint16,SPublishValue> temp=container.getMap();
//        QMap<quint16,SPublishValue>::iterator it = temp.begin();
//        char szLocalTime[15]={'\0'};
//        memcpy(szLocalTime,QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit(),15);
//        for (it;it != temp.end();++it)
//        {
//            SPublishValue pPublish=it.value();
//            if ((strncmp(szLocalTime,pPublish.szUseTime,14)>=0) &&
//                    (strncmp(szLocalTime,pPublish.szExpireTime,14)<0))
//            {
//                PublishList.append(pPublish);
//            }
//        }
//        return PublishList.size()>0;
//    }
//    return false;
//}

//bool WidgetBBS::sort(QList<SPublishValue> &m_list)
//{

//    if(NULL== m_list.size())
//        return false;
//    for(int i=0;i<m_list.size();++i)
//    {
//        for(int j=0;j<m_list.size()-i-1;++j)
//        {
//            if(m_list[j].szUseTime>m_list[j+1].szUseTime)
//            {
//                swap(m_list[j],m_list[j+1]);
//            }
//        }
//    }
//    return true;
//}
