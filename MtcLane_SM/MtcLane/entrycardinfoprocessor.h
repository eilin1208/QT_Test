/*****************************************************************************
                          *本单元说明：入口输入状态机
                          liu_bo
 ****************************************************************************/
#ifndef ENTRYCARDINFOPROCESSOR_H
#define ENTRYCARDINFOPROCESSOR_H
#include <QObject>
#include "CardReaderArm/qcardreader.h"
class Entrycardinfoprocessor : public QObject
{
    Q_OBJECT
public:
    explicit Entrycardinfoprocessor(QObject * parent = 0);

public:
    //处理车牌识别事件
    bool ProcessVPRDetectEvent(int nIndex);//override;
    //处理CPU卡
    bool Processopencardevent_Procard(int nReaderId);
    //处理通行卡
    bool ProcessOpenCardEvent_TollCard(int nReaderId);
    //写通CPU卡-wxk
    bool WriteProCard();
    void SetPhyCardType(int nCardType)
    {
        m_PhysicalCardType = (CARD_TYPE_PHYSICAL)nCardType;
    }
    //写卡成功，放行车辆
    void EntryWriteCardSucComplate();
    //自动卡机设置卡盒编号
    void setCardBoxId(int boxId);
    bool Prompt();
private:
    //       frameMain:TFrameEntry;
    // 是否已经输入车牌
    bool m_bVehPlate;
    //卡片物理类型
    CARD_TYPE_PHYSICAL  m_PhysicalCardType;
    //M1卡片基本信息
    TTollCardBasicInfo m_TollCardBasicInfo;
    //CPU卡片基本信息
    TProCardBasicInfo m_ProCardBasicInfo;
    //CPU卡余额
    quint32 m_dwBalance;
    //卡片通行信息
    TCardTollInfo m_CardTollInfo;

private:

    //处理刷卡
    bool ProcessCardEvent(int nReaderId);

    // 读CPU卡并判断卡片可用性
    bool ReadProCard(int nReaderId);
    // 绑定车辆信息的云通卡与输入的车辆信息比较
    bool CompareVehProCard();
    //填写写卡的入口信息
    void SetEntryInfo();
    //保存写卡的入口信息
    void SaveEntryTollItem();
    //判断车辆信息输入情况，并进行信息提示
    //    void Prompt();TODO_liubo


};

Entrycardinfoprocessor* getEntryCardProcessor();

#endif // ENTRYCARDINFOPROCESSOR_H
