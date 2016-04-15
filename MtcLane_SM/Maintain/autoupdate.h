#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QObject>

class AutoUpdate : public QObject
{
    Q_OBJECT
public:
    explicit AutoUpdate(QObject *parent = 0);
public:
    //软件更新
    bool softWareUpdate();
    //启动软件
    bool startSoftware();
    //获取错误描述
    QString getErrorMessage();
private:
    //判断是否有新文件要更新
    bool haveUpdateFile();
    //备份现在的软件
    bool softWareBackup();
    //拷贝新的软件
    bool copyNewSoftWare();
    //删除新程序
    bool delNewSoftWare();
    //设置错误描述
    void setErrorMessage(QString sError);
private:
    QString sNewPath;
    QString sOldPath;
    QString sErrMessage;
};

#endif // AUTOUPDATE_H
