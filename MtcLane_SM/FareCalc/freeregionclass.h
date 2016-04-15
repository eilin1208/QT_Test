#ifndef FREEREGIONCLASS_H
#define FREEREGIONCLASS_H

#include <QtGlobal>
#include <QList>
#include <CommonModule/LaneType.h>

// 免费区间信息
class FreeRegionClass
{
public:
    // 鲁通卡免费区间类型定义
    enum FREE_REGION_TYPE
    {
        frNone=0,
        frRoadOwner=1,              //	1-管理处
        frAdmRegion=2,              //	2-行政区域
        frNeighborSta=3,            //	3-相邻站
        frNeighborStaPlus=4         //	4-相邻站+入出口
    };
    // 鲁通卡免费区间  YN(wumax)
    int m_nFreeRegionType;          //	免费区间类型
    quint8 m_FreeRegion[80];          //	免费区间（按位存储）
    // 公务卡类型定义
    enum GONGWUKA_TYPE
    {
        gtNone=0,                   //	收费
        gtQuanLuWang=1,             //	全路网有效
        gtBuFenLuDuanGuaZhang=2,    //	非有效区间通行费挂帐（在计算应收金额时，视同全路网免费）
        gtBuFenLuDuanJiaoFei=3      //	非有效区间交通行费
    };
    // 老公务卡的免费区间
    quint8 m_bGongWuKaType;         //	公务卡类型
    QList<quint32> m_GongWuKaQuJian;
    ///////YN(wumax)
public:
    // 构造函数
    FreeRegionClass()
    {
        clear();
    }
    // 清零  ///////YN(wumax)
    void clear()
    {
        m_nFreeRegionType=frNone;
        memset(m_FreeRegion, 0, sizeof m_FreeRegion);
        m_bGongWuKaType=gtNone;
        m_GongWuKaQuJian.clear();
    }
    // 赋值 ///////YN(wumax)
    void operator =(const FreeRegionClass &b)
    {
        m_nFreeRegionType=b.m_nFreeRegionType;
        memcpy(m_FreeRegion, b.m_FreeRegion, sizeof m_FreeRegion);
        m_bGongWuKaType=b.m_bGongWuKaType;
        m_GongWuKaQuJian=b.m_GongWuKaQuJian;
    }
    // 设置鲁通卡免费区间 ///////YN(wumax)
    void SetFreeRegion(int nFreeRegionType, const quint8 *pbFreeRegion, quint8 bFreeRegionBytes);
    // 设置老公务卡免费区间
    void SetGongWuKaQuJian(quint8 bGongWuKaType, const quint32 *pdwYouXiaoQuJian, quint8 bYouXiaoQuJianNum);
    // 测试是否为免费区间 ///////YN(wumax)
    bool IsFreeRegion(bool *pbHasError, quint32 dwRoadOwner, quint32 dwAdmRegion, quint32 dwSta1, quint32 dwSta2) const;

    //将区间免费类型转换为免费类型 wumax ==
   // CFreeType GetFreeTypeByFreeRegionType() const;


protected:
    // 标记公务卡某一区间（分中心或中心）
    void MarkGongWuKaFreeRegion(quint32 nQuJian);
    // 将某一顺序号置为免费区间
    void MarkSeqIdAsFreeRegion(int nSeqId);
protected:
    bool IsGongwukaQujian(quint32 dwRoadOwner) const;   ///////YN(wumax)
    bool IsFreeRegion_RoadOwner(bool *pbHasError, quint32 dwRoadOwner) const;
    bool IsFreeRegion_AdmRegion(bool *pbHasError, quint32 dwAdmRegion) const;  ///////YN(wumax)
    bool IsFreeRegion_Sta(bool *pbHasError, quint32 dwSta1, quint32 dwSta2) const;  ///////YN(wumax)
    bool IsFreeRegion_Sta(bool *pbHasError, quint32 dwSta) const;  ///////YN(wumax)
    bool IsFreeRegion(int nSeqId) const;  ///////YN(wumax)

};
    /////////////////

#endif // FREEREGIONCLASS_H
