#ifndef CARDINFOPROCESSOR_H
#define CARDINFOPROCESSOR_H
#include "CardReaderArm/qcardreader.h"

#include <QObject>

class CardInfoProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CardInfoProcessor(QObject *parent = 0);

signals:

public slots:
private:
    //入口信息
    CVehEntryInfo FVehEntryInfo;
    //读到的物理卡类型
    CARD_TYPE_PHYSICAL m_PhysicalCardType;
    //卡内通行信息
    TCardTollInfo FCardTollInfo;
    //通行卡基本信息
    TTollCardBasicInfo FTollCardBasicInfo;
    //CPU卡信息
    TProCardBasicInfo FProCardBasicInfo;
    //CPU卡余额
    quint32 FdwBalance;
    // 是否已经输入车牌
    bool m_bVehPlate;
public:
    //预处理通行卡(读取卡信息、并进行检查)
    bool PreprocessTollCard(QCardReader* Reader);
    //处理通行卡
    bool ProcessOpenCardEvent_TollCard(int nReaderId);
    //预处理CPU卡(读取卡信息、并进行检查)
    bool PreprocessProCard(QCardReader* Reader);
    //处理CPU卡
    bool ProcessOpenCardEvent_ProCard(int nReaderId);
    //设置卡片类型
    void SetPhyCardType(int nCardType);
    static bool ClearPassCardState();
    static bool ClearProCardState();
    //
    void clearSaveInfo();
private:
    //卡片校验
    bool CheckCardValidity();
    // 车辆信息比较
    //检查入口信息与操作输入信息一致性
    bool CheckEntryVehInfo();
    // 车辆输入信息与云通卡固定区信息比较
    bool CompareVehProCard(QString &sError);
private:
    //保存入口信息
    void SetEntryInfo();
    //保存出口信息
    void SetExitTollItem();
    //根据入口信息计算费率
    bool ProcEntryInfo();
};


CardInfoProcessor* getCardProcessor();
#endif // CARDINFOPROCESSOR_H

