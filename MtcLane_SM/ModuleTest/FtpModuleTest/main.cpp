#include <QCoreApplication>
#include "FtpModule/ftpclient.h"
#include <QDebug>
FtpClient *client;

void listFiles();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    client = new FtpClient();
    qDebug() << "connect to server" << client->connectToServer("192.168.26.219", "root", "1q2w3e");
    listFiles();
    qDebug() << "cd" << client->cd("Program_Files");
    listFiles();

    qDebug() << "rmdir" << client->rmdir("dir");
    delete client;
    return a.exec();
}

void listFiles()
{
    QVector<QUrlInfo> fileList;

    qDebug() << "list " << client->list(&fileList);

    foreach(QUrlInfo info, fileList)
    {
        qDebug() << info.name();
    }
}
