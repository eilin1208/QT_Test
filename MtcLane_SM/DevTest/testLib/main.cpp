#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc < 2)
    {
        qDebug() << "argv count is invalid,plz enter lib path:";
        return 0;
    }
    QString libName = argv[1];
    qDebug() << "lib name is" << libName;
    QLibrary lib(libName);
    qDebug() << "lib load" << (lib.load()? "success" : "fail");
    return 0;
}
