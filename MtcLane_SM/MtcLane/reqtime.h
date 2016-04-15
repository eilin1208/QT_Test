#ifndef REQTIME_H
#define REQTIME_H

#include <QObject>
#include "CommonModule/qpausablethread.h"
#include "CommonModule/qlanesocket.h"
class ReqTime : public QPausableThread
{
    Q_OBJECT
public:
    explicit ReqTime(QObject *parent = 0);
    virtual ~ReqTime();
signals:
    
public slots:
public:
    bool ReqNewTime(const QString& sIp, bool bRunThread = true);
protected:
    void RunOnce();
private:
    bool TryReadBuffer(char * buffer, const int nByteLen, const int nTimeOut);
    bool AdjustTime(const QDateTime& newTime);
private:
    quint32 m_nMaxBufSize;
    char* m_pDataBuffer;
    int m_nBufLength;
    QLaneSocket* m_iClient;
    QString m_sIp;
    bool m_bFirstRun;

};

ReqTime* getReqTime();
#endif // REQTIME_H
