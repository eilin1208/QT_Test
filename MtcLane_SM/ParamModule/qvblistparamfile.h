#ifndef QVBLISTPARAMFILE_H
#define QVBLISTPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include <QDateTime>
#pragma pack(push,1)
struct SVBListContainer
{
    char szVLP[13];
    quint16 wVLPColor;
    quint16 wVBType;
    char szVBTDes[65];
    char szOccurTime[15];
    char szExpireTime[15];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SVBListValue
{
    QString sVLP;                //车牌号
    quint16 wVLPColor;             //车牌颜色
    quint16 wVBType;               //黑名单类型
    QString sVBTDes;                //黑名单描述
    QDateTime sOccurTime;          //生效时间
    QDateTime sExpireTime;         //失效时间
    QString sLastVer;            //最后版本号
    QDateTime sVerUseTime;         //版本生效时间
};
struct SVBListKey
{
    QString sVLP;
    quint16 wVLPColor;
public:
    friend bool operator <(const SVBListKey &a,const SVBListKey &b);
};
class QVBlistParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QVBlistParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool IsBlackVeh(const QString &sVLP,quint16 wVLPColor,quint16 &wVBType);
    bool IsBlackVeh(const QString &sVLP,quint16 &wVBType);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SVBListKey,SVBListValue> m_container;
signals:

public slots:

};

#endif // QVBLISTPARAMFILE_H
