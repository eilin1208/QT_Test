#include <QtGui/QApplication>
#include "widget.h"
#include "RSUArm/rsuctrl.h"
#include <QTextCodec>
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
    Widget w;
    w.show();
    //rsuctrl myrsu("RSU");
    //    myrsu.StartDev();
    // myrsu.Init();
    //myrsu.StartDev();
    return a.exec();
}
