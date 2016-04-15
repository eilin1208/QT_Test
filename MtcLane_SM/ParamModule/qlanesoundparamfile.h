#ifndef QLANESOUNDPARAMFILE_H
#define QLANESOUNDPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
struct SLaneSoundKey
{
    quint16 wSoundEvent;
    int nEnExFlag;
    QString sVCFlag;
public:
    friend bool operator <(const SLaneSoundKey &a,const SLaneSoundKey &b);
};

struct SLaneSoundValue
{
    quint16 wSoundEvent;			//	语音事件代码
    int nEnExFlag;				//	出入口标志  1,入口 2出口
    QString sVCFlag;				//	车型标志
    QString sOverWeightFlag;		//	超限标志
    QString sSoundEventName;		//	语音事件名称
    QString sSoundFileName;		//	对应语音文件名称
    QString sSoundDesc;			//	短语内容
    QString sLastVer;			//	最后版本号
    QDateTime sVerUseTime;			//	版本生效时间
};

#pragma pack(push,1)
struct SLaneSoundContainer
{
    quint16 wSoundEvent;//  1,车辆到来 2，操作完成 3车辆放行
    char szEnExFlag[2];
    char szVCFlag[2];
    char szOverWeightFlag[2];
    char szSoundEventName[101];
    char szSoundFileName[101];
    char szSoundDesc[201];
    char szLastVer[9];
    char szVerUseTime[15];
};
#pragma pack(pop)
class QLaneSoundParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QLaneSoundParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SLaneSoundKey, SLaneSoundValue> m_container;
signals:

public slots:

};

#endif // QLANESOUNDPARAMFILE_H
