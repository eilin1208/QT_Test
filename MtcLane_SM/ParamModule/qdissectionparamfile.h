#ifndef QDISSECTIONPARAMFILE_H
#define QDISSECTIONPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#pragma pack(push,1)
struct SDisSectionContainer
{
    quint16 wDisType;
    char szSectionID[7];
    char szRoadID[7];
    char szStartStationID[7];
    char szEndStationID[7];
    quint16 wDiscount;
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SDisSectionValue
{
    quint16 wDisType;
    QString sSectionID;
    QString sRoadID;
    QString sStartStationID;
    QString sEndStationID;
    quint16 wDiscount;
    QString LastVer;
    QString sVerUseTime;
};
struct SDisSectionKey
{
    quint32 sSectionID;
    //	quint16 wCardType;
    quint32 sRoadID;
public:
    friend bool operator <(const SDisSectionKey &a,const SDisSectionKey &b);
};
class QDisSectionParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QDisSectionParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SDisSectionKey,SDisSectionValue> m_container;
signals:

public slots:

};

#endif // QDISSECTIONPARAMFILE_H
