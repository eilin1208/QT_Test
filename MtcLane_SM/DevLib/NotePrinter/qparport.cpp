#include "qparport.h"
#include <QDebug>

QParPort::QParPort(const QString &portName, QObject *parent):
    QParPortNative(portName, parent)
{
    qDebug() << "对象构建了一次";
}

bool QParPort::open(QIODevice::OpenMode mode)
{
    if( isOpen() )
    {
        return true;
    }

    if(m_portName.isEmpty())
    {
        return false;
    }

    if( !openImpl() )
    {
        if(m_notifier != NULL)
        {
            delete m_notifier;
            m_notifier = NULL;
        }
    }

    setOpenMode( mode | QIODevice::Unbuffered);
    return true;
}

void QParPort::close()
{
    if( isOpen() )
    {
        if(m_notifier)
        {
            disconnect(m_notifier, SIGNAL(activated(int)), this, SLOT(onDataReceived()));
            delete m_notifier;
            m_notifier = NULL;
        }
        closeImpl();
    }

    QIODevice::close();
    qDebug() << "Par Port \"" << m_portName << "\" is successfully closed";

}

qint64 QParPort::pos()
{
    return 0;
}

qint64 QParPort::size()
{
    return bytesAvailable();
}

bool QParPort::atEnd()
{
    return true;
}

qint64 QParPort::bytesAvailable()
{
    qint64 nBytes = bytesAvailableImpl();
    if ( nBytes == -1LL )
    {
        return -1LL;
    }
    return QIODevice::bytesAvailable() + nBytes;
}

qint64 QParPort::bytesToWrite()
{
    return 0LL;
}

bool QParPort::canReadLine()
{
    return false;
}

bool QParPort::waitForReadyRead(int msecs)
{
    bool ret = false;
    m_notifier->setEnabled(false);

    int result = waitForReadyReadImpl((msecs < 0 ? -1 : msecs));
    if ( result > 0 )
    {
        ret = true;
        emit readyRead();
    }
    else if(result == 0)
    {

    }
    else
    {

    }
    m_notifier->setEnabled(true);
    return ret;
}

bool QParPort::waitForBytesWritten(int msecs)
{
    return true;
}

bool QParPort::isOnline()
{
    return this->isOnlineImpl();
}

qint64 QParPort::readData(char *data, qint64 maxlen)
{
    Q_CHECK_PTR(data);
    qint64 bytesRead = readDataImpl(data, maxlen);
    if(bytesRead == -1LL)
    {

    }
    if(bytesRead > 0LL)
    {
        doNotify_ = false;
        pendingByteCount_ = (pendingByteCount_ - bytesRead ) < 0LL ? 0LL :
                                                                     (pendingByteCount_ - bytesRead);
    }

    m_notifier->setEnabled(true);
    return bytesRead;
}

qint64 QParPort::writeData(const char *data, qint64 len)
{
    Q_CHECK_PTR(data);

    qint64 nBytes = writeDataImpl(data, len);
    if(nBytes > 0LL)
    {
        emit bytesWritten(nBytes);
    }
    return nBytes;
}

void QParPort::onDataReceived()
{
    qint64 nBytes = bytesAvailableImpl();
    if(-1LL == nBytes)
    {
        return;
    }

    if ( nBytes > pendingByteCount_ ||
         (nBytes > 0LL && nBytes == pendingByteCount_ && !doNotify_) ) {
        pendingByteCount_ = nBytes;
        doNotify_ = true;

        if ( pendingByteCount_ >= (qint64)readNotifyThreshold_ ) {
            m_notifier->setEnabled(false);
            emit readyRead();
        }
    }


}
