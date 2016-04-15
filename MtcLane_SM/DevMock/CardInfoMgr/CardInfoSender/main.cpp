#include <QCoreApplication>
#include "tcpsender.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpSender sender;
    sender.start();
    return a.exec();
}
