#ifndef CARDSTATEDETECTER_H
#define CARDSTATEDETECTER_H

#include <QObject>
#include "devicefactory.h"

//用于检测操作员翻卡，云通卡流程
class CardStateDetecter : public QObject
{
    Q_OBJECT
public:
    explicit CardStateDetecter(QObject *parent = 0);
    
signals:
    
public slots:
    void hasOneCard(quint32 nCardId, int nCardType);
    void hasNoCard();
public:
    //启动检测
    void Start();
    //停止检测
    void Close();
private:
private:
    quint32 m_nCardId;
    int m_nCardType;
    bool m_bNoCard;
};

#endif // CARDSTATEDETECTER_H
