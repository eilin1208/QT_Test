#include "formmain.h"
#include "ui_formmain.h"
#include "formfile.h"
#include "formcfgfile.h"
#include "formlog.h"
#include "formlogin.h"
#include "formfiledialog.h"
#include "formtelnet.h"
#include <QKeyEvent>
#include <QMessageBox>
#include "autoupdate.h"
#include <QDesktopWidget>
#include "formdevtest.h"
#include "maintaincfg.h"
#include "formsoftupdate.h"
#include "formlogftp.h"
FormMain::FormMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMain)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    createPages();
    connect(ui->stackedWidget->widget(m_formIndex.value(FORMLOGIN)),SIGNAL(closeMainForm())
            ,this,SLOT(on_btnClose_clicked()));
    connect(ui->stackedWidget->widget(m_formIndex.value(FORMLOGIN)),SIGNAL(changeToMaintain())
            ,this,SLOT(LoginSuccess()));
    hideToolBox();
    setWidgetSize(400,300);
    //焦点默认给登陆界面
    ((FormLogin*)ui->stackedWidget->widget(m_formIndex.value(FORMLOGIN)))->setFocus();
}

FormMain::~FormMain()
{
    delete ui;
}
void FormMain::showToolBox()
{
    ui->widget->show();
}

void FormMain::hideToolBox()
{
    ui->widget->hide();
}

void FormMain::createPages()
{
//    m_SoftRole = MaintainCFG::getSoftRole();
    //本地操作
    m_formIndex[FORMSETTING]=addPages(new FormCfgFile);//配置文件
    m_formIndex[FORMLOG] = addPages(new FormLog);//日志查看
    m_formIndex[FORMDEVTEST]=addPages(new FormDevTest);//设备测试
    m_formIndex[FORMFILEDIALOG]=addPages(new FormFileDialog);//文件传输
    //远程操作
    m_formIndex[FORMTELNET]=addPages(new FormTelnet);//数据重传
    m_formIndex[FORMFILE] = addPages(new FormFile); //文件传输
    m_formIndex[FORMUPDATE]=addPages(new FormSoftUpdate);//程序升级
    m_formIndex[FORMLOGFTP] =addPages(new FormLogftp);//日志查看
    m_formIndex[FORMLOGIN]= addPages(new FormLogin);
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMLOGIN));
}

int FormMain::addPages(QWidget *page)
{
    return ui->stackedWidget->addWidget(page);
}


void FormMain::on_btnClose_clicked()
{
    this->close();
}

void FormMain::on_btnMax_clicked()
{
    static bool flag = true;
    if(flag)
    {
        this->showMaximized();
    }
    else
    {
        this->showNormal();
    }
    flag = !flag;
}

void FormMain::on_btnMin_clicked()
{
    this->showMinimized();
}

void FormMain::LoginSuccess()
{
    m_SoftRole = ((FormLogin*)ui->stackedWidget->widget(m_formIndex.value(FORMLOGIN)))->getSoftRole();
    if(m_SoftRole == 0)
    {
        ui->tb_DataResend->hide();
        ui->tb_Update->hide();
    }
    else if(m_SoftRole == 1)
    {
        ui->tb_devtest->hide();
        ui->tb_Setting->hide();
    }
    setWidgetSize(800,600);
    showToolBox();
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMTELNET));
}
void FormMain::setWidgetSize(int h, int w)
{
    this->setFixedSize(h,w);
    //程序居中,自适应屏幕大小
    int currentScreenWid = QApplication::desktop()->width();
    int currentScreenHei = QApplication::desktop()->height();
    this->setGeometry(currentScreenWid/2-h/2,currentScreenHei/2-w/2,h,w);
}

void FormMain::on_tb_Update_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMUPDATE));
}

void FormMain::on_tb_Setting_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMSETTING));
}

void FormMain::on_tb_devtest_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMDEVTEST));
}

void FormMain::on_tb_File_clicked()
{
    if(m_SoftRole == 0)
    {
       ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMFILEDIALOG));
    }
    else if(m_SoftRole == 1)
    {
        ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMFILE));
    }
}

void FormMain::on_tb_Log_clicked()
{
    if(m_SoftRole == 0)
    {
       ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMLOG));
    }
    else if(m_SoftRole == 1)
    {
        ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMLOGFTP));
    }
}

void FormMain::on_tb_DataResend_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_formIndex.value(FORMTELNET));
}

void FormMain::on_tb_Home_clicked()
{
    AutoUpdate tempUpdate;
    if(!tempUpdate.startSoftware())
    {
        int code=QMessageBox::warning(this,tr("程序启动失败"),tr("程序直接启动失败"),QMessageBox::Ok);
        if(code==QMessageBox::Ok)
            this->close();
    }
}
