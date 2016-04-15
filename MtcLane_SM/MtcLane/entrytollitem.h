#ifndef ENTRYTOLLITEM_H
#define ENTRYTOLLITEM_H

#include <QObject>
#include <QDateTime>

#include "tollitem.h"
class EntryTollItem : public TollItem
{
    Q_OBJECT
public:
    explicit EntryTollItem(QObject *parent = 0);
    void FillYwWaste(QDateTime lpTime);
    void SendYewuWasteMsg(int nInductCnt);
signals:

public slots:

};

#endif // ENTRYTOLLITEM_H
