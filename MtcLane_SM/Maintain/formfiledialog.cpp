#include "formfiledialog.h"
#include "ui_formfiledialog.h"
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QDebug>
#include <QFileIconProvider>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDropEvent>
FormFileDialog::FormFileDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFileDialog)
{
    ui->setupUi(this);
    ui->treeWidget->setHeaderLabel(tr("文件目录"));
    ui->listWidget->setAcceptDrops(true);

    AddFileToTree(QDir::rootPath(),ui->treeWidget);
    ui->treeWidget->addTopLevelItems(listItem);
    //设置listWidget
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setTextElideMode(Qt::ElideRight);
    ui->listWidget->setSpacing(12);
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(clickItems(QTreeWidgetItem*,int)));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(clickUpListItems(QListWidgetItem*)));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(clickTransPushbutton()));
    connect(ui->listWidget_2,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(clickDownListItems(QListWidgetItem*)));
    bturnUp=true;
}

FormFileDialog::~FormFileDialog()
{
    delete ui;
}

void FormFileDialog::AddFileToTree(const QString &path,QTreeWidget *father,QTreeWidgetItem *fatherItem)
{
    QDir dir;
    dir.setPath(path);
    dir.setFilter(QDir::Files | QDir::Hidden|QDir::Dirs);
    dir.setSorting(QDir::DirsFirst | QDir::Reversed);

    QFileInfoList List=dir.entryInfoList();
    QFileInfo fileInfo;

    for(quint16 index=0;index!=List.size();++index)
    {
        fileInfo=List.at(index);
        //文件过滤
        if(fileInfo.fileName()=="."||fileInfo.fileName()=="..")
        {
            continue;
        }
        QTreeWidgetItem *tempItem;
        if(father==NULL)
        {
            tempItem=new QTreeWidgetItem(fatherItem);
        }
        else
        {
            tempItem=new QTreeWidgetItem(father);
        }
        tempItem->setText(0,fileInfo.fileName());
        QVariant tempVariant;
        tempVariant.setValue(fileInfo.filePath());
        tempItem->setData(0,Qt::UserRole,tempVariant);
        QFileIconProvider tempIcon;
        if(fileInfo.isDir())
        {
            tempItem->setIcon(0,tempIcon.icon(QFileIconProvider::Folder));
        }
        else
        {
            tempItem->setIcon(0,tempIcon.icon(QFileIconProvider::File));
        }
        listItem.append(tempItem);
    }
}

void FormFileDialog::clickItems(QTreeWidgetItem* item,int index)
{
    QString tempPath=item->data(0,Qt::UserRole).value<QString >();
    QFileInfo tempFileinfo(tempPath);
    //增加到左边listWidget
    if(bturnUp)
    {
        AddFileTOListWidget(tempFileinfo.filePath(),ui->listWidget);
        //显示当前目录信息
        if(tempFileinfo.isDir())
        {
            QList<QString>::iterator index;
            for(index=listtree1OpenFileName.begin();index!=listtree1OpenFileName.end();++index)
            {
                if(tempFileinfo.fileName()==*index)
                    return;
            }
            AddFileToTree(tempFileinfo.filePath(),NULL,item);
            listtree1OpenFileName.append(tempFileinfo.fileName());
        }
    }
    else
    {
        AddFileTOListWidget(tempFileinfo.filePath(),ui->listWidget_2);
        targetPath=tempFileinfo.filePath();
        //显示当前目录信息
        if(tempFileinfo.isDir())
        {
            QList<QString>::iterator index;
            for(index=listtree2OpenFileName.begin();index!=listtree2OpenFileName.end();++index)
            {
                if(tempFileinfo.fileName()==*index)
                    return;
            }
            AddFileToTree(tempFileinfo.filePath(),NULL,item);
            listtree2OpenFileName.append(tempFileinfo.fileName());
        }
    }
}

void FormFileDialog::AddFileTOListWidget(const QString &path,QListWidget *listwidget)
{
    //获取指定目录下的所有文件
    QDir dir;
    dir.setPath(path);
    dir.setFilter(QDir::Files | QDir::Hidden|QDir::Dirs);
    dir.setSorting(QDir::DirsFirst | QDir::Reversed);

    QFileInfoList List=dir.entryInfoList();
    QFileInfo fileInfo;

    //清空原有文件信息
    listwidget->clear();
    //增加新的文件信息
    QFileInfoList::Iterator index;
    for(index=List.begin();index!=List.end();++index)
    {
        fileInfo=*index;
        if(fileInfo.fileName()=="."||fileInfo.fileName()=="..")
            continue;
        QListWidgetItem *tempItem=new QListWidgetItem(listwidget);
        QFileIconProvider tempIcon;
        if(fileInfo.isDir())
        {
            tempItem->setIcon(tempIcon.icon(QFileIconProvider::Folder));
        }
        else
        {
            tempItem->setIcon(tempIcon.icon(QFileIconProvider::File));
        }
        tempItem->setText(fileInfo.fileName());
        tempItem->setSizeHint(QSize(80,80));
        QVariant tempVariant;
        tempVariant.setValue(fileInfo.filePath());
        tempItem->setData(Qt::UserRole,tempVariant);
        QString fileSize=tr("%1 %2")
                .arg(fileInfo.size()>1024.0?fileInfo.size()/(1024.0*1024.0):fileInfo.size()/1024.0)
                .arg(fileInfo.size()>1024?"Mb":"Kb");
        tempItem->setToolTip(tr("名字:%1\n大小：%2")
                             .arg(fileInfo.fileName())
                             .arg(fileSize));
    }
}

void FormFileDialog::clickUpListItems(QListWidgetItem *item)
{
    clickItems(ui->treeWidget->currentItem()->child(ui->listWidget->currentIndex().row()),0);
}

void FormFileDialog::clickTransPushbutton()
{
    //创建
    tempWidget=new QWidget(this);
    tempLayout=new QVBoxLayout(tempWidget);
    tempTreeWidget=new QTreeWidget;
    temppushbutton=new QPushButton;

    //设置属性
    temppushbutton->setText(tr("确定"));
    tempTreeWidget->setHeaderLabel(tr("保存目录"));
    tempWidget->setWindowFlags(Qt::FramelessWindowHint);
    tempWidget->setGeometry(this->geometry().width()/2-200,this->geometry().height()/2-150,400,300);
    ui->listWidget_2->setDragEnabled(true);
    ui->listWidget_2->setViewMode(QListView::IconMode);
    ui->listWidget_2->setResizeMode(QListView::Adjust);
    ui->listWidget_2->setTextElideMode(Qt::ElideRight);
    ui->listWidget_2->setSpacing(12);

    //设置布局
    tempLayout->addWidget(tempTreeWidget);
    tempLayout->addWidget(temppushbutton);
    tempWidget->setLayout(tempLayout);

    //设置数据
    AddFileToTree(QDir::rootPath(),tempTreeWidget);
    bturnUp=false;
    //设置信号槽
    connect(temppushbutton,SIGNAL(clicked()),this,SLOT(clickSavePushbutton()));
    connect(tempTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(clickItems(QTreeWidgetItem*,int)));
    //显示
    tempWidget->show();
}

void FormFileDialog::clickDownListItems(QListWidgetItem *item)
{
    //    AddFileTOListWidget(QDir::currentPath(),ui->listWidget_2);
}

void FormFileDialog::clickSavePushbutton()
{
    temppushbutton->setDisabled(true);
    //    clickItems(tempTreeWidget->currentItem(),0);
    //拷贝文件
    if(ui->listWidget->currentItem()==NULL)
    {
        int flag=QMessageBox::warning(this,tr("警告"),tr("请选择你要复制的文件"),QMessageBox::Ok);
        switch(flag)
        {
        case QMessageBox::Ok:
            tempWidget->close();
            listtree2OpenFileName.clear();
            //标记
            bturnUp=true;
            return;
        }
    }
    QVariant tempVarient=ui->listWidget->currentItem()->data(Qt::UserRole);
    QFileInfo tempUpFile=tempVarient.value<QFileInfo >();
    if(targetPath.isEmpty())
    {
        int flag=QMessageBox::warning(this,tr("警告"),tr("请选择你要把文件复制到哪"),QMessageBox::Ok);
        switch(flag)
        {
        case QMessageBox::Ok:
            tempWidget->close();
            listtree2OpenFileName.clear();
            //标记
            bturnUp=true;
            return;
        }
    }
    if(tempUpFile.isDir())
    {
        copyDirectoryFiles(tempUpFile.filePath(),tempUpFile.isDir()?(targetPath+"/"+tempUpFile.fileName()):targetPath,true);
    }
    else
    {
        copyFileToPath(tempUpFile.filePath(),targetPath+"/"+tempUpFile.fileName(),true);
    }
    listtree2OpenFileName.clear();
    //标记
    bturnUp=true;
    //关闭窗口
    tempWidget->close();
}
bool FormFileDialog::copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    qDebug()<<"copy floder";
    qDebug()<<"sourse"<<fromDir;
    qDebug()<<"target"<<toDir;
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists())
    {
        if(!targetDir.mkdir(targetDir.absolutePath()))
        {
            errorMessage=tr("目标文件夹不存在，并且创建失败");
            return false;
        }
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir())
        {
            if(!copyDirectoryFiles(fileInfo.filePath(),
                                   targetDir.filePath(fileInfo.fileName()),
                                   coverFileIfExist))
            {
                errorMessage=tr("递归复制失败");
                return false;
            }
        }
        else
        {
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                            targetDir.filePath(fileInfo.fileName())))
            {
                errorMessage=tr("文件复制失败");
                return false;
            }
        }
    }
    return true;
}
bool FormFileDialog::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    qDebug()<<"copy file";
    qDebug()<<"sourse"<<sourceDir;
    qDebug()<<"target"<<toDir;
    toDir.replace("\\","/");
    if (sourceDir == toDir)
    {
        return true;
    }
    if (!QFile::exists(sourceDir))
    {
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist)
    {
        if(coverFileIfExist)
        {
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}

void FormFileDialog::dragEnterEvent(QDragEnterEvent *event)
{
    //拖拽功能
}
