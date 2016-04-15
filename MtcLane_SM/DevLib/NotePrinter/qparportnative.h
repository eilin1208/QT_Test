#ifndef QPARPORTNATIVE_H
#define QPARPORTNATIVE_H

#include <QIODevice>
#include <QSocketNotifier>

class QParPortNative : public QIODevice
{
    Q_OBJECT
public:
    //explicit QParPortNative(QObject *parent = 0);
    QParPortNative(const QString& portName, QObject* parent = 0);
    virtual ~QParPortNative()
    {

    }
protected:
    bool openImpl();
    void closeImpl();
    /** Gets the error message related to the last error occurred. */
    QString lastErrorTextImpl() const;
    /** Gets the last native error. */
    int lastErrorImpl() const;
    qint64 bytesAvailableImpl() const;
    int waitForReadyReadImpl(int timeout);

    qint64 readDataImpl(char *data, qint64 maxlen);
    qint64 writeDataImpl(const char *data, qint64 len);
    bool isOnlineImpl();

signals:
    
public slots:

protected:
    QString m_portName;
    int m_fdPar;
    QSocketNotifier* m_notifier;
};

#endif // QPARPORTNATIVE_H
