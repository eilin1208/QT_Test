#include <QtGui/QApplication>
#include "widget.h"
#include <QTextCodec>
#include "LogModule/qfilelogger.h"

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QFileLogger::InitLog();
    QFileLogger* pLogger = new QFileLogger;
    SetLogger(pLogger);
    Widget w;
    w.show();

    return a.exec();
}
