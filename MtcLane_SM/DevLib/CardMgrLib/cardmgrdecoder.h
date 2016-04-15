#ifndef CARDMGRDECODER_H
#define CARDMGRDECODER_H

#include <QObject>
#include "datadef.h"
/*!
 \brief
卡机协议数据解包器，用于将串口接收到的数据进行解析
*/
class CardMgrDecoder : public QObject
{
    Q_OBJECT
public:
    explicit CardMgrDecoder(QObject *parent = 0);
    
signals:
    
public slots:
public:

    /*!
     \brief
     解析串口数据
     \param data
     要解析的数据内容
     \param cardType
     解析后的的数据类型
     \param nCount
     频道总数
     \param rsctl
     rsctl
     \param nIndex
     出口参数，返回此次信息所改变的频道编号，此编号使用二进制表示，int为32位，则可表示32个频道是否被改变，
     int的n位对应n频道，n位为1时说明被改变，n位为0时，说明无改变
     \param arrChannels
     解析信息后需要将解析的内容存放到此数组中
    */
    static bool Decode(QByteArray& data, int nCount,quint8& rsctl, CMFRAMETYPE& cardType, int& nIndex, ChannelInfo * arrChannels);
private:
    /*!
     \brief
     解析状态信息
     \return bool
     是否解析成功
    */
    static bool decodeStatus(QByteArray& data, int nCount, int &nIndex, ChannelInfo * arrChannels);
    /*!
     \brief
     解析已出卡信息
     \return bool
     是否解析成功
    */
    static bool decodeCardOut(QByteArray& data, int& nIndex);
    /*!
     \brief
     解析按键信息
     \return bool
     是否解析成功
    */
    static bool decodePush(QByteArray& data, int& nIndex);
    /*!
     \brief
     解析卡取走信息
     \return bool
     是否解析成功
    */
    static bool decodeCardaway(QByteArray& data, int& nIndex);
    /*!
     \brief
     解析频道信息
     \param data
     \param nCount
     \param cardType
     \param nIndex
     \param arrChannels
     \return bool
     是否解析成功
    */
    static bool decodeChannels(QByteArray& data, int nCount, ChannelInfo* arrChannels);
};

#endif // CARDMGRDECODER_H
