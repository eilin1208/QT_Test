#include "cardmgrdecoder.h"

CardMgrDecoder::CardMgrDecoder(QObject *parent) :
    QObject(parent)
{
}

bool CardMgrDecoder::Decode(QByteArray &data, int nCount, quint8& rsctl,CMFRAMETYPE &cardType, int &nIndex, ChannelInfo *arrChannels)
{
    rsctl = data.at(0);
    cardType = (CMFRAMETYPE)data.at(1);
    data = data.right(data.length() - 2);
    switch(cardType)
    {
    case CMBOOTUP:
    case CMTRUE:
    case CMFALSE:
        return true;
    case CMSTATUS:
        return decodeStatus(data, nCount,nIndex, arrChannels);
    case CMCARDOUT:
        return decodeCardOut(data, nIndex);
    case CMPUSH:
        return decodePush(data, nIndex);
    case CMCARDAWAY:
        return decodeCardaway(data, nIndex);
    case CMCHANNEL:
        return decodeChannels(data, nCount, arrChannels);
    default:
        return false;
    }
}

bool CardMgrDecoder::decodeStatus(QByteArray &data, int nCount, int &nIndex, ChannelInfo *arrChannels)
{
    if(data.length() < 26)
    {
        return false;
    }
    //空出前两位当前卡机编号
    int boxNo = data.at(0) - 0x31;
    if(boxNo >= 0)
    {
        nIndex = (0x1 << boxNo);
    }
    boxNo = data.at(1) - 0x31;
    if(boxNo >= 0)
    {
        nIndex |= (0x1 << boxNo);
    }
    //循环读取每个频道内容
    int channelIndex = 0;
    for(int i = 2; i < nCount * 6 + 2; i = i + 6)
    {
        arrChannels[channelIndex].Status = data.at(i);
        arrChannels[channelIndex].HasCardBox = data.at(i + 1) == 0x30;
        arrChannels[channelIndex].nCardCount = data.mid(i + 2, 3).toInt();
        arrChannels[channelIndex].HasCardOn = data.at(i + 5) == 0x30;
        channelIndex++;
    }
    return true;
}

bool CardMgrDecoder::decodeCardOut(QByteArray &data,  int &nIndex)
{
    if(data.length() < 2)
    {
        return false;
    }
    //获取当前机位
    int boxNo = data.at(1) - 0x31;
    if(boxNo >= 0)
    {
        nIndex = 0x1 << boxNo;
    }
    return true;
}

bool CardMgrDecoder::decodePush(QByteArray &data, int &nIndex)
{
    if(data.length() < 2)
    {
        return false;
    }
    int boxNo = data.at(1) - 0x31;
    if(boxNo  >= 0)
    {
        nIndex = 0x1 << boxNo;
    }
    return true;
}

bool CardMgrDecoder::decodeCardaway(QByteArray &data, int &nIndex)
{
    if(data.length() < 2)
    {
        return false;
    }
    int boxNo = data.at(1) - 0x31;
    if(boxNo  >= 0)
    {
        nIndex = 0x1 << boxNo;
    }
    return true;
}

bool CardMgrDecoder::decodeChannels(QByteArray &data, int nCount, ChannelInfo *arrChannels)
{
    if(data.length() < 36)
    {
        return false;
    }
    int channelIndex = 0;
    for(int i = 0; i < nCount * 9; i = i + 9)
    {
        arrChannels[channelIndex].nCardCount = data.mid(i + 6, 3).toInt();
        channelIndex++;
    }
    return true;

}
