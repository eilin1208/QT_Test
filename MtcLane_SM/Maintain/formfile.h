#ifndef FORMFILE_H
#define FORMFILE_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QFtp>
#include <QFile>
namespace Ui {
class FormFile;
}

class FtpDirModel;

class FormFile : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormFile(QWidget *parent = 0);
    ~FormFile();
    //设置车道端model
    void setServerModel(QAbstractItemModel* model);
    void setClientModel(QAbstractItemModel* model);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void on_tvServer_clicked(const QModelIndex &index);

    void on_lvServer_doubleClicked(const QModelIndex &index);

    void on_btnServerUp_clicked();

    void ftpCommandFinished(int id, bool error);

    void updateProgress(qint64 currentValue, qint64 totalValue);

    void removeFile();

    void renameFile();
private:
    void updateFile(const QString &sourceFileName, const QString &destinationFileName);
private:
    FtpDirModel* fileModel;
    QFtp ftp;
    int ftpCommandId;
    QFile *sourceFile;
    QAction *removeCommand;
    QAction *renameCommand;
private:
    Ui::FormFile *ui;
};

#endif // FORMFILE_H
