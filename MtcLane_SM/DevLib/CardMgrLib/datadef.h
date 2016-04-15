#ifndef DATADEF_H
#define DATADEF_H
#include <QObject>
#include <QEvent>
const int EventID_BootUp=1;		//卡机上电
const int EventID_Failure=2;	//卡机故障
const int EventID_CardUp=3;		//卡片取走
const int EventID_BoxLoad=4;	//上箱
const int EventID_BoxUnLoad=5;	//下箱
const int EventID_CardInsert=6;	//备卡
const int EventID_StateChange=7;	//状态改变
const int EventID_UpCardSuccess=8;	//发卡成功
const int EventID_UpCardFailed=9;	//发卡失败

const int CHANNELCOUNT = 4;

enum CMFRAMETYPE
{
    CMNONE = -1,    /*!< 初始值 */
    CMBOOTUP = 'A', /*!< 卡机上电 */
    CMSTATUS = 'B', /*!< 卡机发送状态帧 */
    CMCARDOUT = 'C', /*!< 卡机出卡帧 */
    CMPUSH = 'D', /*!< 按键取卡帧 */
    CMCARDAWAY = 'E', /*!< 卡被取走帧 */
    CMCHANNEL = 'F', /*!< 频道信息帧 */
    CMTRUE = '0', /*!< 正应答信息 */
    CMFALSE = '1' /*!< 负应答信息 */
};

enum SENDTYPE
{
    STNONE = -1,
    STINIT = 'a', /*!< 初始化信息 */
    STCARDOUT = 'b', /*!< 出卡信息 */
    STBADCARD = 'c', /*!< 坏卡信息 */
    STSTATUS = 'e', /*!< 查询卡机状态 */
    STCHANNEL = 'f', /*!< 查询卡夹 */
    STSETCHANNEL = 'g', /*!< 设置卡夹卡数 */
    STTRUE = '0',
    STFALSE = '1'
};

//当卡机事件发生时，会调用此回调函数
typedef void (*onCardMgrEvent)(int nEventType, int param1, int param2);

struct ChannelInfo
{
public:
    char Status;	//'0'正常 '1'故障 '2'坏卡
    bool HasCardBox;//有卡箱
    bool HasCardOn;	//天线有卡
    int nCardBoxId;	//卡箱号
    int nCardCount;	//卡数
public:
    void Clear()
    {
        Status='0';
        HasCardOn =false;
        HasCardBox =false;
        nCardCount =0;
        nCardBoxId=0;
    }
    ChannelInfo()
    {
        Clear();
    }
};

class CardMgrEvent : public QEvent
{
public:
    CardMgrEvent(int nMgrEventType, int nMgrIndex, int nSendType) :
        QEvent((Type)(User + 3)), m_nMgrType(nMgrEventType), m_nMgrIndex(nMgrIndex), m_nSendType(nSendType)
    {}
public:
    void setMgrEventType(int nEventType)
    {
        m_nMgrType = nEventType;
    }
    int getMgrEventType()
    {
        return m_nMgrType;
    }
    void setIndex(int nIndex)
    {
        m_nMgrIndex = nIndex;
    }
    int getIndex()
    {
        return m_nMgrIndex;
    }
    void setSendType(int nSendType)
    {
        m_nSendType = nSendType;
    }
    int getSendType()
    {
        return m_nSendType;
    }

private:
    int m_nMgrType;
    int m_nMgrIndex;
    int m_nSendType;
};

#endif // DATADEF_H
