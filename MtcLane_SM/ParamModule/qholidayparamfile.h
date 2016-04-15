#ifndef QHOLIDAYPARAMFILE_H
#define QHOLIDAYPARAMFILE_H

#include <QObject>
#include <QDateTime>
#include "qbaseparamfile.h"
#pragma pack(push,1)
struct SHolidayContainer
{
    char szDesc[65];              //节假日描述
    quint16 vc1;
    quint16 vc2;
    quint16 vc3;
    quint16 vc4;
    quint16 vc11;
    quint16 vc12;
    quint16 vc13;
    quint16 vc14;
    quint16 vc15;
    quint16 vc21;
    quint16 vc22;
    quint16 overRate;
    char szFreeBegin[15];
    char szFreeEnd[15];
    char szLastVer[15];
    char szUseTime[15];
};
#pragma pack(pop)

struct SHolidayValue
{
    QString desc;
    quint16 vc1;
    quint16 vc2;
    quint16 vc3;
    quint16 vc4;
    quint16 vc11;
    quint16 vc12;
    quint16 vc13;
    quint16 vc14;
    quint16 vc15;
    quint16 vc21;
    quint16 vc22;
    quint32 overRate;
    QDateTime freeBegin;
    QDateTime freeEnd;
    QString lastVer;
    QString useTime;
};

class QHolidayParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QHolidayParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<int ,SHolidayValue> m_container;
signals:
    
public slots:
public:
    bool getUsedValue(SHolidayValue& value);
    bool getVTFreeValue(int vehClass, int& rate);
};

#endif // QHOLIDAYPARAMFILE_H
