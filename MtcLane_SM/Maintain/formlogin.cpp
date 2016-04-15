#include "formlogin.h"
#include "ui_formlogin.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include "autoupdate.h"
#include "maintaincfg.h"
#include <QCryptographicHash>
#include "maintaindatebase.h"
FormLogin::FormLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLogin)
{
    ui->setupUi(this);
    timeCount=10;
    timerStart();
    currentPage=TIMERCOUNTPAGE;
    QObject::connect(&timerlog,SIGNAL(timeout()),this,SLOT(timerLogin()));
}

FormLogin::~FormLogin()
{
    delete ui;
}

void FormLogin::timerLogin()
{
    ui->MessageTip1_2->setText(tr("程序启动倒计时：%1 秒").arg(timeCount));
    if(timeCount==0)
    {
        AutoUpdate tempUpdate;
        if(!tempUpdate.startSoftware())
        {
            int code=QMessageBox::warning(this,tr("程序启动失败"),tr("程序自启动失败"),QMessageBox::Ok);
            if(code==QMessageBox::Ok)
                emit closeMainForm();
        }
        emit closeMainForm();
        timerStop();
    }
    timeCount--;
}
bool FormLogin::startSoftWare()
{
    AutoUpdate tempUpdate;
    bool ret=tempUpdate.startSoftware();
    emit closeMainForm();
    return ret;
}
void FormLogin::timerStart()
{
    timerlog.stop();
    timerlog.start(1000);
}

void FormLogin::timerStop()
{
    timerlog.stop();
}

void FormLogin::changePageToLogin()
{
    if(ui->stackedWidget->currentIndex()==0)
    {
        ui->stackedWidget->setCurrentIndex(1);
        currentPage=LOGINPAGE;
        timerStop();
        ui->lineEdit_UserName->setFocus();
    }
}

void FormLogin::changePageToCountTime()
{
    if(ui->stackedWidget->currentIndex()==1)
    {
        ui->stackedWidget->setCurrentIndex(0);
        currentPage=TIMERCOUNTPAGE;
        timerStart();
    }
}

quint8 FormLogin::getCurrent()
{
    return currentPage;
}

void FormLogin::setTimeCount(quint16 count)
{
    timeCount=count;
}

QString FormLogin::getUserPwdMD5()
{
    return (ui->lineEdit_UserName->text()+ui->lineEdit_Password->text());
}

bool FormLogin::LoginAuth(QString usr, QString pwd)
{
    QString tempStr0 ,tempStr1;
    QString tempPwdLocal;
    tempStr0 = tr("%1%2%3").arg(usr).arg(pwd).arg(QString::number(0));
    tempStr1 = tr("%1%2%3").arg(usr).arg(pwd).arg(QString::number(1));
    //计算用户的输入MD5
    QByteArray tempPwd0,tempPwd1;
    tempPwd0=QCryptographicHash::hash(tempStr0.toAscii(),QCryptographicHash::Md5);
    tempPwd1=QCryptographicHash::hash(tempStr1.toAscii(),QCryptographicHash::Md5);
    //获取数据库里面的记录
    QList<AuthNode> m_auth;
    m_auth = getMaintainDB()->selectDataFromTbAnth();
    if(0 > m_auth.size())
    {
        return false;
    }
    for(int i = 0; i != m_auth.size();i++)
    {
        if(m_auth.at(i).user == usr)
        {
            tempPwdLocal = m_auth.at(i).MD5;
            break;
        }
    }
    if(tempPwdLocal.isEmpty())
    {
        return false;
    }
    if(tempPwdLocal.compare(QString::fromLocal8Bit(tempPwd0.toHex().data()))==0)
    {
        m_SoftRole = 0;
    }
    else if(tempPwdLocal.compare(QString::fromLocal8Bit(tempPwd1.toHex().data()))==0)
    {
        m_SoftRole = 1;
    }
    else
    {
        return false;
    }
    return true;
}

void FormLogin::clearLineEdit()
{
    ui->lineEdit_Password->clear();
    ui->lineEdit_UserName->clear();
}
void FormLogin::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
    {
        if(getCurrent()==TIMERCOUNTPAGE)
        {
            changePageToLogin();
            break;
        }
        if(getCurrent()==LOGINPAGE)
        {
            changePageToCountTime();
            setTimeCount(10);
            clearLineEdit();
            break;
        }
    }
    case Qt::Key_Return:
    {
        if(getCurrent()==TIMERCOUNTPAGE)
        {
            bool ret=startSoftWare();
            if(!ret)
            {
                if(QMessageBox::warning(this,tr("启动失败"),tr("程序直接启动失败"),QMessageBox::Ok)==QMessageBox::Ok)
                    emit closeMainForm();
            }
            break;
        }
        if(getCurrent()==LOGINPAGE)
        {
            if(ui->lineEdit_UserName->text().isEmpty())
            {
                ui->label->setText(tr("请输入用户名"));
                ui->label->setStyleSheet(tr("color: rgb(255, 0, 0);"));
                ui->lineEdit_UserName->setFocus();
                break;
            }
            if(ui->lineEdit_Password->text().isEmpty())
            {
                ui->label->setText(tr("请输入密码"));
                ui->label->setStyleSheet(tr("color: rgb(255, 0, 0);"));
                ui->lineEdit_Password->setFocus();
                break;
            }
            if(LoginAuth(ui->lineEdit_UserName->text(),ui->lineEdit_Password->text()))
            {
                emit changeToMaintain();
            }
            else
            {
                ui->label->setText(tr("用户名密码错误，重新输入"));
                ui->label->setStyleSheet(tr("color: rgb(255, 0, 0);"));
                clearLineEdit();
                ui->lineEdit_UserName->setFocus();
            }
            break;
        }
    }
    case Qt::Key_Up:
    {
        if(getCurrent()==LOGINPAGE)
            this->focusNextChild();
        break;
    }
    case Qt::Key_Down:
    {
        if(getCurrent()==LOGINPAGE)
            this->focusNextChild();
        break;
    }
    }
}

int FormLogin::getSoftRole()
{
    return m_SoftRole;
}
