#ifndef QALLRATEPARAMFILE_H
#define QALLRATEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include "parammapcontainer.h"
#include <QVector>
//文件存储结构
#pragma pack(push,1)
struct SAllRateContainer
{
    char szStationIn[7];
    char szStationOut[7];
    quint16 wStationNum;
    char szPathInfo[601];
    quint16 wCellNum;
    quint32 dwTollDistance;
    quint32 dwRealDistance;
    char szWayFlag[2];
    char szTollInfo[49];
    char szLastVer[15];
    char szVerUseTime[15];
};
#pragma pack(pop)

struct SPrice
{
   int vehCode;
   quint16 price;
};
struct SAllRateValue
{
    quint32 dwStationIn;                //	入口站
    quint32 dwStationOut;               //	出口站
    quint16 wStationNum;                //	经过站数量
    QString sPathInfo;                  //	路径信息
    QList<quint32> tPassStaAry;       //	路径信息，包含所有经过的收费站，由wStationCnt和sPathInfo解析出
    quint16 wCellNum;                   //	收费单元数量
    quint32 dwTollDistance;             //	计费里程（米）
    quint32 dwRealDistance;             //	实际里程（米）
    QString szWayFlag;                  //	方向标志
    QString sTollInfo;                  //	价目信息，支持12种车型，单位到元，4位一个车型价目
    QMap<int, quint16> tPriceAry;       //	价目信息，由sTollInfo解析出
    QString tmLastVer;                  //	最后版本号
    QString tmVerUsertime;              //	版本生效时间

public:
    // 取某车型价目
    bool getPrice(quint16 &nPrice, int nVehClass)
    {
        if (tPriceAry.find(nVehClass)==tPriceAry.end())
        {
            return false;
        }
        nPrice=tPriceAry[nVehClass];
        return true;
    }

};
struct SAllRateKey
{
public:
    quint32 nSta1;
    quint32 nSta2;
public:
    friend bool operator <(const SAllRateKey &a,const SAllRateKey &b);
};
//wumax ==
const QString UNIFIED_ROUTE_RATE_CODE="01";
const QString UNIFIED_POINT_RATE_CODE="51";
//
class QAllRateParamFile : public QBaseParamFile
{
    Q_OBJECT
private:
    bool ParsePassStationInfo(const QString& sPathInfo,quint16 nStaCount,QList<quint32> &aryStations);
    bool ParseVCPrice(const QString& sTollInfo,QMap<int, quint16> &aryToll);
public:
    bool queryTollValue(SAllRateValue &tollValue, quint32 nSta1, quint32 nSta2);
    bool queryFarthestStaAndMinimumFare(quint32 *pFarthestSta,
        quint16 *pMinimumFareInYuan, QList<quint32> enStaList, quint32 nSta2);
public:
    explicit QAllRateParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SAllRateKey,SAllRateValue> m_container;
signals:

public slots:

};

#endif // QALLRATEUNITPARAMFILE_H
