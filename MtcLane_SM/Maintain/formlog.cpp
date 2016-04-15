#include "formlog.h"
#include "ui_formlog.h"
#include <QDir>
#include <QDateTime>
#include "maintaincfg.h"
FormLog::FormLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLog)
{
    ui->setupUi(this);
    GetLogFile();
    GetLogInfo(ui->treeWidget_log);
    ui->treeWidget_log->addTopLevelItems(m_listItemInfo);
    //信号槽
    connect(ui->treeWidget_log,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(SetLogStr(QTreeWidgetItem*,int)));
}

FormLog::~FormLog()
{
    delete ui;
}

void FormLog::on_pushButton_Search_clicked()
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
        ui->label_warings->setText(tr("已完成对文件%1的搜索。").arg(m_logName));
    }
}

void FormLog::SetLogStr(QTreeWidgetItem *item, int column)
{
    QString logtemp;
    ui->label_warings->clear();
    ui->textEdit_log->clear();
    m_logName = item->text(0);
    m_file.setFileName(m_dirPath+m_logName);
    if(!m_file.exists())
    {
        ui->label_warings->setText(tr("%1日志文件不存在").arg(m_logName));
        return;
    }
    if(!m_file.open(QIODevice::ReadOnly))
    {
        ui->label_warings->setText(tr("%1日志文件无法打开").arg(m_logName));
        return;
    }
    ui->label_warings->setText(tr("打开%1日志文件，读取数据").arg(m_logName));
    logtemp = QString(m_file.readAll().data());
    if(logtemp.isEmpty())
    {
        m_file.close();
        ui->label_warings->setText(tr("%1日志文件为空").arg(m_logName));
        logtemp.clear();
        return;
    }
    ui->textEdit_log->setText(logtemp);
    m_file.close();
    logtemp.clear();
}

void FormLog::GetLogFile()
{
    m_dirPath = MaintainCFG::getOldFilePath();
    qDebug()<<"m_dirPath"<<m_dirPath;
    ui->label_warings->clear();
    ui->label_warings->setText(tr("当前日志文件的目录是%1").arg(m_dirPath));
    QDir dir(m_dirPath);
    QStringList strlist;
    strlist << "*.log*";
    dir.setNameFilters(strlist);
    m_listLogInfo = dir.entryInfoList(strlist);
    if(m_listLogInfo.count() == 0)
    {
        ui->label_warings->clear();
        ui->label_warings->setText(tr("搜索到文件个数为0，请查看路径是否正确！"));
        return;
    }
}

void FormLog::GetLogInfo(QTreeWidget *widget)
{
    QTreeWidgetItem *item;
    for(int i = 0; i <m_listLogInfo.count();i++)
    {
        item=new QTreeWidgetItem(widget);
        item->setText(0,m_listLogInfo.at(i).fileName());
        item->setText(1,SetLogDescribe(m_listLogInfo.at(i).fileName()));
        item->setText(2,QString::number(m_listLogInfo.at(i).size()));
        item->setText(3,m_listLogInfo.at(i).lastModified().toString());
        m_listItemInfo.append(item);
    }
}

QString FormLog::SetLogDescribe(QString filestr)
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

