#ifndef STATEMACHINETRANSITION_H
#define STATEMACHINETRANSITION_H

#include "MtcKey/MtcKeySignalTransition.h"
#include "MtcKey/MtcLaneState.h"
#include <QAbstractState>
#include "ParamModule/qoperparamfile.h"
#include "laneinfo.h"
#include "formmessage.h"
#include "entrycardinfoprocessor.h"
#include "formentryreverse.h"
//确认设备状态
class TranConfirmDev : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranConfirmDev(QState* target) :
        MtcKeySignalTransition(target)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//确认工班Transiton
class TranConfirmShift : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranConfirmShift(QState* target):
        MtcKeySignalTransition(target)
    {
    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};
class TranShowInvoiceMenu : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    static int currentIndex;
    TranShowInvoiceMenu(QState* target):
        MtcKeySignalTransition(target)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//选择换上票据Transiton
class TranChangeUpInvoice:public MtcKeySignalTransition
{
public :
    TranChangeUpInvoice(QState *target):MtcKeySignalTransition(target)
    {

    }
private:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};

//选择修改票据Transiton
class TranModifyInvoice:public MtcKeySignalTransition
{
public:
    TranModifyInvoice(QState *target):MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//打开雨棚灯Transiton
class TranTurnOnCanLight: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranTurnOnCanLight(QState* target):
        MtcKeySignalTransition(target)
    {
    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//上班时打开雨棚灯
class TranControlCanLight: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranControlCanLight(QState* target): MtcKeySignalTransition(target)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//票号确认完毕Transiton
class TranFinInvoice: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranFinInvoice(QState* target):
        MtcKeySignalTransition(target)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//返回票号确认Transiton
class TranRtInvConfirm:public MtcKeySignalTransition
{
public:
    TranRtInvConfirm(QAbstractState *target):MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};
//更改票据时的按enter返回到源界面
class TranEnRepBill:public MtcKeySignalTransition
{
public:
    TranEnRepBill(QAbstractState *target):MtcKeySignalTransition(target)
    {
    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};

//输入车牌完成或者，取消输入车牌Transition
class TranFinEnterPlate: public MtcKeySignalTransition
{
public:
    TranFinEnterPlate(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};


//输入车牌Transition
class TranEnterPlate: public MtcKeySignalTransition
{
public:
    TranEnterPlate(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//无卡处理Transition
class TranNoCard: public MtcKeySignalTransition
{
public:
    TranNoCard(QAbstractState* target) : MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//无卡选择中心
class TranChooseCenter: public MtcKeySignalTransition
{
public:
    TranChooseCenter(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//无卡选择收费站
class TranChooseSta: public MtcKeySignalTransition
{
public:
    TranChooseSta(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//无卡选择完成收费站，计算金额并跳转显示收费金额页面
class TranShowFeeByConfirm: public MtcKeySignalTransition
{
public:
    TranShowFeeByConfirm(QAbstractState* target): MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//无卡最大里程
class TranNoCardMaxDis: public MtcKeySignalTransition
{
public:
    TranNoCardMaxDis(QAbstractState* target): MtcKeySignalTransition(target)
    {}

protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//------------------------------------------------------------

//锁杆
class TranLockBar: public MtcKeySignalTransition
{
public:
    TranLockBar(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//计算出金额后云通卡Transition
class TranYunTongCard: public MtcKeySignalTransition
{
public:
    TranYunTongCard(QAbstractState* target):MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    int StoreConsume();
    void ExitConfirmComplete();
};

//计算出金额后现金收款处理
class TranCashHandle: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranCashHandle(QAbstractState* target): MtcKeySignalTransition(target), m_bPrint(true)
    {}
signals:
    void printSuccess();
    void printFail();
    void freePass();
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    bool PrintInvoiceInfo();
private slots:
    void timeOutToPrint();
private:
    TInvPrintInfo m_invPrtInfo;
    bool m_bPrint;
};

//票打成功处理
class TranPrintSuccess: public MtcKeySignalTransition
{
public:
    TranPrintSuccess(QObject * sender, const char * signal) : MtcKeySignalTransition(sender, signal)
    {
    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//免费车放行
class TranFreePass : public MtcKeySignalTransition
{
public:
    TranFreePass(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void onTransition(QEvent *event);
    bool eventTest(QEvent *event);
};

//票打失败处理
class TranPrintFail: public MtcKeySignalTransition
{
public:
    TranPrintFail(QObject * sender, const char * signal) : MtcKeySignalTransition(sender, signal)
    {
    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//放弃票打，放行处理
class TranCancelPrint: public MtcKeySignalTransition
{
public:
    TranCancelPrint(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//重打发票
class TranRePrint: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranRePrint(QAbstractState* target): MtcKeySignalTransition(target){}
public slots:
    void timeOutToPrint();
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    bool RePrint(int nType);
private:
    int m_nIndex;
};

//车辆通过转初始状态
class TranVehPass: public MtcKeySignalTransition
{
public:
    TranVehPass(QObject * sender, const char * signal) : MtcKeySignalTransition(sender, signal)
    {
    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//特殊车辆金额处理
class TranSpecialVeh: public MtcKeySignalTransition
{
public:
    TranSpecialVeh(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//其他免征处理
class TranOtherFree: public MtcKeySignalTransition
{
public:
    TranOtherFree(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//其他免征显示完毕
class TranUpdateFeeByOtherFree : public MtcKeySignalTransition
{
public:
    TranUpdateFeeByOtherFree(QAbstractState* target):MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//违章
class TranViolation : public MtcKeySignalTransition
{
public:
    TranViolation(QAbstractState* target) : MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//处理违章
class TranFinViolation : public MtcKeySignalTransition
{
public:
    TranFinViolation(QAbstractState* target) : MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//包交卡-刷卡
class TranMonthCard: public MtcKeySignalTransition
{
public:
    TranMonthCard(QObject * sender, const char * signal): MtcKeySignalTransition(sender, signal)
    {}
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//包交卡-公务车键
class TranMonthCardByOfficial: public MtcKeySignalTransition
{
public:
    TranMonthCardByOfficial(QAbstractState* target):MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//跳转到纸卡处理
class TranPaperCard:public MtcKeySignalTransition
{
public:
    TranPaperCard(QAbstractState* target): MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &/*keyPropertity*/, int /*keyType*/);
};

//纸卡输入号码错误
class TranPaperCardErr : public MtcKeySignalTransition
{
public:
    TranPaperCardErr(QObject* sender, const char* signal): MtcKeySignalTransition(sender, signal)
    {}
protected:
    void onTransition(QEvent *event);
    bool eventTest(QEvent *event);
};
//纸卡输入号码正确
class TranPaperCardSuccess: public MtcKeySignalTransition
{
public:
    TranPaperCardSuccess(QObject* sender, const char* signal):
        MtcKeySignalTransition(sender, signal)
    {}
protected:
    void onTransition(QEvent *event);
    bool eventTest(QEvent *event);
};

//坏卡选择坏卡类型
class TranChooseBadCardType: public MtcKeySignalTransition
{
public:
    TranChooseBadCardType(QAbstractState* target) : MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//坏卡卡号输入界面
class TranEnterCardNum : public MtcKeySignalTransition
{
public:
    TranEnterCardNum(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//坏卡输入完毕跳转选择入口号界面
class TranChooseEntrySta : public MtcKeySignalTransition
{
public:
    TranChooseEntrySta(QAbstractState* target): MtcKeySignalTransition(target)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//显示收费转到初始界面
class TranReturnInit : public MtcKeySignalTransition
{
public:
    TranReturnInit(QAbstractState* target) : MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

class TranErrorReInit : public MtcKeySignalTransition
{
public:
    TranErrorReInit(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {}
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

////等待过车到初始状态 #按键模拟
//class TranFinPassVehSim : public MtcKeySignalTransition
//{
//public:
//    TranFinPassVehSim(QAbstractState* target) : MtcKeySignalTransition(target)
//    {

//    }
//protected:
//    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
//    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
//};

////等待过车到初始状态 #线圈触发
//class TranFinPassVeh : public MtcKeySignalTransition
//{
//public:
//    TranFinPassVeh(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
//    {

//    }
//protected:
//    bool eventTest(QEvent *event);
//    void onTransition(QEvent *event);
//};

//车队
class TranMotor: public MtcKeySignalTransition
{
public:
    TranMotor(QAbstractState* target): MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

class TranMotorPass : public MtcKeySignalTransition
{
public:
    TranMotorPass(QAbstractState* target): MtcKeySignalTransition(target)
    {

    }
    TranMotorPass(QObject* sender, const char* signal):MtcKeySignalTransition(sender, signal)
    {

    }

protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//结束车队操作transition
class TranVehQueEnd : public MtcKeySignalTransition
{
public:
    TranVehQueEnd(QAbstractState *state):MtcKeySignalTransition(state)
    {}
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//F12 transition
class TranExitApp : public MtcKeySignalTransition
{
public:
    TranExitApp(QAbstractState *state):MtcKeySignalTransition(state)
    {}
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//选择退出程序transition
class TranSelectExitApp : public MtcKeySignalTransition
{
public :
    TranSelectExitApp(QAbstractState *state):MtcKeySignalTransition(state)
    {}
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
////选择重启程序
//class TranReStartApp : public MtcKeySignalTransition
//{
//public:
//    TranReStartApp(QAbstractState *state):MtcKeySignalTransition(state)
//    {}
//    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
//    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
//};
////选择关闭计算机
//class TranShutDown : public MtcKeySignalTransition
//{
//public :
//    TranShutDown(QAbstractState *state):MtcKeySignalTransition(state)
//    {}
//    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
//    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
//};
////选择重启计算机
//class TranRestartPC : public MtcKeySignalTransition
//{
//public :
//    TranRestartPC(QAbstractState *state):MtcKeySignalTransition(state)
//    {}
//    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
//    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
//};
//ESC 退出F12 transition
class TranQuitSelect : public MtcKeySignalTransition
{
public:
    TranQuitSelect(QAbstractState *state):MtcKeySignalTransition(state)
    {}
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//确认下班
class TranLogOut : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranLogOut(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
class TranFunction:public MtcKeySignalTransition
{
public:
    TranFunction(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
class TranAccedit:public MtcKeySignalTransition
{
public:
    TranAccedit(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }

protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};
class TranWeightToTable:public MtcKeySignalTransition
{
public:
    TranWeightToTable(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};
class TranCardToTable :public MtcKeySignalTransition
{
public:

    TranCardToTable(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};
class TranIstallToTable: public MtcKeySignalTransition
{
public:

    TranIstallToTable(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
class TranSetToCarInfo:public MtcKeySignalTransition
{
public:
    TranSetToCarInfo(QAbstractState *state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);

};

//打印发票成功后，判断是否还有发票剩余
class TranCheckInvNum: public MtcKeySignalTransition
{
public:
    TranCheckInvNum(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }
    TranCheckInvNum(QObject* sender, const char* signal):
        MtcKeySignalTransition(sender, signal)
    {

    }

protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};
//更换票据流程-------------------------------------------
//按键到输入票号的界面
class TranRepBill:public MtcKeySignalTransition
{
public:
    TranRepBill(QState *target):MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};
//更换卡盒流程
class TranRepCardBox:public MtcKeySignalTransition
{
public:
    TranRepCardBox(QState *target):MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//按绿通键处理绿通卡信息
class TranHandleFarm: public MtcKeySignalTransition
{
public:
    TranHandleFarm(QAbstractState* state): MtcKeySignalTransition(state)
    {}
    TranHandleFarm(QObject* sender, const char* signal):
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//确认绿通车类型
class TranConfirmFarm: public MtcKeySignalTransition
{
public:
    TranConfirmFarm(QAbstractState* state):MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//判断是否U型,如果超时则显示选择超时类型界面
class TranJudgeUType: public MtcKeySignalTransition
{
public:
    TranJudgeUType(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }
    TranJudgeUType(QObject* sender, const char* signal):
        MtcKeySignalTransition(sender, signal)
    {

    }

protected:
    void onTransition(QEvent *event);
    bool eventTest(QEvent *event);
};

//处理U型免费
class TranUTypeFree : public MtcKeySignalTransition
{
public:
    TranUTypeFree(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//处理U型选站
class TranUTypeChsSta : public MtcKeySignalTransition
{
public:
    TranUTypeChsSta(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//处理U型自动计算
class TranUTypeAuto : public MtcKeySignalTransition
{
public:
    TranUTypeAuto(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

//正常车辆，没有超时，正常计算通行费
class TranCalcFee: public MtcKeySignalTransition
{
    Q_OBJECT
signals:
    void calcFail();
public:
    TranCalcFee(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
    TranCalcFee(QObject* sender, const char* signal):
        MtcKeySignalTransition(sender, signal), m_bCalcError(false)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    bool m_bCalcError;
};

//超时车辆正常超时
class TranCalcOverTimeNormal: public MtcKeySignalTransition
{
public:
    TranCalcOverTimeNormal(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//超时车辆，可疑超时
class TranExcOverTime: public MtcKeySignalTransition
{
public:
    TranExcOverTime(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);

};

//用户刷卡
class TranReadCard : public MtcKeySignalTransition
{
public:
    TranReadCard(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//用户放卡按上班键
class TranShift : public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranShift(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }

    TranShift(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void showShiftInfo();
    QString getShiftString(const QString& sShiftName, const QDate &workDate);

private:
};

class TranCardBox: public MtcKeySignalTransition
{
    Q_OBJECT
public:
    TranCardBox(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
    TranCardBox(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

class TranOffical: public MtcKeySignalTransition
{
public:
    TranOffical(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &/*keyPropertity*/, int /*keyType*/);
};

//拖车处理
class TranTract : public MtcKeySignalTransition
{
public:
    TranTract(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
class TranFinRead : public MtcKeySignalTransition
{
public:
    TranFinRead(QObject* sender, const char* signal) : MtcKeySignalTransition(sender,signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

class TranChangeCard : public MtcKeySignalTransition
{
public:
    TranChangeCard(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

class TranRushLogout : public MtcKeySignalTransition
{
public:
    TranRushLogout(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

class TranRushLogin : public MtcKeySignalTransition
{
public:
    TranRushLogin(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent* event);
    void onTransition(QEvent *event);
};

//功能按键
class TranLogOutFunc : public MtcKeySignalTransition
{
public:
    TranLogOutFunc(QAbstractState* state) : MtcKeySignalTransition(state), m_nType(0)
    {

    }

    TranLogOutFunc(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal), m_nType(0)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
private:
    int m_nType;
};

//超长车处理
class TranLongVeh : public MtcKeySignalTransition
{
public:
    TranLongVeh(QAbstractState* state) : MtcKeySignalTransition(state), m_bIsLongVeh(false)
    {

    }
    TranLongVeh(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal), m_bIsLongVeh(false)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
private:
    bool m_bIsLongVeh;
};

//计重明细查看
class TranWeightDetail : public MtcKeySignalTransition
{
public:
    TranWeightDetail(QAbstractState* state): MtcKeySignalTransition(state)
    {}
    TranWeightDetail(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

class TranDisInvoice : public MtcKeySignalTransition
{
public:
    TranDisInvoice(QAbstractState* state) : MtcKeySignalTransition(state)
    {

    }
    TranDisInvoice(QObject* sender, const char* signal) : MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};

//切换车牌识别界面
class TranChangeVpr : public MtcKeySignalTransition
{
public:
    TranChangeVpr(QAbstractState* state) : MtcKeySignalTransition(state)
    {}
protected:
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
};
//入口纸卡处理
class TranEntryPaperCard:public MtcKeySignalTransition
{
public:
    TranEntryPaperCard(QAbstractState* target): MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &/*keyPropertity*/, int /*keyType*/);
};

//入口放行
class TranEntryAllowPass : public MtcKeySignalTransition
{
public:
    TranEntryAllowPass(QObject* sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};
//入口倒车
class TranEntryReverse:public MtcKeySignalTransition
{
public:
    TranEntryReverse(QAbstractState* target):MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity,int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);


};

//入口写卡
class TranWriteCard : public MtcKeySignalTransition
{
public:
    TranWriteCard(QObject* sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};
//入口补卡
class TranRepairCard : public MtcKeySignalTransition
{
public:
    TranRepairCard(QAbstractState* target):MtcKeySignalTransition(target)
    {
    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity,int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);


};


////////////////////////////////////自动发卡专用/////////////////////////////////////

//卡机出现异常
class TranAutoException : public MtcKeySignalTransition
{
public:
    TranAutoException(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//卡机恢复正常
class TranAutoNormal : public MtcKeySignalTransition
{
public:
    TranAutoNormal(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//自动发卡锁杆
class TranAutoLockBar : public MtcKeySignalTransition
{
public:
    TranAutoLockBar(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

////自动发卡车队开始
//class TranAutoMotor : public MtcKeySignalTransition
//{
//public:
//    TranAutoMotor(QObject *sender, const char* signal) :
//        MtcKeySignalTransition(sender, signal)
//    {

//    }
//protected:
//    bool eventTest(QEvent *event);
//    void onTransition(QEvent *event);
//};

////自动发卡车队过车
//class TranAutoMotorPass : public MtcKeySignalTransition
//{
//public:
//    TranAutoMotorPass(QObject *sender, const char* signal) :
//        MtcKeySignalTransition(sender, signal)
//    {

//    }
//protected:
//    bool eventTest(QEvent *event);
//    void onTransition(QEvent *event);
//};

////自动发卡车队结束
//class TranAutoMotorEnd : public MtcKeySignalTransition
//{
//public:
//    TranAutoMotorEnd(QObject *sender, const char* signal) :
//        MtcKeySignalTransition(sender, signal)
//    {

//    }
//protected:
//    bool eventTest(QEvent *event);
//    void onTransition(QEvent *event);
//};


//自动发卡写卡成功
class TranAutoWriteSuccess : public MtcKeySignalTransition
{
public:
    TranAutoWriteSuccess(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//自动发卡写卡失败
class TranAutoWriteFail : public MtcKeySignalTransition
{
public:
    TranAutoWriteFail(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//车辆放行
class TranAutoAllowPass : public MtcKeySignalTransition
{
public:
    TranAutoAllowPass(QObject *sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//自动发卡下班
//class TranAutoLogOut : public MtcKeySignalTransition
//{
//public:
//    TranAutoLogOut(QObject *sender, const char* signal) :
//        MtcKeySignalTransition(sender, signal)
//    {

//    }
//protected:
//    bool eventTest(QEvent *event);
//    void onTransition(QEvent *event);
//};
//自动发卡下班
class TranAutoLogOut : public MtcKeySignalTransition
{
public:
    TranAutoLogOut(QAbstractState* state): MtcKeySignalTransition(state)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//自动卡机车队
class TranAutoMotor: public MtcKeySignalTransition
{
public:
    TranAutoMotor(QAbstractState* target): MtcKeySignalTransition(target)
    {

    }
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

class TranAutoMotorPass : public MtcKeySignalTransition
{
public:
    TranAutoMotorPass(QAbstractState* target): MtcKeySignalTransition(target)
    {

    }
    TranAutoMotorPass(QObject* sender, const char* signal):MtcKeySignalTransition(sender, signal)
    {

    }

protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//结束自动卡机车队操作transition
class TranAutoVehQueEnd : public MtcKeySignalTransition
{
public:
    TranAutoVehQueEnd(QAbstractState *state):MtcKeySignalTransition(state)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};

class TranEntryConfirmShift : public MtcKeySignalTransition
{
public:
    TranEntryConfirmShift(QAbstractState *state):MtcKeySignalTransition(state)
    {}
protected:
    void mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType);
    void onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType);
};
//自动发卡Pro写卡
class TranAutoWriteCard : public MtcKeySignalTransition
{
public:
    TranAutoWriteCard(QObject* sender, const char* signal) :
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

//判断存储空间，如果存储空间低于标准值，则提醒下班
class TranCheckFreeDisk : public MtcKeySignalTransition
{
public:
    TranCheckFreeDisk(QObject *sender, const char *signal):
        MtcKeySignalTransition(sender, signal)
    {

    }
protected:
    bool eventTest(QEvent *event);
    void onTransition(QEvent *event);
};

#endif // STATEMACHINETRANSITION_H

