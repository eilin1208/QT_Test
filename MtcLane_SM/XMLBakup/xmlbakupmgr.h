#ifndef XMLBAKUPMGR_H
#define XMLBAKUPMGR_H

#include <QObject>
#include "xmlmovethread.h"
class XMLBakupMgr : public QObject
{
    Q_OBJECT
public:
    explicit XMLBakupMgr(QObject *parent = 0);
    ~XMLBakupMgr();
signals:
    
public slots:
    void bakupFinish();
public:
    //初始化
    void init();
protected:
    //定时进行文件转移
    void timerEvent(QTimerEvent *event);
private:
    int getBakUpHour();
private:
    XMLMoveThread m_xmlThread;
};

inline XMLBakupMgr* getXMLBakupMgr()
{
    static XMLBakupMgr mgr;
    return &mgr;
}

#endif // XMLBAKUPMGR_H
