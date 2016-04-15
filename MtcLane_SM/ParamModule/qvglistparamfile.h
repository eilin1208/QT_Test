#ifndef QVGLISTPARAMFILE_H
#define QVGLISTPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"

#pragma pack(push,1)
struct SVGListContainer
{
    char szVLP[13];
    quint16 wVLPColor;
    quint16 wVGType;
    char VGTDes[65];
    char szOccurTime[15];
    char szExpireTime[15];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SVGListValue
{
    QString sVLP;          //车牌号
    quint16 wVLPColor;       //车牌颜色
    quint16 wVGType;         //灰名单类型
    QString sVGTDes;          //灰名单描述
    QDateTime sOccurTime;    //生效时间
    QDateTime sExpireTime;   //失效时间
    QString sLastVer;      //最后版本号
    QDateTime sVerUseTime;   //版本生效时间
};
struct SVGListKey
{
    QString sVLP;
    quint16 wVLPColor;
    //QString sRecordNum;
public:
    friend bool operator <(const SVGListKey &a,const SVGListKey &b);
};

class QVGListParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QVGListParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool IsGrayVeh(const QString& sVlp, const quint16 wVlpColor, SVGListValue& value);
    bool IsGrayVeh(const QString& sVlp, SVGListValue& value);
protected:
    bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SVGListKey,SVGListValue> m_container;
signals:

public slots:

};

#endif // QVGLISTPARAMFILE_H
