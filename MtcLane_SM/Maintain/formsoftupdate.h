#ifndef FORMSOFTUPDATE_H
#define FORMSOFTUPDATE_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QAction>
namespace Ui {
    class FormSoftUpdate;
}

class FormSoftUpdate : public QWidget
{
    Q_OBJECT

public:
    explicit FormSoftUpdate(QWidget *parent = 0);
    ~FormSoftUpdate();
    bool eventFilter(QObject *watched, QEvent *event);
protected:
    //显示的时候更新显示IP的treeWidget
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private slots:
    //获得要更新的文件（软件）
    void on_toolButton_Soft_clicked();

    void on_toolButton_update_clicked();
    //连接IP车道机
    void on_toolButton_link_clicked();

    void on_toolButton_disconnect_clicked();

    void selectDataToTree();

    void clickItem(QTreeWidgetItem* item,int index);

    void removeItem();

    void ReflashTree();

private:
    Ui::FormSoftUpdate *ui;
    QString sourceFileName;
    QString destinationFileName;
    QString destinationDir;
    QAction *removeCommand;
    QAction *renameCommand;
};

#endif // FORMSOFTUPDATE_H
