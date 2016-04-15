#include "formsoftupdate.h"
#include "ui_formsoftupdate.h"
#include <QFileDialog>
#include <QDir>
#include "FtpModule/ftpclient.h"
#include <QMessageBox>
#include "maintaincfg.h"
#include <QFileInfo>
#include "maintaindatebase.h"
#include <QRegExp>
#include <QMenu>
#include <QContextMenuEvent>
FormSoftUpdate::FormSoftUpdate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSoftUpdate)
{
    ui->setupUi(this);
    destinationFileName = MaintainCFG::getUpdateFileName();
    destinationDir = MaintainCFG::getUpdateFilePath();
    //限制住IP地址
    QRegExp ipRx("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-4]|[01]?\\d\\d?)");
    QRegExpValidator *pIpValidator = new QRegExpValidator(ipRx,this);
    ui->lineEdit_ip->setValidator(pIpValidator);

    //设置treewidget的头
    ui->treeWidget->headerItem()->setText(0,tr("IP"));
    ui->treeWidget->installEventFilter(this);
    //添加action
    removeCommand=new QAction(tr("删除"),this);

    //信号槽的连接
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(clickItem(QTreeWidgetItem*,int)));
    connect(removeCommand,SIGNAL(triggered()),this,SLOT(removeItem()));
}

FormSoftUpdate::~FormSoftUpdate()
{
    getFtpClient()->close();
    delete ui;
}

void FormSoftUpdate::on_toolButton_Soft_clicked()
{
    sourceFileName = QFileDialog::getOpenFileName(this, tr("打开文件"),
                                                    QDir::currentPath());
    ui->lineEdit_file->setText(sourceFileName);
}

void FormSoftUpdate::on_toolButton_update_clicked()
{
    bool ret = false;
    QFileInfo fileinfo(sourceFileName);

//    if(fileinfo.fileName() != destinationFileName)
//    {
//        QMessageBox::warning(this,tr("文件错误"),tr("选中文件%1与要更新的软件名%2不符，请重新选择。").arg(fileinfo.fileName()).arg(destinationFileName)
//                             ,QMessageBox::Ok);
//        return;
//    }
    if(!getFtpClient()->isConnected()|| !getFtpClient()->isLoggedIn())
    {
        QMessageBox::warning(this,tr("连接错误"),tr("当前并没有连接到相应的车道机")
                             ,QMessageBox::Ok);
        return;
    }
    ret = getFtpClient()->cd(destinationDir);
    if(!ret)
    {
        QMessageBox::warning(this,tr("文件错误"),tr("无法打开目标文件所在文件夹%1，按确定后重新创建文件夹").arg(destinationDir)
                             ,QMessageBox::Ok);
        ret = getFtpClient()->mkdir(destinationDir);
        if(!ret)
        {
            QMessageBox::warning(this,tr("文件错误"),tr("无法创建目标文件所在文件夹%1").arg(destinationDir)
                                 ,QMessageBox::Ok);
            return;
        }
        ret = getFtpClient()->cd(destinationDir);
        if(!ret)
        {
            QMessageBox::warning(this,tr("文件错误"),tr("创建成功，无法打开目标文件所在文件夹%1").arg(destinationDir)
                                 ,QMessageBox::Ok);
            return;
        }
    }
    ret = getFtpClient()->put(sourceFileName,fileinfo.fileName());
    if(!ret)
    {
        QMessageBox::warning(this,tr("更新错误"),tr("无法更新程序%1").arg(destinationDir)
                             ,QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this,tr("更新成功"),tr("成功更新程序%1").arg(fileinfo.fileName())
                             ,QMessageBox::Ok);
    }
}

void FormSoftUpdate::on_toolButton_link_clicked()
{
    bool ret ;
    ret = getFtpClient()->connectToServer(ui->lineEdit_ip->text(),ui->lineEdit_usr->text(),ui->lineEdit_pwd->text());
    if(!ret)
    {
        QMessageBox::warning(this,tr("连接失败"),tr("连接车道机失败，原因是：%1").arg(getFtpClient()->error()),QMessageBox::Ok);
        return;
    }
    QMessageBox::warning(this,tr("连接成功"),tr("成功连接车道机：%1").arg(ui->lineEdit_ip->text()),QMessageBox::Ok);
    selectDataToTree();
    for(int i = 0;i != ui->treeWidget->topLevelItemCount();i++)
    {
        if(ui->treeWidget->topLevelItem(i)->text(0) == ui->lineEdit_ip->text())
        {
            return;
        }
    }
    getMaintainDB()->InsertData(ui->lineEdit_ip->text(),ui->lineEdit_usr->text(),ui->lineEdit_pwd->text());
    selectDataToTree();
}

void FormSoftUpdate::on_toolButton_disconnect_clicked()
{
    getFtpClient()->close();
    QMessageBox::warning(this,tr("关闭连接"),tr("连接已关闭"),QMessageBox::Ok);
}

void FormSoftUpdate::showEvent(QShowEvent *)
{
    selectDataToTree();
}

void FormSoftUpdate::removeItem()
{
    LoginNode temp=ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<LoginNode>();
    getMaintainDB()->delIpData(temp.ip);
    ReflashTree();
}

void FormSoftUpdate::clickItem(QTreeWidgetItem *item, int index)
{
    LoginNode tempNode=item->data(0,Qt::UserRole).value<LoginNode>();
    ui->lineEdit_ip->setText(tempNode.ip);
    ui->lineEdit_usr->setText(tempNode.user);
    ui->lineEdit_pwd->setText(tempNode.passWord);
}

void FormSoftUpdate::selectDataToTree()
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

void FormSoftUpdate::ReflashTree()
{
    while(ui->treeWidget->topLevelItemCount())
    {
        ui->treeWidget->takeTopLevelItem(0);
    }
    selectDataToTree();
}

bool FormSoftUpdate::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->treeWidget)
    {
        if(event->type()==QEvent::ContextMenu)
        {
            QContextMenuEvent* dee = dynamic_cast<QContextMenuEvent*>(event);
            QMenu *menu = new QMenu();
            menu->addAction(removeCommand);
            //menu->addAction(renameCommand);
            menu->exec(dee->globalPos());
            delete menu;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void FormSoftUpdate::hideEvent(QHideEvent *)
{
    getFtpClient()->close();
}
