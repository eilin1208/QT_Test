#ifndef FORMLOG_H
#define FORMLOG_H
#include <QSettings>
#include <QFileInfoList>
#include <QDebug>
#include <QWidget>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QFile>
namespace Ui {
    class FormLog;
}

class FormLog : public QWidget
{
    Q_OBJECT

public:
    explicit FormLog(QWidget *parent = 0);
    ~FormLog();

public slots:
    //双击选中的日志，将打开选中的日志并将其中的内容读到QTextEdit中
    void SetLogStr(QTreeWidgetItem *item,int column);
public:
    //获取相关目录下的Log文件，文件目录路径从INI文件中读取，最后可能更改
    void GetLogFile();
    //将Log文件的文件名,描述、大小、时间等添加到m_listItemInfo中，便于在QTreeWidget中显示,
    void GetLogInfo(QTreeWidget *widget);
    //根据日志文件名确定返回哪个日志描述
    QString SetLogDescribe(QString filestr);

private slots:
    void on_pushButton_Search_clicked();

private:
    Ui::FormLog *ui;
    //用于获取日志文件的文件名,显示在TreeWidget中
    QList<QTreeWidgetItem *> m_listItemInfo;
    //保存文件信息
    QFileInfoList m_listLogInfo;
    //当前日志目录路径
    QString m_dirPath;
    //要在界面显示内容的日志名
    QString m_logName;
    //打开的文件
    QFile m_file;
    //要查询的字符串
    QString m_searchData;
};

#endif // FORMLOG_H
