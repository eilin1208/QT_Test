#ifndef QSPRATEPARAMFILE_H
#define QSPRATEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"

#pragma pack(push,1)
struct SSpRateRowContainer
{
    char szSpStation1[7];
    char szSpStation2[7];
    quint16 wVC;
    char szRateDesc[33];
    quint16 wSpVCRate;
    quint16 wSpTrunkMinRate;
    char szSpVCRateFlag[2];
    quint16 wSpFlagStation;
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)

struct SSpRateValue
{
    quint32 dwSpStation1;		//	例外入口站1
    quint32 dwSpStation2;		//	例外出口站2
    quint16 wVC;				//	车型
    QString sRateDesc;		//	例外费率描述（该费率编号的描述）
    quint16 wSpVCRate;			//	例外车型收费指定值
    quint16 wSpTrunkMinRate;	//	例外货车最低收费标准
    QString sSpVCRateFlag;	//	例外是否不超限货车按车型收费标志
    quint16 wSpFlagStation;	//	例外经过标示站可用标志
    QString sLastVer;		//	最后版本号
    QDateTime sVerUseTime;		//	版本生效时间
};
struct SSpRateKey
{
    quint32 dwSpStation1;
    quint32 dwSpStation2;
    quint16 wVC;
public:
    friend bool operator <(const SSpRateKey &a,const SSpRateKey &b);
};
class QSpRateParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QSpRateParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool querySpecialTollInfo(SSpRateValue &specialRate, quint32 nSta1, quint32 nSta2, int nVehClass);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SSpRateKey,SSpRateValue> m_container;
signals:

public slots:

};

#endif // QSPRATEPARAMFILE_H
