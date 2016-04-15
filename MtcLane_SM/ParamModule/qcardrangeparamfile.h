#ifndef QCARDRANGEPARAMFILE_H
#define QCARDRANGEPARAMFILE_H
#include "qbaseparamfile.h"
#include <QObject>
#include <QDateTime>
#pragma pack(push,1)
struct SCardRangeContainer
{
    char szCardID[13];
    char szOccurTime[15];
    char szExpireTime[15];
    quint16 wFreeStationCnt;
    char szFreeInfo[80];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SCardRangeValue
{
    QString sCardID;
    QDateTime sOccurTime;
    QDateTime sExpireTime;
    quint16 wFreeStationCnt;
    quint8 sFreeInfo[80];
    QString szLastVer;
    QDateTime szVerUseTime;
public:
    void clear()
    {
        sCardID="";
        //QDateTime sOccurTime;
        //QDateTime sExpireTime;
        wFreeStationCnt=0;
        //quint8 sFreeInfo[80];
        memset(sFreeInfo,0,sizeof(sFreeInfo));
        szLastVer="";
        //QDateTime szVerUseTime;

    }
};
class QCardRangeParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QCardRangeParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool QueryCardRange(quint32 cardID,SCardRangeValue& value);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<quint32,SCardRangeValue> m_container;
signals:

public slots:

};

#endif // QCARDRANGEPARAMFILE_H
