#ifndef FORMLOGIN_H
#define FORMLOGIN_H

#include <QWidget>
#include <QTimer>
namespace Ui {
class FormLogin;
}
enum CurrentPage
{
    TIMERCOUNTPAGE=1,
    LOGINPAGE
};
class FormLogin : public QWidget
{
    Q_OBJECT

public:
    explicit FormLogin(QWidget *parent = 0);
    ~FormLogin();
private:
    //倒计时开始
    void timerStart();
    //停止定时器
    void timerStop();
public slots:
    //倒计时后槽函数
    void timerLogin();
    //切换到登陆界面
    void changePageToLogin();
    //切换到倒计时界面
    void changePageToCountTime();
public:
    quint8 getCurrent();
    void setTimeCount(quint16 count);
    //启动软件
    bool startSoftWare();
    //获取用户名和密码
    QString getUserPwdMD5();
    //进行身份比对
    bool LoginAuth(QString usr,QString pwd);
    //清空用户名和密码的输入框
    void clearLineEdit();
    //获取角色值
    int getSoftRole();

    void keyPressEvent(QKeyEvent *event);
signals:
    void closeMainForm();
    void changeToMaintain();
private:
    Ui::FormLogin *ui;
    QTimer timerlog;
    quint16 timeCount;
    quint8 currentPage;
    int m_SoftRole;
};

#endif // FORMLOGIN_H
