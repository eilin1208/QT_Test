#ifndef XMLMOVETHREAD_H
#define XMLMOVETHREAD_H

#include <QThread>

class XMLMoveThread : public QThread
{
    Q_OBJECT
public:
    explicit XMLMoveThread(QObject *parent = 0);
    //删除SD卡中的xml以及图片备份文件
    void DelSDPicFile();
    void DelSDXmlFile();
    void movePicFile();
signals:
    
public slots:
    
protected:
    void run();
};

#endif // XMLMOVETHREAD_H
