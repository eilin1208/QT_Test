#include "formbase.h"
#include "ui_formordinartprocess.h"
#include <QTextEdit>
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include "devicefactory.h"
#include "formmainwindow.h"
#include "MtcKey/MtcKeySignalTransition.h"
#include "MtcKey/MtcLaneState.h"
#include "statemachinetransition.h"
#include "dialogshowproinfo.h"
#include "formwtdetail.h"
FormBase* g_ord = NULL;
FormBase* getOrdinaryWidget()
{
    return g_ord;
}
FormBase::FormBase(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormOrdinartProcess)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    g_ord = this;
    m_parentState = NULL;
    m_nextState = NULL;
    m_initState = NULL;
    m_funcState = NULL;
    m_setupState = NULL;
    m_finState = NULL;
    m_motorState = NULL;
    m_rushState = NULL;
    m_pPlate = NULL;
    m_pVehInfo = NULL;
    m_pTable = NULL;
    m_pMessage = NULL;
    m_pMotor = NULL;
}

FormBase::~FormBase()
{
    ////qDebug() << "release ....... " << objectName();
    delete ui;
}

////清除界面所有信息
void FormBase::clearInfo()
{
    //车型窗口信息清空
    ui->labEnterTime->clear();
    ui->labEnterPlate->clear();
    ui->labEnterStation->clear();
    ui->labCardNum->clear();
    ui->labWorkNum->clear();
    ui->labDriveTime->clear();
    ui->labFlagStation->clear();
}

void FormBase::setEntryInfo(const QDateTime &entryTime, const QString &sPlate, const QString &sStation, const QString &sCardNo, const QString &sWorkNo, const QString &driveTime, const QString &sFlag)
{
    ui->labEnterTime->setText(entryTime.toString("yyyy-MM-dd hh:mm:ss"));
    ui->labDriveTime->setText(driveTime);
    ui->labEnterPlate->setText(sPlate);
    ui->labWorkNum->setText(sWorkNo);
    ui->labCardNum->setText(sCardNo);
    ui->labFlagStation->setText(sFlag);
    ui->labEnterStation->setText(sStation);
}

void FormBase::hideEntryInfo()
{
    ui->widget->hide();
}

void FormBase::showEntryInfo()
{
    ui->widget->show();
}

void FormBase::initStateMachine(QState *parent, QState *next)
{
    m_parentState = parent;
    m_nextState = next;
    createChildrenWidget();
    createStates();
    buildSubStateMachine();
}

void FormBase::createChildrenWidget()
{
    m_widget = ui->widget_2;
    m_layout = new QVBoxLayout(ui->widget_2);
    m_pVehInfo = new FormEntryVehInfo(ui->widget_2, m_layout);
    m_pTable = new FormTableWidget(ui->widget_2, m_layout);
    m_pMessage = getMsgWidget(m_widget, m_layout);
    getShowProInfo(m_widget, m_layout);
    getShowEnPlate(m_widget, m_layout);
    m_pMotor = new FormVehQueBegin(ui->widget_2,m_layout);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
}

void FormBase::buildSubStateMachine()
{
    //下班
    m_initState->addTransition(new TranLogOut(m_nextState));
    //功能
    m_initState->addTransition(new TranFunction(m_funcState));
    //超长车处理
    m_funcState->addTransition(new TranLongVeh(m_initState));
    //返回初始页面
    m_funcState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));
    //跳转到设置页面
    m_funcState->addTransition(new TranIstallToTable(m_setupState));
    //更换票据
    m_funcState->addTransition(new TranRepBill(m_initState));
    //返回初始页面
    m_setupState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));
    //设置内容页面跳转到车辆信息页面
    m_setupState->addTransition(new TranSetToCarInfo(m_initState));
    //编辑计重
    m_funcState->addTransition(new TranAccedit(m_initState));
    //重连读卡器
    m_funcState->addTransition(new TranCardToTable(m_initState));
    //更换卡盒
    m_funcState->addTransition(new TranRepCardBox(m_initState));
    //车队开始状态
    m_initState->addTransition(new TranMotor(m_motorState));
    //车队过车
    QSignalTransition* tMotorPass = new TranMotorPass(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)));
    tMotorPass->setTargetState(m_motorState);
    m_motorState->addTransition(tMotorPass);
    //结束车队操作
    m_motorState->addTransition(new TranVehQueEnd(m_initState));
    //修改车牌
    m_initState->addTransition(new TranEnterPlate(m_initState));
    //初始状态其他免征
    m_initState->addTransition(new TranOtherFree(m_otherState));
    m_otherState->addTransition(new TranUpdateFeeByOtherFree(m_initState));
    //初始状态其他免征返回
    m_otherState->addTransition(new SpecialKeySignalTransition(m_initState, KeyEsc, KC_Func));
    //初始状态闯关
    m_initState->addTransition(new TranViolation(m_rushState));
    m_rushState->addTransition(new TranFinViolation(m_initState));

    m_initState->addTransition(new TranSpecialVeh(m_initState));

    //锁杆
    m_initState->addTransition(new TranLockBar(m_initState));

}

void FormBase::createStates()
{
    m_initState = new MtcLaneState(m_parentState, m_pVehInfo);
    m_funcState = new MtcLaneState(m_parentState, m_pTable);
    m_setupState = new MtcLaneState(m_parentState, m_pTable);
    m_finState = new MtcLaneState(m_parentState, m_pVehInfo);
    m_motorState = new MtcLaneState(m_parentState, m_pMotor);
    m_otherState = new MtcLaneState(m_parentState, m_pTable);
    m_rushState = new MtcLaneState(m_parentState, m_pTable);
    m_parentState->setInitialState(m_initState);

}


