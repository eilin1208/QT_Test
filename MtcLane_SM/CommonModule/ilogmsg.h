#ifndef ILOGMSG_H
#define ILOGMSG_H

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
class ILogMsg : public QObject
{
    Q_OBJECT
public:
    explicit ILogMsg(QObject *parent = 0);
    virtual void LogMsg(const QString& sTitle, const QString& sContent, bool bWnd = false)
    {
        ////qDebug() << tr("[%1]:%2").arg(sTitle).arg(sContent);
    }
signals:

public slots:

};
namespace DONOTUSE
{
    ILogMsg * getLogger();
    static QMutex g_Mutex;
}
void LogMsg(const QString& sTitle, const QString& sContent, bool bWnd = false);
void SetLogger(ILogMsg* log);
#endif // ILOGMSG_H
