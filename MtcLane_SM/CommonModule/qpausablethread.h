#ifndef QPAUSABLETHREAD_H
#define QPAUSABLETHREAD_H

#include <QObject>
#include <QtGui>
#include <QWaitCondition>
class QPausableThread : public QThread
{
    Q_OBJECT
public:
    explicit QPausableThread(QObject *parent = 0);
    virtual ~QPausableThread();
protected:
    void run();
    virtual void RunOnce();
signals:

public slots:
    //暂停线程
    void Pause();
    //继续执行
    void Resume();
    //停止线程
    void Stop();

    inline bool isPause(){return !m_bContinue;}
    void CheckExit(unsigned long time = ULONG_MAX);
protected:
    QWaitCondition m_waitCondition;
    QMutex m_continueMutex;
    QMutex m_stopMutex;
    volatile bool m_bContinue;
    volatile bool m_bStop;
};

#endif // QPAUSABLETHREAD_H
