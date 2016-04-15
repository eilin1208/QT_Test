#ifndef CARDMGRARM_H
#define CARDMGRARM_H

#include <QObject>
#include "../CommonModule/ilogmsg.h"
#include "BaseInterface/devbaseinterface.h"
enum POSITION
{
    POSITIONUP, /*!< 上机位 */
    POSITIONDOWN /*!< 下机位 */
};
class CardMgrEvent;
class CardMgrArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit CardMgrArm(const QString & sDevName, QObject *parent = 0);
    
signals:
    /*!
     \brief
     用户按键
     \param nIndex
     上下机位
    */
    void buttonPush(int nIndex);

    /*!
     \brief
    出卡成功
     \param nMgrIndex
     出卡卡机编号
    */
    void cardOut(int nMgrIndex);

    /*!
     \brief
    用户取走卡片
     \param nMgrIndex
     用户取卡编号
    */
    void cardAway(int nMgrIndex);

    /*!
     \brief
    卡机出现故障
     \param nMgrIndex
     出现故障编号
    */
    void channelException(int nMgrIndex);

    void channelNormal(int index);
    /*!
     \brief
    通知上层刷新数据
    */
    void freshInfo();

    /*!
     \brief
    整个卡机全部出现问题
    */
    void cmException();

    /*!
     \brief
    卡机恢复正常
    */
    void cmNormal();
public slots:
public:
    /*!
     \brief
    让某卡机出卡,在写卡成功后调用
     \param nMgrIndex
     卡机编号
    */
    void UpCard(int nMgrIndex);
    /*!
     \brief
    处理某卡机坏卡，写卡失败时调用
     \param nMgrIndex
     坏卡卡机编号
    */
    void BadCard(int nMgrIndex);
    /*!
     \brief
    清除所有坏卡
    */
    void ClearAllBadCard();
    /*!
     \brief
    设置某卡机卡片总数
     \param nMgrIndex
     卡机编号
     \param nCardCount
     卡片总数
    */
    void SetCardCount(int nMgrIndex, int nCardCount);
    /*!
     \brief
    获取频道信息
     \param nBoxCode
     频道代码0-3
     \param nCount
     卡片总数
     \param bHasCardOn
     当前是否有卡
     \param curStatus
     当前状态
    */
    void GetChannelInfo(int nBoxCode,int &boxIndex, int& nCount, bool& bHasCardOn, char& curStatus);

    /*!
     \brief
    获取卡盒信息
     \param nBoxCode
     频道代码0-3
     \param nCount
     卡片总数
    */
    void GetBoxInfo(int nBoxCode, int &nCount);

    int getUpChannelId()
    {
        return m_upCurChannel;
    }

    int getDownChannelId()
    {
        return m_downCurChannel;
    }

    virtual int GetStatus()
    {
        if(!m_bBootUp)
        {
            return DEV_STATUS_Offline;
        }
        return m_nStatus;
    }

protected:
    //处理卡机事件
    void customEvent(QEvent *event);

protected:
    //加载驱动
    virtual bool LoadFunction();
    //卸载驱动
    virtual bool ReleaseFunction();
    //关闭
    virtual bool Close();
    //初始化
    virtual bool Init();
private:
    bool isUpPosition(int mgrIndex)
    {
        return (0 <= mgrIndex && mgrIndex  < 2);
    }
    bool isDownPosition(int mgrIndex)
    {
        return (2<= mgrIndex && mgrIndex < 4);
    }
    void handleCardMgrInfo(CardMgrEvent* event);
private:
    int m_upCurChannel;
    int m_downCurChannel;
    bool m_bBootUp;
};

#endif // CARDMGRARM_H
