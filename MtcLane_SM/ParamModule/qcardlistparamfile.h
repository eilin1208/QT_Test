#ifndef QCARDLISTPARAMFILE_H
#define QCARDLISTPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"

#pragma pack(push,1)
struct SCardBListContainer
{
    char szCard[17];
    quint16 wCardType;
    char szCardNet[5];
    quint16 wBlistType;
    char szBListDesc[65];
    char szOccurTime[15];
    char szExpireTime[15];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)
struct SCardBListValue
{
    QString sCard;              //卡号
    quint16 wCardType;            //卡类型
    QString sCardNet;           //卡片网络号
    quint16 wBlistType;           //黑名单类型
    QString sBListDesc;         //黑名单描述
    QDateTime sOccurTime;         //生效时间
    QDateTime sExpireTime;        //失效时间
    QString sLastVer;           //最后版本号
    QDateTime sVerUseTime;        //原始版本号

};
struct SCardBListKey
{
    QString sCard;
    quint16 wCardType;
    QString sCardNet;
public:
    friend bool operator <(const SCardBListKey &a,const SCardBListKey &b);
};
class QCardListParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QCardListParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool IsBlackCard(const QString& sCard,const QString &sCardNet, quint8 &wCardType,quint16 &wBListType,QString &sBListDesc);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SCardBListKey,SCardBListValue> m_container;
signals:

public slots:

};

#endif // QCARDLISTPARAMFILE_H
