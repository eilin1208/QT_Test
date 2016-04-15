#ifndef QPARPORT_H
#define QPARPORT_H

#include "qparportnative.h"
class QParPort : public QParPortNative
{
    Q_OBJECT
public:
    //explicit QParPort(QObject *parent = 0);
    QParPort(const QString& portName, QObject *parent = 0);
    ~QParPort()
    {

    }

signals:
    
public slots:
    
public:
    bool open(OpenMode mode = ReadWrite);

    void close();

    qint64 pos();

    qint64 size();

    bool atEnd();

    qint64 bytesAvailable();

    qint64 bytesToWrite();

    bool canReadLine();

    bool waitForReadyRead(int msecs);

    bool waitForBytesWritten(int msecs);

    bool isOnline();
protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private slots:
    void onDataReceived();

protected:
    volatile qint64 pendingByteCount_;    // internal counter for pending bytes in the driver character buffer.
    volatile bool doNotify_;              // internal state flag to control when to notify the user about incoming data.
    volatile int readNotifyThreshold_;    // minimum number of pending bytes required to emit readyRead() signal.

    Q_DISABLE_COPY(QParPort)
};

#endif // QPARPORT_H
