#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTime>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc != 3)
    {
        qDebug() << "argument is invalid!";
        return 0;
    }
    QString firstParam = argv[1];
    bool ok = false;
    int fileSize = firstParam.toInt(&ok);
    if(!ok || fileSize <= 0)
    {
        qDebug() << "file size is invalid";
        return 0;
    }
    QFile file("Test");
    if(file.exists())
    {
        file.remove();
    }
    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug() << "file open fail";
        return 0;
    }
    qDebug() << "begin create file(" << fileSize << "M )" ;
    for(int i = 0; i < fileSize; i++)
    {
        for(int i = 0; i < 1024*1024; i++)
        {
            file.write("q");
        }
    }
    file.close();
    qDebug() << "file is created success!";
    qDebug() << "begin move file Test to" << argv[2];
    QTime timer;
    timer.start();
    if(file.copy(argv[2]))
    {
        qDebug() << "copy file success!";
    }
    else
    {
        qDebug() << "copy file fail!";
        return 0;
    }
    qDebug() << "copy time is" << timer.elapsed();

    if(file.remove())
    {
        qDebug() << "remove file success!";
    }
    else
    {
        qDebug() << "remove file fail";
        return 0;
    }
    qDebug() << "total time(ms) is" << timer.elapsed();
    return a.exec();
}
