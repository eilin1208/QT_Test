#include "qparportnative.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "/usr/include/linux/ppdev.h"
#include <QDebug>
#include <linux/lp.h>
QParPortNative::QParPortNative(const QString &portName, QObject *parent) :
    m_portName(portName), QIODevice(parent), m_fdPar(0), m_notifier(NULL)
{

}

bool QParPortNative::openImpl()
{
    m_fdPar = ::open(qPrintable(m_portName), O_RDWR);
    if(m_fdPar == -1)
    {
        return false;
    }
    if(ioctl(m_fdPar, PPCLAIM))
    {
        return false;
    }

    m_notifier = new QSocketNotifier(m_fdPar, QSocketNotifier::Read, this);
    Q_CHECK_PTR(m_notifier);
    if(!m_notifier || !connect(m_notifier, SIGNAL(activated(int)), this, SLOT(onDataReceived())))
    {
        qWarning() << QString("QParPortNative::open(%1) failed when connecting to read notifier")
                      .arg(m_portName);
    }
    return true;
}

void QParPortNative::closeImpl()
{
    ::close(m_fdPar);
    m_fdPar = 0;
}

QString QParPortNative::lastErrorTextImpl() const
{
    return strerror(errno);
}

int QParPortNative::lastErrorImpl() const
{
    return errno;
}

qint64 QParPortNative::bytesAvailableImpl() const
{
    int nBytes = 0;
    if(ioctl(m_fdPar, FIONREAD, &nBytes) == -1)
    {
        return -1LL;
    }
    return QIODevice::bytesAvailable() + nBytes;
}

int QParPortNative::waitForReadyReadImpl(int timeout)
{
    fd_set input;
    struct timeval wait, *waitTimeOut = NULL;
    FD_ZERO(&input);
    FD_SET(m_fdPar, &input);
    if(timeout != -1)
    {
        wait.tv_sec = timeout / 1000;
        wait.tv_usec = (timeout % 1000) * 1000;
        waitTimeOut = &wait;
    }

    int max_fd = m_fdPar + 1;
    int num = select(max_fd, &input, NULL, NULL, waitTimeOut);
    if(num > 0)
    {
        if(FD_ISSET(m_fdPar, &input))
        {
            return 1;
        }
        else
        {
            qWarning() << QString("QParPortNative::waitForReadyRead(%1): unexpected value returned from select().")
                          .arg(m_portName);
            return 0;
        }
    }
    return num;
}


qint64 QParPortNative::readDataImpl(char *data, qint64 maxlen)
{
    qint64 numBytes = ::read(m_fdPar, data, maxlen);
    if( numBytes == -1LL && errno != EAGAIN)
    {
        return -1LL;
    }
    return numBytes;
}

qint64 QParPortNative::writeDataImpl(const char *data, qint64 len)
{
    qint64 numBytes = ::write(m_fdPar, data, len);
    if( numBytes == -1 && errno != EAGAIN)
    {
        return -1LL;
    }
    return numBytes;
}

bool QParPortNative::isOnlineImpl()
{
    if(m_fdPar == -1)
    {
        return false;
    }
    int prnstate;

    ioctl(m_fdPar, LPGETSTATUS, &prnstate);
    if(prnstate & LP_PSELECD)
    {
        return true;
    }
    return false;
}




