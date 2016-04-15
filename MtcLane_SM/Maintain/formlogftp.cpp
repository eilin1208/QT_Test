#include "formlogftp.h"
#include "ui_formlogftp.h"
#include "maintaindatebase.h"
#include <QMessageBox>
#include "maintaincfg.h"
#include <QDateTime>
FormLogftp::FormLogftp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLogftp)
{
    ui->setupUi(this);
    //设置treewidget的头
    ui->treeWidget->headerItem()->setText(0,tr("IP"));
    ui->treeWidget->installEventFilter(this);
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(LaneclickItem(QTreeWidgetItem*,int)));
    connect(ui->treeWidget_log,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(LogclickItem(QTreeWidgetItem*,int)));
    m_logdir = MaintainCFG::getOldFilePath();
}

FormLogftp::~FormLogftp()
{
    delete ui;
}

void FormLogftp::on_pushButton_Search_clicked()
{
    m_searchData = ui->lineEdit_searchStr->text();
    if(m_searchData.isEmpty())
    {
        ui->label_warings->setText(tr("请输入要搜索的内容！"));
        return;
    }
    if(ui->textEdit_log->find(m_searchData,QTextDocument::FindWholeWords))
    {
        ui->label_warings->setText(tr("查找到搜索的内容【%1】").arg(m_searchData));
    }
    else
    {
        ui->label_warings->setText(tr("已完成对文件%1的搜索。").arg(m_logname));
    }
}

void FormLogftp::selectDataToTree()
{
    ui->treeWidget->clear();
    QList<LoginNode> tempList=getMaintainDB()->selectData();
    for(int i=0;i!=tempList.size();++i)
    {
        QTreeWidgetItem *tempItem=new QTreeWidgetItem;
        tempItem->setText(0,tempList.at(i).ip);
        QVariant tempVariant;
        tempVariant.setValue(tempList.at(i));
        tempItem->setData(0,Qt::UserRole,tempVariant);
        ui->treeWidget->addTopLevelItem(tempItem);
    }
}

void FormLogftp::LaneclickItem(QTreeWidgetItem *item, int index)
{
    m_fileList.clear();
    LoginNode tempNode=item->data(0,Qt::UserRole).value<LoginNode>();
    m_ip = tempNode.ip;
    m_usr = tempNode.user;
    m_pwd = tempNode.passWord;
}

void FormLogftp::showEvent(QShowEvent *)
{
    selectDataToTree();
}

void FormLogftp::LogclickItem(QTreeWidgetItem *item, int index)
{
    ui->textEdit_log->clear();
    ui->lineEdit_searchStr->clear();
    m_logname.clear();
    m_logname = item->text(0);
    bool ret;
    ret = getFtpClient()->get(m_logname);
    if(!ret)
    {
        QMessageBox::warning(this,tr("日志错误"),tr("无法获取日志文件%1，原因是%2")
                             .arg(m_logname).arg(getFtpClient()->error()),QMessageBox::Ok);
        return;
    }
    QFile file(m_logname);
    if(!file.open(QIODevice::ReadOnly))
    {
        ui->label_warings->setText(tr("无法打开日志文件%1").arg(m_logname));
        return;
    }
    QString str = tr("%1").arg(file.readAll().data());
    file.close();
    if(str.isEmpty())
    {
        ui->label_warings->setText(tr("%1日志文件为空").arg(m_logname));
        str.clear();
        return;
    }
    ui->textEdit_log->setText(str);
    ui->label_warings->setText(tr("正在读取日志文件%1").arg(m_logname));
}

void FormLogftp::selectLogtoTree()
{
    ui->treeWidget_log->clear();
    if(m_fileList.count() == 0)
    {
        ui->label_warings->setText(tr("日志个数为0"));
        return;
    }
    for(int i=0;i!=m_fileList.count();i++)
    {
        if(!m_fileList.at(i).name().contains("log"))
        {
            continue;
        }
        QTreeWidgetItem *tempItem=new QTreeWidgetItem;
        tempItem->setText(0,m_fileList.at(i).name());
        tempItem->setText(1,SetLogDescribe(m_fileList.at(i).name()));
        tempItem->setText(2,QString::number(m_fileList.at(i).size()));
        tempItem->setText(3,m_fileList.at(i).lastModified().toString());
        ui->treeWidget_log->addTopLevelItem(tempItem);
    }
}

QString FormLogftp::SetLogDescribe(QString filestr)
{
    if(filestr.contains("lane"))
    {
        return tr("软件收费日志");
    }
    if(filestr.contains("net"))
    {
        return tr("网络传输日志");
    }
    if(filestr.contains("Time"))
    {
        return tr("软件特殊操作时间日志");
    }
    if(filestr.contains("key"))
    {
        return tr("软件操作按键日志");
    }
    if(filestr.contains("FD"))
    {
        return tr("软件费显设备日志");
    }
    if(filestr.contains("Wt"))
    {
        return tr("软件计重设备日志");
    }
    if(filestr.contains("Printer"))
    {
        return tr("软件打印机日志");
    }
    if(filestr.contains("RSU"))
    {
        return tr("软件天线RSU日志");
    }
    else
    {
        return tr("软件日志");
    }
}

void FormLogftp::on_tb_close_clicked()
{
    getFtpClient()->close();
    ui->treeWidget_log->clear();
}

void FormLogftp::on_tb_connect_clicked()
{
    m_fileList.clear();
    bool ret;
    ret = getFtpClient()->connectToServer(m_ip,m_usr,m_pwd);
    if(!ret)
    {
        QMessageBox::warning(this,tr("连接错误"),tr("无法连接到车道机，IP为%1，原因为：%2")
                             .arg(m_ip).arg(getFtpClient()->error()),QMessageBox::Ok);
        return;
    }
    ui->label_warings->setText(tr("连接车道机%1成功").arg(m_ip));
    ret = getFtpClient()->cd(m_logdir);
    if(!ret)
    {
        QMessageBox::warning(this,tr("日志错误"),tr("无法打开日志所在文件夹，文件夹为%1，原因为：%2")
                             .arg(m_logdir).arg(getFtpClient()->error()),QMessageBox::Ok);
        return;
    }
    ret = getFtpClient()->list(&m_fileList,m_logdir);
    if(!ret)
    {
        QMessageBox::warning(this,tr("日志错误"),tr("无法获取所在文件夹内的日志列表，原因为：%2")
                             .arg(getFtpClient()->error()),QMessageBox::Ok);
        return;
    }
    ui->label_warings->setText(tr("成功打开日志所在文件夹，获取日志列表"));
    selectLogtoTree();
}

void FormLogftp::hideEvent(QHideEvent *)
{
    getFtpClient()->close();
    ui->label_warings->setText(tr("已关闭IP为%1的车道机").arg(m_ip));
}
