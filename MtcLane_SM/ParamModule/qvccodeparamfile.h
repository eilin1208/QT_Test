#ifndef QVCCODEPARAMFILE_H
#define QVCCODEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"

#pragma pack(push,1)
struct SVCCodeContainer
{
    quint16 wVCCode;
    char szVCName[11];
    quint16 wOverSpeed;
    quint16 wVUSpeed;
    quint16 wMaxSpeed;
    quint32 dwMinVWeight;
    quint32 dwMaxVWeight;
    quint16 wMinAxle;
    quint16 wMaxAxle;
    char szVCDesc[65];
    char szLastVer[15];
    char szUseTime[15];
};
#pragma pack(pop)

struct SVCCodeValue
{
    quint16 wVCCode;        //	车型编码
    QString sVCName;        //	车型名称
    quint16 wOverSpeed;     //	超时时速
    quint16 wVUSpeed;       //	U型车计费时速
    quint16 wMaxSpeed;      //	最高时速
    quint32 dwMinVWeight;   //	最低重量
    quint32 dwMaxVWeight;   //	最高重量
    quint16 wMinAxle;       //	最少轴数
    quint16 wMaxAxle;       //	最多轴数
    QString sVCDesc;        //	特征描述
    QString sLastVer;       //	最后版本号
    QDateTime sUseTime;     //	版本生效时间
};
class QVCCodeParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QVCCodeParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool queryVehClassInfo(SVCCodeValue &vehClassInfo, int nVehClass);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<quint16,SVCCodeValue> m_container;
signals:

public slots:

};

#endif // QVCCODEPARAMFILE_H
