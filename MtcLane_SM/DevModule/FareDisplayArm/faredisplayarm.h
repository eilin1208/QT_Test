#ifndef FAREDISPLAYARM_H
#define FAREDISPLAYARM_H
#include "BaseInterface/devbaseinterface.h"

enum SHORT_MSG_TYPE
{
    SM_Welcome,                     //欢迎信息，标记一辆车的开始，如“欢迎行驶高速公路”、“请保管好通行卡”
    SM_Error,                         //错误信息，如ETC车道“余额不足”
    SM_Prompt,                      //提示信息，如“请保管好通行卡”
    SM_Goodbye,                     //标记一辆车的结束，如“雨天路滑"，注意安全”
    SM_NoVeh,                         //空闲，清屏或显示其它类型的欢迎信息，如“ETC专用车道”
    SM_Other                          //其它类型的信息
};

enum MSG_COLOR
{
    SM_Blue = 0,
    SM_Red,
    SM_Yellow
};

class FareDisplayArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit FareDisplayArm(const QString& sName, QObject *parent = 0);

signals:

public slots:

public:
    //设置工作模式
    bool SetMode(int nLaneType,bool bTollByWeight = true);
    //显示车型
    bool ShowVehClass(int nVehClass, bool bIsNewVeh = false,bool bSynData = true);
    //显示车货重量、限载、超限率
    bool ShowWeightInfo(int nTotalWeight, int nWeightLimit, int nOverWeightRate,bool bSynData = true);
    //显示应收金额:nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值
    bool ShowFare(int nFare, bool bSynData = true);
    //显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围>，应清空，不应显示错误值）
    bool ShowBalanceBeforeDeduct(int nBalanceBeforeDeduct, bool bIsTallyCard= false, bool bSynData = true);
    //显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示>器显示范围，应清空，不应显示错误值）
    bool ShowBalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard = false, bool bSynData = true);
    //显示入口站
    bool ShowEntrySta(char * szEntrySta,bool bSynData = true);
    //显示文字
    bool ShowText(char* szText, SHORT_MSG_TYPE nMsgType,bool bSynData = true);
    //同步消息
    bool SynData();
    //清空所有
    bool ClearAll();
    //进行报警 - 启动报警延时一段时间自动关闭，默认5000毫秒，参数为0时表示不自动停止报警
    bool Alarm(bool bAlarm,int nAutoStopTime =5000);
    //只进行声音报警
    bool AlarmSound(bool bAlarm,int nAutoStopTime =5000);
    //只进行闪光报警
    bool AlarmLight(bool bAlarm,int nAutoStopTime =5000);
    //设置亮度
    bool SetBrightness(int nBrightness);
    //设置通行信息灯
    bool SetPassLight(bool bGreen);
public:
    //通知费额显示器进行新的车辆处理
    void NotifyNewVeh();
    // 通知费额显示器车辆已经离开
    void NotifyVehPassed();
protected:
    virtual bool LoadFunction();
    virtual bool ReleaseFunction();
    virtual bool Init();
    virtual bool Close();
    virtual void timerEvent(QTimerEvent *event);
private:
    //是否有新车辆开始处理
    bool m_bNewVehStarted;
    //车道类型
    int m_nLaneType;
    //是否计重收费
    bool m_bTollByWeight;
    //欢迎语
    QString m_sWelcome;
    //定时器ID
    int m_timerId;
};
#endif // FAREDISPLAYARM_H
