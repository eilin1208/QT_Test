#ifndef QLANEEVENTCODEPARAMFILE_H
#define QLANEEVENTCODEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"

#pragma pack(push,1)
struct SLaneEvenCodeContainer
{
    quint16 wEventCode;				//	事件编码
    char szCaption[33];				//	事件名称
    char szSuperAuthorize[2];		//	是否监控确认
    quint16 wSuperOverTime;			//	监控超时时间
    quint16 wSuperConfirmTime;			//	监控确认时间
    char szMonitorAuthorize[2];		//	是否班长授权
};
#pragma pack(pop)

struct SLaneEventValue
{
    quint16 wEventCode;			//事件代码
    QString sCaption;			//事件名称
    quint8 bSuperAuthorize;		//是否监控认证
    quint16 wSuperOverTime;		//监控超时时间
    quint16 wSuperConfirmTime;		//监控确认时间
    quint8 bMonitorAuthorize;		//班长认证
public:
    SLaneEventValue()
    {
        bSuperAuthorize =0;
        bMonitorAuthorize =0;
        wEventCode =0;
    }
    void  operator=(SLaneEventValue b)
    {
        wEventCode = b.wEventCode;
        sCaption = b.sCaption;
        bSuperAuthorize = b.bSuperAuthorize;
        wSuperConfirmTime = b.wSuperConfirmTime;
        wSuperOverTime = b.wSuperOverTime;
        bMonitorAuthorize = b.bMonitorAuthorize;
    }
};
class QLaneEventCodeParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QLaneEventCodeParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<qint16,SLaneEventValue> m_container;
signals:

public slots:

};

#endif // QLANEEVENTCODEPARAMFILE_H
