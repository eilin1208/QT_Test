#include "cardmgrencoder.h"
#include <QDateTime>
CardMgrEncoder::CardMgrEncoder(QObject *parent) :
    QObject(parent)
{
}

bool CardMgrEncoder::Encode(quint8 rsctl, SENDTYPE type, int nCardNo, int nCardCount, QByteArray &data)
{
    data.append('<');
    data.append((char)rsctl);
    data.append((char)type);
    bool rlt = false;
    switch(type)
    {
    case STINIT:
        rlt = EncodeInit(data);
        break;
    case STCARDOUT:
        rlt = EncodeCardOut(nCardNo, data);
        break;
    case STBADCARD:
        rlt = EncodeBadCard(nCardNo, data);
        break;
    case STSTATUS:
        rlt = EncodeStatus(data);
        break;
    case STCHANNEL:
        rlt = EncodeChannel(data);
        break;
    case STSETCHANNEL:
        rlt = EncodeSetChannel(nCardNo, nCardCount, data);
        break;
    case STTRUE:
    case STFALSE:
        rlt = true;
        break;
    default:
        return false;
    }
    data.append('>');
    return rlt;
}

bool CardMgrEncoder::EncodeInit(QByteArray &data)
{
    data.append(QString::number(500));
    data.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    return true;
}

bool CardMgrEncoder::EncodeCardOut(int nCardNo, QByteArray &data)
{
    data.append((char)nCardNo);
    return true;
}

bool CardMgrEncoder::EncodeBadCard(int nCardNo, QByteArray &data)
{
    data.append((char)nCardNo);
    return true;
}

bool CardMgrEncoder::EncodeStatus(QByteArray &data)
{
    data.append((char)0);
    return true;
}

bool CardMgrEncoder::EncodeChannel(QByteArray &data)
{
    data.append((char)0);
    return true;
}

bool CardMgrEncoder::EncodeSetChannel(int nCardNo, int nCardCount, QByteArray &data)
{
    data.append((char)nCardNo);
    data.append(tr("%1").arg(nCardCount, 3, 10, QLatin1Char('0')));
    return true;
}

bool CardMgrEncoder::EncodeTrue(QByteArray &data)
{
    data.append('0');
    return true;
}

bool CardMgrEncoder::EncodeFalse(QByteArray &data)
{
    data.append('1');
    return true;
}
