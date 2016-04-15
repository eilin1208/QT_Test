#include "statemachinemgr.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qsysparadicparamfile.h"
#include <QTimer>
#include "passwordverifytransition.h"
#include"forminformid.h"
#include "statemachinetransition.h"
#include "formmainwindow.h"
#include "formmtctitle.h"
#include "devicefactory.h"
#include "lanectrl.h"
#include "PacketSendModule/sqlitedatasaver.h"
#include "PacketSendModule/monclient.h"
#include "PacketSendModule/qdatasendmgr.h"
#include "tcpserver.h"
#include "reqparam.h"
#include "formcardbox.h"
#include "formauth.h"
#include "formbottombar.h"
#include "ParamModule/qparammgr.h"
#include "dialogshowproinfo.h"
#include "formdevicestate.h"
#include "shiftsum.h"
#include "WtArm/wtdatamgrunit.h"
#include "formexit.h"
#include "formentry.h"
#include "formautocard.h"
#include "formregulatetime.h"
StateMachineMgr::StateMachineMgr(QObject *parent) :
    QObject(parent)
{
    m_pFormMsg = NULL;
    m_pFormLoadParam = NULL;
    m_pTableWidget = NULL;
    //m_informTK = NULL;
    m_pOrdWidget = NULL;
    //车队流程处理状态
    m_QueProcess = NULL;
}

StateMachineMgr::~StateMachineMgr()
{
}

void StateMachineMgr::buildStateMachine(QLayout* layout,bool bIsEntryLane)
{
    if(bIsEntryLane)
    {
        buildEntryWindows(layout);
    }
    else
    {
        buildExitWindows(layout);
    }
    //设备查看状态
    QState * devShowState = new MtcLaneState(&m_machine, m_devTable);
    //初始化状态
    QState * initState = new MtcLaneState(&m_machine, m_pFormLoadParam);
    //参数加载成功状态
    QState * loadSuccess = new MtcLaneState(&m_machine,m_pFormMsg);
    //加载参数成功时页面显示设置
    loadSuccess->assignProperty(m_pFormMsg, "message", tr("请放置身份卡\n按『上/下班』键登录"));
    loadSuccess->assignProperty(m_pFormMsg, "title", tr(""));
    //选择程序操作：退出程序、重启程序、关闭计算机、重启计算机
    QState *exitState = new MtcLaneState(&m_machine,m_pTableWidget);
    //工班选择状态
    QState * shiftState = new MtcLaneState(&m_machine, m_pTableWidget);
    //选择票号修改状态
    QState *IdTKstate=new MtcLaneState(&m_machine,m_pTableWidget);
    //修改票号页面状态
    QState *IdInform=new MtcLaneState(&m_machine,getInformWidget());
    //确认票号状态
    QState *confirmInvState = new MtcLaneState(&m_machine, m_pFormMsg);
    //正常上班状态
    QState* ordState = new MtcLaneState(&m_machine, m_pOrdWidget);
    //提示切换雨棚灯状态
    QState * turnOnLightState = new MtcLaneState(&m_machine, m_pFormMsg);

    turnOnLightState->assignProperty(m_pFormMsg, "message", tr("请按【雨棚开关】键\n将雨棚灯切换至绿灯"));
    //设备状态确认->初始化状态
    devShowState->addTransition(new TranConfirmDev(initState));
    //初始化状态->参数加载成功状态
    LoadParamTransition *tLoadParam = new LoadParamTransition(initState,SIGNAL(entered()));
    tLoadParam->setTargetState(loadSuccess);
    initState->addTransition(tLoadParam);
    //加载成功状态->选择程序退出状态
    TranExitApp *exitTrans = new TranExitApp(exitState);
    loadSuccess->addTransition(exitTrans);
    //加载成功状态->闯关tran->加载成功状态
    addRushTranLogOut(loadSuccess);
    //选择程序退出状态->执行退出操作
    TranSelectExitApp *exitAppTrans = new TranSelectExitApp(loadSuccess);
    exitState->addTransition(exitAppTrans);
    //退出程序操作，返回初始化界面
    TranQuitSelect *quitselTrans = new TranQuitSelect(loadSuccess);
    exitState->addTransition(quitselTrans);
    //选择工班时出现闯关
    addRushTranLogOut(shiftState);


    //上班后出现闯关
    QHistoryState* rushState = new QHistoryState(ordState);
    TranRushLogin* tranLogin = new TranRushLogin(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)));
    tranLogin->setTargetState(rushState);
    ordState->addTransition(tranLogin);
    //上班后收费员按F12键，切换菜单
    QHistoryState* showLogState = new QHistoryState(ordState);
    TranChangeVpr* tranVpr = new TranChangeVpr(showLogState);
    ordState->addTransition(tranVpr);

    //建立上班后子状态机
    m_pOrdWidget->initStateMachine(ordState, loadSuccess);
    //加载参数成功->用户验证身份
    TranShift *tShowPassword = new TranShift(shiftState);
    loadSuccess->addTransition(tShowPassword);

    //打开雨棚灯状态闯关
    addRushTranLogOut(turnOnLightState);
    //打开雨棚灯状态->正常上班状态
    TranTurnOnCanLight* tOrd = new TranTurnOnCanLight(ordState);
    turnOnLightState->addTransition(tOrd);

    //确认卡盒卡ID操作，模拟实现
    TranCardBox *tGotoLight = new TranCardBox(turnOnLightState);

    //入口没有票号处理
    if(getLaneInfo()->isEntryLane())
    {
        shiftState->addTransition(new TranEntryConfirmShift(turnOnLightState));
    }
    else
    {
        //下班时票号操作
        QState *logOutIdState=new MtcLaneState(&m_machine,m_pTableWidget);
        QState *logOutInform=new MtcLaneState(&m_machine,getInformWidget());
        loadSuccess->addTransition(new TranShowInvoiceMenu(logOutIdState));
        logOutIdState->addTransition(new TranModifyInvoice(logOutInform));
        logOutIdState->addTransition(new TranChangeUpInvoice(logOutInform));
        logOutInform->addTransition(new TranFinInvoice(loadSuccess));
        logOutIdState->addTransition(new TranRtInvConfirm(loadSuccess));
        logOutInform->addTransition(new TranRtInvConfirm(loadSuccess));

        //确认班次，用户从班次中选择一个班次上班，班次记录到LaneInfo
        TranConfirmShift *tConfirm = new TranConfirmShift(confirmInvState);
        shiftState->addTransition(tConfirm);
        //修改票号时出现闯关
        addRushTranLogOut(IdTKstate);
        //修改票号时出现闯关
        addRushTranLogOut(IdInform);
        //确认票号状态闯关
        addRushTranLogOut(confirmInvState);
        //选择票号处理菜单，包括票据换上、修改票号
        TranShowInvoiceMenu * tMessIDTick = new TranShowInvoiceMenu(IdTKstate);
        confirmInvState->addTransition(tMessIDTick);
        //显示插入票号页面
        TranChangeUpInvoice *tInsertTK=new TranChangeUpInvoice(IdInform);//跳转到票号输入页面
        IdTKstate->addTransition(tInsertTK);
        //显示修改票号页面
        TranModifyInvoice *tInformTK=new TranModifyInvoice(IdInform);
        IdTKstate->addTransition(tInformTK);
        //完成票号操作，跳转票号确认状态
        TranFinInvoice *tShowInform=new TranFinInvoice(confirmInvState); //从页面输入和修改跳转到票号页面
        IdInform->addTransition(tShowInform);
        //输入修改票号跳转
        TranRtInvConfirm *treturntable=new TranRtInvConfirm(confirmInvState);
        IdInform->addTransition(treturntable);
        IdTKstate->addTransition(new TranRtInvConfirm(confirmInvState));
        //显示确认卡盒卡界面
        confirmInvState->addTransition(tGotoLight);
    }
    //返回等待上班状态
    shiftState->addTransition(new SpecialKeySignalTransition(loadSuccess, KeyEsc, KC_Func));
    confirmInvState->addTransition(new SpecialKeySignalTransition(loadSuccess, KeyEsc, KC_Func));
    //跳转到功能页面
    m_machine.setInitialState(devShowState);
    m_machine.start();
    //上班前功能界面
    loadSuccess->addTransition(new TranLogOutFunc(loadSuccess));
    QTimer::singleShot(1000, this, SLOT(beginInitState()));
}

void StateMachineMgr::buildEntryWindows(QLayout* layout)
{
    m_pFormMsg = getMsgWidget(NULL, layout);
    getAuth(NULL, layout);
    m_pFormLoadParam = getLoadParam(NULL, layout);
    getPasswordVerify(NULL, layout);
    //m_pTableWidget = new FormTableWidget(NULL, layout);
    m_pTableWidget = getTableWidget(NULL, layout);
    //m_informTK = new ForminformId(NULL,layout);
    if(getLaneInfo()->isAutoLane())
    {
        m_pOrdWidget = new FormAutoCard(NULL, layout);
    }
    else
    {
        m_pOrdWidget = new FormEntry(NULL, layout);
    }
    getFuncForm(NULL, layout);
    getCardBox(NULL, layout);
    m_devTable = new FormDeviceTable(NULL, layout);
    //增加校时功能界面
    getRegulateTimeForm(NULL, layout);
}

void StateMachineMgr::buildExitWindows(QLayout* layout)
{
    m_pFormMsg = getMsgWidget(NULL, layout);
    getAuth(NULL, layout);
    //m_pFormLoadParam = new FormLoadParam(NULL, layout);
    m_pFormLoadParam = getLoadParam(NULL, layout);
    getPasswordVerify(NULL, layout);
    //m_pTableWidget = new FormTableWidget(NULL, layout);
    m_pTableWidget = getTableWidget(NULL, layout);
    //m_informTK = new ForminformId(NULL,layout);
    m_pOrdWidget = new FormExit(NULL, layout);
    getFuncForm(NULL, layout);
    getCardBox(NULL, layout);
    getInformWidget(NULL, layout);
    m_devTable = new FormDeviceTable(NULL, layout);
    //增加校时功能界面
    getRegulateTimeForm(NULL, layout);
}

StateMachineMgr* getStateMgr()
{
    static StateMachineMgr mgr;
    return &mgr;
}

bool LoadParamTransition::eventTest(QEvent *event)
{
    //创建参数文件夹_liubo
    QDir nQdir(getCfgFilePath());
    if(!nQdir.exists())
    {
        qDebug() << "load param event test.................";

        QDir *temp = new QDir;
        bool ok = temp->mkdir(getCfgFilePath());
        if(ok)
        {
            //qDebug()<<"参数文件夹创建成功";
        }
    }
    if(!QSignalTransition::eventTest(event))
        return false;
    //连接加载参数对象和下载参数对象
    //connect(getParamMgr(), SIGNAL(FileLoadFail(int)), getReqParam(), SLOT(FileLoadFail(int)));
    connect(getParamMgr(), SIGNAL(AskFile(int,bool)), getReqParam(), SLOT(AskFile(int,bool)));
    QEventLoop q;
    getParamMgr()->LoadAllParamFile();
    //当所有参数都加载完成，开始下载需要下载的参数
    connect(getParamMgr(), SIGNAL(AllFileLoadFinished()), getReqParam(), SLOT(StartAskParamFiles()));
    connect(getReqParam(), SIGNAL(ParamFilesLoadSuccess()), &q, SLOT(quit()));
    q.exec();
    return true;
}

void LoadParamTransition::onTransition(QEvent */*event*/)
{
    //加载参数完成后，根据systeminfo，orgcode查询本站站编号，车道编码是否合法
    QSysParaDicParamFile* pSys =(QSysParaDicParamFile*)getParamMgr()->GetParamFile(cfSysParaDic);
    QOrgCodeParamFile * orgCode = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue orgValue;
    bool ret = orgCode->queryOrgCodeValue(orgValue, pSys->GetSysParamInfo().m_nStationID);
    if(!ret)
    {
        QMessageBox::warning(NULL, tr("错误"), tr("加载参数错误，机构参数中查询不到本站代码:%1")
                             .arg(pSys->GetSysParamInfo().m_nStationID));
        LogMsg("lane", tr("加载参数错误，机构参数中查询不到本站代码:%1")
               .arg(pSys->GetSysParamInfo().m_nStationID));

        //程序退出
        QApplication::exit(-1);
        return;
    }
    //保存orgvalue
    getLaneInfo()->setOrgInfo(orgValue);

    //设置计重
    getWtDataMgr()->setWtDev(getDeviceFactory()->getWtDev());

    //界面显示站名、车道号
    FormMTCTitle* pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);
    pTitle->setStaName(orgValue.sOrgSName);
    pTitle->setStaType(getLaneInfo()->getLaneType(), getLaneInfo()->getLaneCode());
    FormBottomBar* pBottom = NULL;
    getWidgetByType(emFormBottomBar, (QWidget**)&pBottom);
    //出口显示费率版本
    if(!getLaneInfo()->isEntryLane())
    {
        QBaseParamFile* pAll = getParamMgr()->GetParamFile(cfAllRate);
        if(pBottom)
        {
            pBottom->setRateVersion(pAll->GetVersionCode());
        }
    }

    //打开数据库
    if(!getDataSaver()->Init(getLaneInfo()->getOrgInfo().sOrgCode.toInt(), getLaneInfo()->getLaneCode()))
    {
        ////qDebug() << tr("初始化业务数据库失败");
        QMessageBox::warning(NULL, tr("错误"), tr("初始化业务数据库失败"));
        qApp->quit();
        return;
    }
    LogMsg("lane", tr("初始化业务数据库成功"));
    //获取大工班
    if(!getShiftSum()->LoadFromFile())
    {
        LogMsg("lane", tr("读取工班数据失败，工班合计清零！"));
    }
    else
    {
        LogMsg("lane", tr("读取工班数据成功！"));
    }
    //显示未上传的数据条数
    pBottom->ShowUnUpWaste();
    //初始化LaneCtrl
    getLaneCtrl()->Init();
    FormDeviceState* devState = NULL;
    getWidgetByType(emFormDevice,(QWidget**)&devState);
    //devState->Init();
    //初始化报文发送对象
    getDataSendMgr()->SetDataSaver(getDataSaver());
    getDataSendMgr()->InitSendDataClient(getLaneInfo()->getDataSrvIp(), getLaneInfo()->getDataSrvPort());
    //初始化监控对象
    getMon()->WriteServerIP(getLaneInfo()->getMonSrvIp());
    getMon()->WriteServerPort(getLaneInfo()->getMonSrvPort());
    getMon()->SetStaId(getLaneInfo()->getOrgInfo().sOrgCode.toInt());
    getMon()->SetLaneId(getLaneInfo()->getLaneCode());
    //getMon()->Resume();
    devState->Init();
    //初始化数据消息接收对象（被动校时，接收参数文件）
    getLaneMsgServer()->init(MSG_LISTEN_PORT);

    //加载票据卡盒
    bool bRet = getDataFileMgr()->Init();
    if(bRet)
    {
        LogMsg("lane", tr("加载票据、卡盒记录成功！"));
    }
    else
    {
        LogMsg("lane", tr("加载票据、卡盒记录失败！"));
    }

    //设置大班记录
    getLaneInfo()->UpdateShiftDateWhenUnlogin();

    //发送程序启动报文
    getLaneCtrl()->getTollItem()->SendStartMsg();

    //启动定时器,(参数判断是否生效，校时判断，参数文件更新,更新大工班)
    getLaneCtrl()->StartNewParamTimer();
    getLaneCtrl()->StartDownLoadTimer();
    getLaneCtrl()->StartVerifyTimer();
    getLaneCtrl()->StartBigShiftTimer();
    //liujian
    getLaneCtrl()->StartDelDBDataTimer();
    getLaneCtrl()->StartDelPicDataTimer();
    getLaneCtrl()->startUpdateTimer();

    //设置雨棚灯状态
    getDeviceFactory()->getIOBoard()->SetCanopy(0);
    //设置上班状态
    getLaneInfo()->setShiftState(lsUnlogin);
    LogMsg("lane", tr("程序启动完成!"));
}

void StateMachineMgr::beginInitState()
{
    emit beginState();
}

void StateMachineMgr::addRushTranLogOut(QState *state)
{
    TranRushLogout* tRush = new TranRushLogout(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)));
    tRush->setTargetState(state);
    state->addTransition(tRush);
}


