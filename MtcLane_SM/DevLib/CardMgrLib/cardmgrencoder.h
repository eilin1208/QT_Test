#ifndef CARDMGRENCODER_H
#define CARDMGRENCODER_H

#include <QObject>
#include "datadef.h"
class CardMgrEncoder : public QObject
{
    Q_OBJECT
public:
    explicit CardMgrEncoder(QObject *parent = 0);
    
signals:
    
public slots:
    static bool Encode(quint8 rsctl,SENDTYPE type,int nCardNo, int nCardCount, QByteArray& data);
private:
    static bool EncodeInit(QByteArray& data);
    static bool EncodeCardOut(int nCardNo, QByteArray& data);
    static bool EncodeBadCard(int nCardNo, QByteArray& data);
    static bool EncodeStatus(QByteArray& data);
    static bool EncodeChannel(QByteArray& data);
    static bool EncodeSetChannel(int nCardNo, int nCardCount, QByteArray& data);
    static bool EncodeTrue(QByteArray& data);
    static bool EncodeFalse(QByteArray& data);
};

#endif // CARDMGRENCODER_H
