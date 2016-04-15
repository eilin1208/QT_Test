#ifndef FORMTELNET_H
#define FORMTELNET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QAction>
#include "mytelnetwidget.h"
#include "maintaindatebase.h"
namespace Ui {
    class FormTelnet;
}

class FormTelnet : public QWidget
{
    Q_OBJECT

public:
    explicit FormTelnet(QWidget *parent = 0);
    ~FormTelnet();
public:
    //获取用户名
    QString getUserName();
    //获取密码
    QString getPassWord();
    //执行终端命令
    void executionCommand(const QString &message);
    //设置数据重传名字
    void setReLoadName(const QString &name);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void slots_connected();
    void slots_disconnect();
    void on_pBConnect_clicked();
    void on_pBDisConnect_clicked();
    void slots_message(const QString &message);
    void slots_MTCisRuning();
    void selectDataToTree();
    void clickItem(QTreeWidgetItem* item,int index);
    void removeItem();
    void ReflashTree();
    void on_pushButton_ReUpload_clicked();
private:
    void hideReload();
    void showReload();
signals:
    void signalMTCisRuning();
private:
    Ui::FormTelnet *ui;
    MyTelnetWidget *mytelnet;
    //对于ps命令的执行标识
    bool bPsFlag;
    QAction *removeCommand;
    QAction *renameCommand;
    QString sReLoadName;
};

#endif // FORMTELNET_H
