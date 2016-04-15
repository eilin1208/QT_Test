#ifndef FORMFILEDIALOG_H
#define FORMFILEDIALOG_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>

Q_DECLARE_METATYPE(QFileInfo);
namespace Ui {
class FormFileDialog;
}

class FormFileDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FormFileDialog(QWidget *parent = 0);
    ~FormFileDialog();
public slots:
    void clickItems(QTreeWidgetItem* item,int index);
    void clickUpListItems(QListWidgetItem* item);
    void clickDownListItems(QListWidgetItem* item);
    void clickTransPushbutton();
    void clickSavePushbutton();
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
private:
    //把指定目录文件下文件显示为treeWidget
    void AddFileToTree(const QString &path,QTreeWidget *father=NULL,QTreeWidgetItem *fatherItem=NULL);
    //把制定目录的文件显示为listWidget
    void AddFileTOListWidget(const QString &path,QListWidget *listwidget);
    //文件拷贝
    bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);
    bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);
private:
    Ui::FormFileDialog *ui;
    QList<QTreeWidgetItem *> listItem;
    QList<QString> listtree1OpenFileName;
    QList<QString> listtree2OpenFileName;
    bool bturnUp;
    //temp
    QWidget *tempWidget;
    QVBoxLayout *tempLayout;
    QTreeWidget *tempTreeWidget;
    QPushButton *temppushbutton;
    //copy path
    QString targetPath;
    QString errorMessage;
};

#endif // FORMFILEDIALOG_H
