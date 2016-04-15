#ifndef FORMEXIT_H
#define FORMEXIT_H
#include "formbase.h"
#include "forminformid.h"
#include "formmodifyweight.h"
#include "formcalchange.h"
#include "formpasscard.h"
#include "formbadcard.h"
#include "formpapercard.h"
#include "formwtdetail.h"
#include "dialogshowproinfo.h"
class FormExit : public FormBase
{
    Q_OBJECT
public:
    FormExit(QWidget* widget = 0, QLayout* layout = 0);
protected:
    virtual void createStates();
    virtual void createChildrenWidget();
    virtual void buildSubStateMachine();
private:
    //出口独用窗口
    //票号编辑
    ForminformId* m_pInvoice;
    //编辑计重信息
    FormModifyWeight* m_pModifyWeight;
    //包交证界面
    FormPassCard* m_pPassCard;
    //坏卡界面
    FormBadCard* m_pBadCard;
    //找零计算
    FormCalChange* m_pCalChange;
    //纸卡-exit
    FormPaperCard* m_pPaperCard;
private:
    //判断超时
    MtcLaneState* m_overTimeState;
    //缺纸
    MtcLaneState* m_noInvState;
    //显示金额
    MtcLaneState* m_showFeeState;
    //无卡
    MtcLaneState* m_noCardState;
    //无卡分中心
    MtcLaneState* m_centerState;
    //无卡站
    MtcLaneState* m_stationState;
    //打印失败
    MtcLaneState* m_prtFailState;
    //打印中
    MtcLaneState* m_prtingState;
    //找零计算
    MtcLaneState* m_calChangeState;
    //判断U型
    MtcLaneState* m_uTypeState;
    //纸卡处理
    MtcLaneState* m_paperState;
    //坏卡类型
    MtcLaneState* m_badTypeState;
    //坏卡卡号输入
    MtcLaneState* m_badNumState;
    //完成卡读写
    MtcLaneState* m_finReadState;
    //选择其他免征
    MtcLaneState* m_otherFeeState;
    //绿通车初始状态
    MtcLaneState* m_farmState;
    MtcLaneState* m_farmFeeState;
    //闯关
    MtcLaneState* m_rushFeeState;
};

#endif // FORMEXIT_H
