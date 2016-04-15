#ifndef FORMLOGFTP_H
#define FORMLOGFTP_H
#include "FtpModule/ftpclient.h"
#include <QWidget>
#include <QTreeWidgetItem>
namespace Ui {
    class FormLogftp;
}

class FormLogftp : public QWidget
{
    Q_OBJECT

public:
    explicit FormLogftp(QWidget *parent = 0);
    ~FormLogftp();

    //根据日志文件名确定返回哪个日志描述
    QString SetLogDescribe(QString filestr);

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
private slots:
    void on_pushButton_Search_clicked();
    //将车道IP从数据库显示在treeWidget
    void selectDataToTree();
    //将查找到的日志添加到treeWidget
    void selectLogtoTree();
    //双击连接相应车道机获取收费软件运行目录下的日志
    void LaneclickItem(QTreeWidgetItem* item,int index);
    //双击获取对应日志并读取日志
    void LogclickItem(QTreeWidgetItem* item,int index);

    void on_tb_close_clicked();

    void on_tb_connect_clicked();

private:
    Ui::FormLogftp *ui;
    QString m_ip;
    QString m_usr;
    QString m_pwd;
    QString m_logdir;//日志所在文件夹
    QVector<QUrlInfo> m_fileList;//list后得到的文件数组
    QString m_logname;
    QString m_searchData;
};

#endif // FORMLOGFTP_H
