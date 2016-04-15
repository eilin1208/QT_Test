#ifndef QPSAMWHITELPARAMFILE_H
#define QPSAMWHITELPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
struct SPSamWhiteKey
{
    //QString sPSam;
    QString sTermId;
public:
    friend bool operator <(const SPSamWhiteKey &a,const SPSamWhiteKey &b);
};

struct SPSamWhiteValue
{
    QString sPSam;                   //Psam卡卡号
    quint16 wPSamType;                 //Psam卡种类
    QString sTermCode;               //终端机编号
    quint16 wOccurOrgType;             //生效机构类型
    QString sOccurOrg;               //生效机构代码
    QString sUseLane;                //生效车道代码
    QString sStatus;                 //状态
    QString sLastVer;                //最后版本号
    QDateTime sVerUseTime;             //版本生效时间
};

#pragma pack(push,1)
struct SPSamWhiteContainer
{
    char szPSam[21];
    quint16 wPSamType;
    char szTermCode[24];
    quint16 wOccurOrgType;
    char szOccurOrg[7];
    char szUseLane[3];
    char szStatus[2];
    char szLastVer[9];
    char szVerUseTime[15];
};
#pragma pack(pop)
class QPSamWhiteParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QPSamWhiteParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SPSamWhiteKey, SPSamWhiteValue> m_container;
signals:

public slots:

};

#endif // QPSAMWHITELPARAMFILE_H
