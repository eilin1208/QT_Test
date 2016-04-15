#ifndef QAREACODEPARAMFILE_H
#define QAREACODEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include "parammapcontainer.h"
#include <QDateTime>
#pragma pack(push,1)
struct SAreaCodeContainer
{
    char szAreaCode[5];
    char szAreaname[33];
    char szRoad[7];
    quint16 wStoreCardAgio;
    quint16 wTallyCardAgio;
    char szSpVTFlag[33];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)

struct  SAreaCodeValue
{
    QString sAreaCode;		//	省级区域代码
    QString sAreaname;		//	省级区域名称
    QString sRoad;			//	路段代码
    quint16 wStoreCardAgio;	//	储值卡折扣率
    quint16 wTallyCardAgio;	//	记账卡折扣率
    QString sSpVTFlag;		//	特殊车种启用标志
    QString sLastVer;		//	最后版本号
    QDateTime sVerUseTime;		//	版本生效时间
public:
    SAreaCodeValue()
    {
        wStoreCardAgio = 0;
        wTallyCardAgio = 0;
    }
};
struct SAreaCodeKey
{
    QString sAreaCode;
    QString sRoad;
public:
    friend bool operator <(const SAreaCodeKey &a,const SAreaCodeKey &b);
};
class QAreaCodeParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QAreaCodeParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool QryAreaCode(const QString &sAreaCode, SAreaCodeValue *pValue);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SAreaCodeKey,SAreaCodeValue> m_container;
signals:

public slots:

};

#endif // QAREACODEPARAMFILE_H
