#include <QtGui/QApplication>
#include "mainwindow.h"
#include "PacketSendModule/qdatasendmgr.h"
#include "PacketSendModule/sqlitedatasaver.h"
#include "LogModule/qfilelogger.h"
#include <QTextCodec>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //设置日志接口
    QFileLogger::InitLog();
    QFileLogger* pLogger = new QFileLogger;
    SetLogger(pLogger);
    //
    QSettings setting("./Lane.ini",QSettings::IniFormat);
    QString ip = setting.value("StationInfo/DataSrvIP").toString();
    quint32 port = setting.value("StationInfo/DataSrvPort").toUInt();
    //打开数据库
    if(!getDataSaver()->ConnectDB())
    {
        qDebug()<<QObject::tr("数据库连接失败");
        a.quit();
        return 0;
    }
    //自动重传
    //getDataSendMgr()->SetDataSaver(getDataSaver());
    //getDataSendMgr()->InitSendDataClient(ip,port);
    quint64 start = QString::fromLocal8Bit(argv[1]).toLongLong();
    quint64 end = QString::fromLocal8Bit(argv[2]).toLongLong();
    qDebug()<<QObject::tr("重传范围,开始:%1--结束:%2").arg(start).arg(end);
    if(getDataSaver()->setDataState(start,end,false))
    {
        qDebug()<<QObject::tr("标志位修改完成");
        getDataSaver()->closeDB();
        a.quit();
    }
    //return a.exec();
}
