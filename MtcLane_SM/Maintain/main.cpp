#include <QtGui/QApplication>
#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include "formmain.h"
#include "globalvalue.h"
#include "autoupdate.h"
#include "CommonModule/ilogmsg.h"
#include "LogModule/qfilelogger.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //设置日志接口
    QFileLogger::InitLog();
    SetLogger(new QFileLogger);
    QFile styleSheet("./../qssfiles/style.qss");
    if(!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("style sheet load fail!");
        QMessageBox::warning(NULL, QObject::tr("警告"), QObject::tr("布局文件打开失败，页面无法正常布局"));
    }
    a.setStyleSheet(styleSheet.readAll());
    setGlobalValue(Maintain::LaneIp, "192.168.26.219");
    setGlobalValue(Maintain::LanePassword, "1q2w3e");
    setGlobalValue(Maintain::LaneUser, "root");
    AutoUpdate updateSoftware;
    if(!updateSoftware.softWareUpdate())
    {
        int code=QMessageBox::warning(NULL,QObject::tr("软件更新"),QObject::tr("软件更新失败：%1")
                             .arg(updateSoftware.getErrorMessage()),QMessageBox::Ok);
        if(code==QMessageBox::Ok)
            return -1;
    }
    FormMain w;
    LogMsg("RSU",QObject::tr("12skl;ghsirhgpuisrhgiuhipaeru"));
    w.show();
    return a.exec();
}
