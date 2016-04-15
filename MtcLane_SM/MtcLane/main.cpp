#include <QtGui/QApplication>
#include <QFile>
#include <QTextCodec>
#include <QSplashScreen>
#include <QMessageBox>
#include "formmainwindow.h"
#include "ParamModule/qparammgr.h"
#include "CommonModule/ilogmsg.h"
#include "laneinfo.h"
#include "LogModule/qfilelogger.h"
#include "statemachinemgr.h"
#include "devicefactory.h"
#include "formordinartprocess.h"
#include "MtcKey/MtcOpWidget.h"
#include "reqparam.h"
#include "dialogshowproinfo.h"
#include "PacketSendModule/qdatasendmgr.h"
#include "lanectrl.h"
#include "formvpr.h"
#include "XMLBakup/xmlbakupmgr.h"
int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //设置版本号
    a.setApplicationVersion(APP_VERSION);
    //设置依赖目录
    //设置日志接口
    QFileLogger::InitLog();
    QFileLogger* pLogger = new QFileLogger;
    SetLogger(pLogger);
    //    //初始化云南键值表
    //    //初始化云南键值表
    initKeyDef_YN2012();
    //显示初始化窗口
    QPixmap pixmap(":/images/image/loading.png");
    QSplashScreen splash(pixmap);
    splash.show();

    //加载布局文件
    QFile styleSheet("./../qssfiles/style.qss");
    if(!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("style sheet load fail!");
        QMessageBox::warning(NULL, QObject::tr("警告"), QObject::tr("布局文件打开失败，页面无法正常布局"));
        LogMsg("lane", QObject::tr("布局文件打开失败，页面无法正常布局"));
    }
    a.setStyleSheet(styleSheet.readAll());


    //加载INI文件信息
    splash.showMessage(QObject::tr("加载配置文件..."), Qt::AlignRight | Qt::AlignBottom, Qt::white);
    if(!getLaneInfo()->loadDataFromIni())
    {
        LogMsg("lane", QObject::tr("Lane.ini文件未找到"));
        QMessageBox::warning(NULL, QObject::tr("错误"), QObject::tr("Lane.ini或Setting.ini文件未找到，无法加载配置项！"));
        return -1;
    }

    //验证车道程序是否是MTC类型
    if(!(getLaneInfo()->isMtcLane() || getLaneInfo()->isAutoLane()))
    {
        LogMsg("lane", QObject::tr("车道类型不符，lane.ini文件中车道类型为[%1]").arg(getLaneInfo()->getLaneType()));
        QMessageBox::warning(NULL, QObject::tr("错误"), QObject::tr("车道类型不符，请联系维护人员进行修正"));
        return -1;
    }
    //检测硬盘空间
    int total,avail;
    if(!getLaneInfo()->getDiskSize(total,avail))
    {
        LogMsg("lane", QObject::tr("硬盘空间不足"));
        QMessageBox::warning(NULL, QObject::tr("错误"), QObject::tr("硬盘空间不足！"));
        return 0;
    }

    //初始化设备
    getDeviceFactory()->InitAllDevice(getLaneInfo()->getLaneType());

    //初始化主窗口
    FormMainWindow mainWindow;

    //设置页面日志
    FormVpr* pVpr = NULL;
    getWidgetByType(emFormVpr, (QWidget**)&pVpr);
    if(pVpr != NULL)
    {
        pVpr->setLogger(pLogger);
    }


    //打开设备
    bool ok = getDeviceFactory()->OpenAllDevice();
    if(!ok)
    {
        LogMsg("lane", QObject::tr("由于设备初始化失败，程序打开失败"));
    }
    //设置视频显示页面
    QWidget* pVideo = NULL;
    getWidgetByType(emFormCapture,&pVideo);
    getDeviceFactory()->getVideoCard()->SetVideoUi(pVideo);

    //设置车牌识别
    getDeviceFactory()->getVpr()->SetEventProcWnd(pVpr, 2005);
    if(getDeviceFactory()->getFD() != NULL)
    {
        getDeviceFactory()->getFD()->SetMode(getLaneInfo()->getLaneType(), getLaneInfo()->getUseWeight());

    }//    getLaneInfo()->setUsePlate(112);//TODO_liubo
    //启动状态机
    getStateMgr()->buildStateMachine(mainWindow.getStateWidgetLayout(),
                                     getLaneInfo()->isEntryLane());
    //显示主窗口
    mainWindow.show();
    splash.finish(&mainWindow);

    //显示视频采集
    getDeviceFactory()->getVideoCard()->InitVideoCap();
    getDeviceFactory()->getVideoCard()->Display();

    //显示字符叠加
    getDeviceFactory()->getVDM()->UpdateDateTime();
    getDeviceFactory()->getVDM()->UpdateLane("", getLaneInfo()->getLaneCode());

    //设置参数下载的IP和端口号
    getReqParam()->SetHostInfo(getLaneInfo()->getDataSrvIp(), 23325);
    getReqParam()->start();

    //设置对应车道类型的参数文件
    getParamMgr()->InitParamFile(getLaneInfo()->getLaneType());

    //启动备份
    getXMLBakupMgr()->init();
    //添加事件过滤器
    a.installEventFilter(MtcKeyFilter::instance());

    //隐藏鼠标_liubo
    QApplication::setOverrideCursor(Qt::BlankCursor);
    return a.exec();
}
