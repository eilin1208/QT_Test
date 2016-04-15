#include <QCoreApplication>
#include "tcpreceiver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpReceiver receiver;
    receiver.start();
    return a.exec();
}
