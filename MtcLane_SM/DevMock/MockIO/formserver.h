#ifndef FORMSERVER_H
#define FORMSERVER_H
#include <QtNetwork/QTcpServer>
#include <QWidget>
#include <QVector>
#include "msgreceiver.h"
namespace Ui {
    class FormServer;
}
class FormServer : public QWidget
{
    Q_OBJECT

public:
    explicit FormServer(QWidget *parent = 0);
    ~FormServer();
public slots:
    void getStr(int &choice , QString &string);

    void newConnect();
public:
//    void InitDec();//初始化idecode，将各个子类加进去

    void InitDsp();//初始化idisplay，将各个子类加进去
private:
    Ui::FormServer *ui;
    QTcpServer *tcpServer;
    QVector<MsgReceiver*> m_receivers;
    QString m_string;
    int m_choice;

};
#endif // FORMSERVER_H
