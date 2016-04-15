#ifndef QBASERATEPARAMFILE_H
#define QBASERATEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include "parammapcontainer.h"
#include <QtEndian>
#include <QDateTime>
#pragma pack(push,1)
struct SBaseRateContainer
{
    char szRateCode[3];
    char szRateDesc[33];
    quint16 wRateParaType;
    quint16 wVC;
    quint16 wVCRate;
    quint16 wWBaseRate;
    quint16 wWMMinParam;
    quint16 wAMinParam;
    quint16 wWAMaxParam;
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)

struct  SBaseRateValue
{
public:
    enum BASE_RATE_PARA_TYPE
    {
        ROUTE_VEH_CLASS=1,              // 01-路车型费率参数
        ROUTE_WEIGHT_UNDER_LIMIT=2,     // 02-路计重费率参数
        ROUTE_WEIGHT_EXCEED_LIMIT=3,    // 03-路吨公里超限费率参数
        POINT_VEH_CLASS=5,              // 05-桥隧车型费率参数
        POINT_WEIGHT_UNDER_LIMIT=6,     // 06-桥隧计重费率参数
        POINT_WEIGHT_EXCEED_LIMIT=7     // 07-桥隧吨公里超限费率参数

//        ROUTE_VEH_CLASS=1,						//	01-路车型费率参数
//        ROUTE_WEIGHT=2,				        //	02-路计重费率参数
//        POINT_VEH_CLASS=5,						//	05-桥隧车型费率参数
//        POINT_WEIGHT=6				        //	06-桥隧计重费率参数
    };
public:
     QString sRateCode;
     QString sRateDesc;
     quint16 wRateParaType;
     quint16 wVC;
     quint16 wVCRate;
     quint16 wWBaseRate;
     quint16 wWMMinParam;
     quint16 wAMinParam;
     quint16 wWAMaxParam;
     QString sLastVer;
     QDateTime sVerUseTime;
public:
};
struct SBaseRateKey
{
public:
    QString sRateCode;      // 费率编号
    quint16 wRateParaType;  // 费率参数类型
    quint16 wVC;            // 车型
public:
    friend bool operator <(const SBaseRateKey & lp, const SBaseRateKey & rp);
};
class QBaseRateParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QBaseRateParamFile(QObject *parent = 0);
    virtual void InitParamFile();
     bool queryBaseRate(SBaseRateValue &BaseRate, const QString &sRateCode, int nRateType, int nVehClass);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SBaseRateKey,SBaseRateValue> m_container;
signals:

public slots:

};

#endif // QBASERATEPARAMFILE_H
