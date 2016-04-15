#include "formtelnet.h"
#include "ui_formtelnet.h"
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QEvent>
FormTelnet::FormTelnet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTelnet)
{
    ui->setupUi(this);
    hideReload();
    setReLoadName("ReUpLoad");
    //把终端模式加进来
    mytelnet=new MyTelnetWidget;
    ui->tab_2->layout()->addWidget(mytelnet);

    //限制住IP地址
    QRegExp ipRx("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-4]|[01]?\\d\\d?)");
    QRegExpValidator *pIpValidator = new QRegExpValidator(ipRx,this);
    ui->lineEdit->setValidator(pIpValidator);

    //设置treewidget的头
    ui->treeWidget->headerItem()->setText(0,tr("IP"));
    ui->treeWidget->installEventFilter(this);
    //添加action
    removeCommand=new QAction(tr("删除"),this);

    //信号槽的连接
    connect(mytelnet,SIGNAL(connected()),this,SLOT(slots_connected()));
    connect(mytelnet,SIGNAL(disconnected()),this,SLOT(slots_disconnect()));
    connect(mytelnet,SIGNAL(termMessage(const QString &)),this,SLOT(slots_message(const QString&)));
    connect(this,SIGNAL(signalMTCisRuning()),this,SLOT(slots_MTCisRuning()));
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(clickItem(QTreeWidgetItem*,int)));
    connect(removeCommand,SIGNAL(triggered()),this,SLOT(removeItem()));

    //数据初始化
    bPsFlag=false;

    //数据库的设置
    getMaintainDB()->setDateBaseName("Maintain.db");
    if(!getMaintainDB()->connectToDB(getMaintainDB()->getDateBaseName()))
        QMessageBox::warning(this,tr("数据库连接失败"),tr("数据库连接失败：%1").arg(getMaintainDB()->getErrorMessage()),QMessageBox::Ok);
    selectDataToTree();
}

FormTelnet::~FormTelnet()
{
    getMaintainDB()->close();
    delete ui;
}

void FormTelnet::slots_connected()
{
    //按钮不可用
    ui->lineEdit->setDisabled(true);
    ui->leUser->setDisabled(true);
    ui->lePassword->setDisabled(true);
    ui->pBConnect->setDisabled(true);
    ui->pBConnect->setStyleSheet(tr("color: rgb(0, 255, 0);"));
    showReload();
    ui->textEdit->clear();
}

void FormTelnet::on_pBConnect_clicked()
{
    mytelnet->connectToHost(ui->lineEdit->text());
    //把登录信息记录到数据库
    if(!ui->lineEdit->text().isEmpty())
        getMaintainDB()->InsertData(ui->lineEdit->text()
                                    ,ui->leUser->text()
                                    ,ui->lePassword->text());
    //刷新tree
    ReflashTree();
}

void FormTelnet::slots_disconnect()
{
    //按钮可用
    ui->lineEdit->setEnabled(true);
    ui->pBConnect->setEnabled(true);
    ui->lePassword->setEnabled(true);
    ui->leUser->setEnabled(true);
    ui->pBConnect->setStyleSheet(tr("color: rgb(255, 0, 0);"));
    hideReload();
    ui->textEdit->clear();
    disconnect(mytelnet,SIGNAL(termMessage(const QString &)),ui->textEdit,SLOT(append(QString)));
}

void FormTelnet::on_pBDisConnect_clicked()
{
    mytelnet->disconnectTelnet();
}

QString FormTelnet::getUserName()
{
    return ui->leUser->text();
}

QString FormTelnet::getPassWord()
{
    return ui->lePassword->text();
}

void FormTelnet::slots_message(const QString &message)
{
    //匹配用户名
    if(message.contains("login",Qt::CaseInsensitive))
    {
        mytelnet->t->sendData(getUserName());
        mytelnet->t->sendData("\r\n");
    }
    //匹配密码
    if(message.contains("password",Qt::CaseInsensitive))
    {
        mytelnet->t->sendData(getPassWord());
        mytelnet->t->sendData("\r\n");

    }
    //ps命令的查询
    if(bPsFlag)
    {
        if(message.contains("/MtcLane/MtcLane",Qt::CaseInsensitive))
            emit signalMTCisRuning();
    }
}

void FormTelnet::executionCommand(const QString &message)
{
    if(message.contains("ps",Qt::CaseInsensitive))
    {
        bPsFlag=true;
    }
    mytelnet->t->sendData(message);
    mytelnet->t->sendData("\r\n");
}

void FormTelnet::slots_MTCisRuning()
{
    int temp=QMessageBox::warning(this,tr("警告"),tr("MTC软件正在运行……\r\n按[ok]断开连接\r\n按[cancel]取消"),QMessageBox::Ok|QMessageBox::Cancel);
    switch(temp)
    {
    case QMessageBox::Ok:
    {
        mytelnet->disconnectTelnet();
        break;
    }
    case QMessageBox::Cancel:
        break;
    }
}

void FormTelnet::selectDataToTree()
{
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

void FormTelnet::clickItem(QTreeWidgetItem *item, int index)
{
    LoginNode tempNode=item->data(0,Qt::UserRole).value<LoginNode>();
    ui->lineEdit->setText(tempNode.ip);
    ui->leUser->setText(tempNode.user);
    ui->lePassword->setText(tempNode.passWord);
}

bool FormTelnet::eventFilter(QObject *watched, QEvent *event)
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

void FormTelnet::removeItem()
{
    LoginNode temp=ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<LoginNode>();
    getMaintainDB()->delIpData(temp.ip);
    ReflashTree();
}

void FormTelnet::ReflashTree()
{
    while(ui->treeWidget->topLevelItemCount())
    {
        ui->treeWidget->takeTopLevelItem(0);
    }
    selectDataToTree();
}

void FormTelnet::on_pushButton_ReUpload_clicked()
{
    //获取重传数据参数
    quint64 begin=ui->dateTimeEdit->dateTime().toTime_t();
    quint64 end=ui->dateTimeEdit_2->dateTime().toTime_t();
    //连接数据到页面显示
    QString commod=tr("cd /Program_Files/MtcLane/");
    executionCommand(commod);
    commod=tr("chmod 777 ./%1").arg(sReLoadName);
    executionCommand(commod);
    commod=tr("./%1 %2 %3 -qws")
            .arg(sReLoadName)
            .arg(begin)
            .arg(end);
    connect(mytelnet,SIGNAL(termMessage(const QString &)),ui->textEdit,SLOT(append(QString)));
    executionCommand(commod);
}

void FormTelnet::setReLoadName(const QString &name)
{
    sReLoadName=name;
}

void FormTelnet::hideReload()
{
    ui->widget_2->hide();
}

void FormTelnet::showReload()
{
    ui->widget_2->show();
}
