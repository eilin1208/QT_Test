#ifndef QSHIFTPARAMFILE_H
#define QSHIFTPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include <QDateTime>
#pragma pack(push,1)
struct SShiftRowContainer
{
    char szStation[7];
    char szBLNetID[7];
    char szBLRoadID[7];
    quint16 wShift;
    char szShiftName[21];
    char szVerStartTime[9];
    char szVerEndTime[9];
    quint16 wTimeDiff;
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SShiftValue
{
    QString	sStation;			//1 收费站代码
    QString     sBLNetID;			//2 所属区域中心
    QString	szBLRoadID;			//3 所属路段代码
    quint16	wShift;                         //4 班次代码
    QString	sShiftName;                     //5 班次名称
    QTime	stVerStartTime;                 //6 班次起始时间
    QTime	stVerEndTime;                   //7 班次截止时间
    quint16     wTimeDiff;                      //  时间位移
    QString	sLastVer;			//11 最后版本号
    QDateTime	sVerUseTime;                    //12 版本生效时间
    QDate iWorkDate;
public:
    void Clear()
    {
        wShift =0;
    }

    void operator=(const SShiftValue &b)
    {
        this->sStation =b.sStation;
        this->wShift = b.wShift;
        this->sShiftName = b.sShiftName;
        this->stVerStartTime = b.stVerStartTime;
        this->stVerEndTime = b.stVerEndTime;
        this->wTimeDiff = b.wTimeDiff;
        this->iWorkDate = b.iWorkDate;
        return;
    }
};
class QShiftParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QShiftParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    //根据时间获取所有可用工班
    void getShiftInfoByTime(const QDateTime &time, QList<SShiftValue>& usableShift);
    //获取第一个可用工班
    bool getFirstUsableShift(SShiftValue& shiftParam, bool bDelay);
    //根据工班号获取工班信息
    SShiftValue getShiftInfoByNo(quint16 wShiftNo);
    //获取第一个工班信息
    SShiftValue getFirstShift();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<quint16,SShiftValue> m_container;
signals:

public slots:

private:
    QDate getWorkDate(const QDateTime& time, quint16 wShiftNum);
};


#endif // QSHIFTPARAMFILE_H
