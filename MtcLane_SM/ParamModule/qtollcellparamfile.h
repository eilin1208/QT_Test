#ifndef QTOLLCELLPARAMFILE_H
#define QTOLLCELLPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
#include "qallrateparamfile.h"
typedef SAllRateKey SNeighborSta;
#pragma pack(push,1)
struct STollCellRowContainer
{
    char szCellCode[4];
    char szCellFlag[2];
    char szStation1[7];
    char szStation2[7];
    quint16 wStationSN;
    char szBL_Road[4];
    char szBL_Country[4];
    char szOwner[4];
    char szWayFlay[2];
    char szSpFreeFlag[17];
    char szRateCode[3];
    quint32 dwRealDistance;
    quint32 dwTollDistance;
    quint16 wLimitSpeedPara;
    char szLastVer[15];
    char szUseTime[15];
};
#pragma pack(pop)
struct STollCellValue
{
public:
    enum TOLL_CELL_TYPE
    {
        ROUTE='1',          //	'1'-路
        POINT='2'           //	'2'-桥遂
    };
public:
    quint16 wCellCode;          // 单元编码
    char cCellFlag;             // 单元标志 '1'-路 '2'-桥遂
    quint32 dwStation1;         // 起点收费站
    quint32 dwStation2;         // 终点收费站
    quint16 wStationSN;         // 所处相邻站顺序号
    quint16 wBL_Road;           // 所属路段 2位
    quint16 wBL_Country;        // 所属行政区域 4位
    quint16 wOwner;             // 拆分业主
    QString sWayFlag;           // 方向标志
    QString sSpFreeFlag;        // 特殊免费标志 按位存放支持16位，第1位'0'-不优惠 '1'-免费(绿通)其他预留
    QString sRateCode;          // 费率编号
    quint32 dwRealDistance;     // 实际里程(单位：米)
    quint32 dwTollDistance;     // 计费里程(单位：米)
    quint16 wLimitSpeedPara;    // 超时系数,千分数 1000表示按标准计算 500表示按标准一半计算
    QString sLastVer;           // 最后版本号
    QDateTime sUseTime;         // 版本生效时间
};
struct STollCellKey
{
    quint16 wCellCode;
    quint32 dwStation1;
    quint32 dwStation2;
public:
    friend bool operator <(const STollCellKey & a,const STollCellKey & b);
};

class QTollCellParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QTollCellParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    bool queryCellListByNeighborSta(QList<quint16> &cellList, quint32 nSta1, quint32 nSta2);
    bool queryTollCellValue(STollCellValue &TollCellValue, int nTollCell);
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
     void addRow_neighborSta(const STollCellKey &Key);
    ParamMapContainer<STollCellKey,STollCellValue> m_container;
    ParamMapContainer<SNeighborSta, quint16> m_container_neighborSta;
    ParamMapContainer<quint16, STollCellValue> m_container_cellID;
    virtual bool loadParam_begin()
    {
        m_container_neighborSta.load_begin();
        m_container_cellID.load_begin();
       return m_container.load_begin();
    }
    virtual bool loadParam_cancel()
    {
        m_container_neighborSta.load_cancel();
        m_container_cellID.load_cancel();
        return m_container.load_cancel();
    }
    virtual bool loadParam_complete()
    {
        m_container_neighborSta.load_complete();
        m_container_cellID.load_complete();
        return m_container.load_complete();
    }
signals:

public slots:

};

#endif // QTOLLCELLPARAMFILE_H
