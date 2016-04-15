#include "formfile.h"
#include "ui_formfile.h"
#include <QUrl>
#include <QDropEvent>
#include <QDebug>
#include <QMenu>
#include "FtpModule/ftpclient.h"
#include "FtpModule/FtpDirModel.h"
#include "globalvalue.h"
FormFile::FormFile(QWidget *parent) :
    QWidget(parent),fileModel(new FtpDirModel()),ftpCommandId(0),
    sourceFile(NULL),removeCommand(new QAction(tr("删除"), this)),
    renameCommand(new QAction(tr("重命名"), this)),
    ui(new Ui::FormFile)
{
    ui->setupUi(this);
    ui->lvServer->installEventFilter(this);
    ui->lvServer->setAcceptDrops(true);
    connect(removeCommand, SIGNAL(triggered()), this, SLOT(removeFile()));
    connect(renameCommand, SIGNAL(triggered()), this, SLOT(renameFile()));
    connect(&ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    connect(&ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
}

FormFile::~FormFile()
{
    delete ui;
}

void FormFile::setServerModel(QAbstractItemModel *model)
{
    ui->tvServer->setModel(model);
    ui->lvServer->setModel(model);
}

void FormFile::setClientModel(QAbstractItemModel *model)
{
    ui->lvClient->setModel(model);
}

bool FormFile::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lvServer)
    {
        if(event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent* dee = dynamic_cast<QDragEnterEvent*>(event);
            dee->acceptProposedAction();
            return true;
        }
        else if(event->type() == QEvent::Drop)
        {
            QDropEvent* de = dynamic_cast<QDropEvent*>(event);
            QList<QUrl>urls = de->mimeData()->urls();
            if(urls.isEmpty())
            {
                return true;
            }
            QString path = urls.first().toLocalFile();
            //上传文件
            updateFile(path, path.split("/").last());
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void FormFile::showEvent(QShowEvent *)
{
    ftpCommandId = ftp.connectToHost(getGlobalValue(Maintain::LaneIp).toString());
}

void FormFile::hideEvent(QHideEvent *)
{
    getFtpClient()->close();
}

void FormFile::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu();
    menu->addAction(removeCommand);
    menu->addAction(renameCommand);
    menu->exec(event->globalPos());
    delete menu;
}

void FormFile::on_tvServer_clicked(const QModelIndex &index)
{
    if(FtpDirModel::getUrlInfo(index).isDir())
    {
        ui->lvServer->setRootIndex(index);
    }
    ftp.cd(FtpDirModel::getDir(index));
}

void FormFile::on_lvServer_doubleClicked(const QModelIndex &index)
{
    if(FtpDirModel::getUrlInfo(index).isDir())
    {
        ui->lvServer->setRootIndex(index);
        ftp.cd(FtpDirModel::getDir(index));
    }
    if(FtpDirModel::getUrlInfo(index).isFile())
    {

    }
}

void FormFile::on_btnServerUp_clicked()
{
    ui->lvServer->setRootIndex(ui->lvServer->currentIndex().parent());
}

void FormFile::ftpCommandFinished(int id, bool error)
{
    if(id == ftpCommandId)
    {
        switch(ftp.currentCommand())
        {
        case QFtp::ConnectToHost:
            if(!error)
            {
                ftpCommandId = ftp.login(getGlobalValue(Maintain::LaneUser).toString(),
                                         getGlobalValue(Maintain::LanePassword).toString());
            }
            break;
        case QFtp::Login:
            if(!error)
            {
                fileModel->setUrl(QUrl("ftp://" + getGlobalValue(Maintain::LaneIp).toString()));
                fileModel->setLogin(getGlobalValue(Maintain::LaneUser).toString(),
                                    getGlobalValue(Maintain::LanePassword).toString());
                setServerModel(fileModel);
            }
            break;
        case QFtp::Close:
            break;
        case QFtp::Get:
            break;
        case QFtp::Put:
            if(!error)
            {
                fileModel->refresh(ui->lvServer->rootIndex());
                sourceFile->close();
                delete sourceFile;
                sourceFile = NULL;
            }
            ui->progressBar->reset();
            break;
        case QFtp::Remove:
            if(!error)
            {
                fileModel->refresh(ui->lvServer->rootIndex());
            }
            break;
        case QFtp::Mkdir:
            break;
        case QFtp::Rmdir:
            break;
        case QFtp::Rename:
            break;
        default:
            break;
        }
    }
}

void FormFile::updateProgress(qint64 currentValue, qint64 totalValue)
{
    ui->progressBar->setMaximum(int(totalValue));
    ui->progressBar->setValue(int(currentValue));
}

void FormFile::removeFile()
{
    ftpCommandId = ftp.remove("./" + ui->lvServer->currentIndex().data().toString());
}

void FormFile::renameFile()
{

}

void FormFile::updateFile(const QString &sourceFileName, const QString &destinationFileName)
{
    if(sourceFile != NULL)
    {
        sourceFile->close();
        delete sourceFile;
    }
    sourceFile = new QFile(sourceFileName);
    if(sourceFile->open(QIODevice::ReadWrite))
    {
        ftpCommandId = ftp.put(sourceFile, "./" + destinationFileName);
    }
}
