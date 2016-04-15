#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QWidget>
#include <QMap>
namespace Ui {
class FormMain;
}

class FormMain : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormMain(QWidget *parent = 0);

    ~FormMain();
public:
    enum FORMTYPE
    {
        FORMHOME,       //首页
        FORMLOG,        //日志
        FORMFILE,       //文件
        FORMUPDATE,     //升级
        FORMDEVTEST,    //外设测试
        FORMSETTING,    //车道配置
        FORMDATARESEND, //数据重传
        FORMLOGIN,      //登陆
        FORMFILEDIALOG,  //文件传输
        FORMTELNET ,      //远程登录(数据重传)
        FORMLOGFTP      //远程日志
    };
public:
    //显示切换按钮
    void showToolBox();
    //隐藏切换按钮
    void hideToolBox();
private slots:
    void on_btnClose_clicked();

    void on_btnMax_clicked();

    void on_btnMin_clicked();

    //登陆成功
    void LoginSuccess();

    void on_tb_Update_clicked();

    void on_tb_Setting_clicked();

    void on_tb_devtest_clicked();

    void on_tb_File_clicked();

    void on_tb_Log_clicked();

    void on_tb_DataResend_clicked();

    void on_tb_Home_clicked();

private:
    void createPages();

    int addPages(QWidget* page);

    void setWidgetSize(int h,int w);
private:
    Ui::FormMain *ui;
    QMap<FORMTYPE, int> m_formIndex;
    int m_SoftRole;
};

#endif // FORMMAIN_H
