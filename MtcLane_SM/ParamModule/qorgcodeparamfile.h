#ifndef QORGCODEPARAMFILE_H
#define QORGCODEPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include "CommonModule/laneutils_new.h"
#pragma pack(push,1)
struct SOrgCodeDataContainer
{
    char szOrgCode[7];
    quint16 wOrgType;
    char szLane[4];
    char szOrgSName[41];
    char szOrgFName[41];
    char szBL_Station[7];
    char szBL_SubCenter[7];
    char szBL_Road[7];
    char szBL_Owner[7];
    char szBL_NetID[7];
    char szBL_Area[7];
    char szBL_ProvID[7];
    quint16 wRegionSN;
    char szIP[21];
    char szUseTime[15];
    char szExpireTime[15];
    char szLastVer[15];
    char szVerUseTime[15];
    char szOldOrgCode[7];
};
#pragma pack(pop)
struct SOrgCodeValue
{
public:
    QString sOrgCode;		//1 机构编码
    quint32 wOrgType;           //2 机构类型
    QString sLane;		//3 车道编码
    QString sOrgSName;		//4 机构简称
    QString sOrgFName;		//5 机构全称
    QString sBL_Station;	//6 所属收费站
    QString sBL_SubCenter;	//7 所属分中心
    QString sBL_Road;		//9 所属路段
    QString sBL_Owner;		//10 所属行政区域
    QString sBL_NetID;		//11 所属小业主
    QString sBL_Area;		//12 所属大业主(公司)
    QString sBL_ProvID;		//12 所属大业主(公司)
    quint32 wRegionSN;          //13 区间顺序号
    QString sIP;		//14 IP地址
    QDateTime sUseTime;		//15 机构启用时间
    QDateTime sExpireTime;	//16 机构注销时间
    QString sLastVer;		//21 最后版本号
    QDateTime sVerUseTime;	//22 版本生效时间
    QString sOldOrgCode;
public:
    quint32 GetRoadBelong()
    {
        return (quint32)atol(sBL_Road.toLocal8Bit().data());
    }
    quint32 GetAdmAreaBelong()
    {
        return (quint32)atol(sBL_Area.toLocal8Bit().data());
    }
    quint32 GetOwnerBelong()
    {
        quint32 wOwner;
        wOwner=(quint32)atol(sBL_Owner.toLocal8Bit().data());
        if (0==wOwner)
        {
            // 如果未设置所属小业主，视为所属分中心即为所属小业主
            wOwner=(quint32)atol(sBL_SubCenter.toLocal8Bit().data());
        }
        return wOwner;
    }
    void Clear()
    {
        sOrgCode = "0";		//1 机构编码
         wOrgType = 0;           //2 机构类型
         sLane = "0";		//3 车道编码
         sOrgSName= "0";		//4 机构简称
         sOrgFName= "0";		//5 机构全称
         sBL_Station= "0";	//6 所属收费站
         sBL_SubCenter= "0";	//7 所属分中心
         sBL_Road= "0";		//9 所属路段
         sBL_Owner= "0";		//10 所属行政区域
         sBL_NetID= "0";		//11 所属小业主
         sBL_Area= "0";		//12 所属大业主(公司)
         sBL_ProvID= "0";		//12 所属大业主(公司)
         wRegionSN = 0;          //13 区间顺序号
         sIP = "0";		//14 IP地址
         sLastVer = "0";		//21 最后版本号
         sOldOrgCode = "0";
    }
};
struct SOrgCodeKey
{
public:
    QString sOrgCode;
    quint32 wOrgType;
public:
    friend bool operator <(const SOrgCodeKey & lp, const SOrgCodeKey & rp);
};
class QOrgCodeParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QOrgCodeParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<SOrgCodeKey,SOrgCodeValue> m_container;
signals:

public slots:
public:
      bool getStaList(QList<quint32> &staList);
      bool getRegionSeqId(int *pnSeqId, const quint32 nOrgCode, const int nOrgType);
      int getSubOrgList(QVector<SOrgCodeValue> &orgList, int nOrgType, const QString &sOrgCode);
      //查询用户输入的编号是否存在
      //参数1：出口参数，如果用户查询值存在，则该参数返回查询到的值
      //参数2：入口参数，用户输入的编号
      //参数3：入口参数，查询的机构类型
      //返回值：是否查询到
      bool queryOrgCodeValue(SOrgCodeValue &orgCodeValue, quint32 nOrgCode, const int nOrgType=ORG_TYPE_STATION);
      ParamMapContainer<SOrgCodeKey, SOrgCodeValue>& GetOrgCodeInfo(){return m_container;}
      bool getCenterOrgList(QVector<SOrgCodeValue>& orgList);
      bool GetOrgName(char* szStaId, QString& sOrgName,quint16 wOrgType=ORG_TYPE_STATION);
      bool getOrgList(quint16 wOrgType, const quint32 dwOrgCode, QVector<SOrgCodeValue> & orgList );
      bool GetNewStaCode(quint32& dwNew, quint32 dwOld);
      bool GetOldStaCode(quint32& dwOld, quint32 dwNew);

};
template<typename T>
inline QString transDecimalToString(T decimal, int nMinLen)
{
    QString s=QString::number(decimal);
    int len=s.length();
    if (len<nMinLen)
    {
        s.prepend(QString("0").repeated(nMinLen-len));
    }
    return s;
}
#endif // QORGCODEPARAMFILE_H
