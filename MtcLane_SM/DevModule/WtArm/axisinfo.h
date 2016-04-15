#ifndef AXISINFO_H
#define AXISINFO_H

#include <QObject>
#include <QList>
#include <QMutexLocker>
#include <QMutex>
class AxisInfo : public QObject
{
    Q_OBJECT
public:
    explicit AxisInfo(QObject *parent = 0);

signals:

public slots:

};
class CSingleAxisData
{
public:
    quint32 m_nWeightRaw;
    quint32 m_nWeight;
public:
    void operator =(const CSingleAxisData &Data2)
    {
        this->m_nWeightRaw = Data2.m_nWeightRaw;
        this->m_nWeight = Data2.m_nWeight;
    }
};
inline bool operator ==(const CSingleAxisData &Data1,const CSingleAxisData Data2)
{
    return (Data1.m_nWeight == Data2.m_nWeight) && (Data1.m_nWeightRaw == Data2.m_nWeightRaw);
}
class CAxisGroupData
{
public:
    int m_nAxisGroupType;
    quint32 m_nAxisGroupWeightRaw;
    quint32 m_nAxisGroupweight;
    double m_fSpeed;
    int m_nAxisNum;
    QList<CSingleAxisData> m_AxisList;
public:
    CAxisGroupData()
    {
        clear();
    }
    CAxisGroupData(int nAxisGroupType,double fSpeed,const long AxisWeightArry[],int nMaxAxisWeightArryLen);
    void clear()
    {
        m_nAxisGroupType = 0;
        m_nAxisGroupWeightRaw = 0;
        m_nAxisGroupweight = 0;
        m_fSpeed = 0;
        m_AxisList.clear();
    }
    void operator =(const CAxisGroupData &AxisData);
    bool operator ==(const CAxisGroupData &AxisData);
    bool operator !=(const CAxisGroupData &AxisData);
};
bool operator ==(const CAxisGroupData &AxisData1,const CAxisGroupData &AxisData2);
class CVehAxisInfo
{
protected:
    bool WeightModifyStatus;
    bool WeightStandardStatus;
    bool WeightCut;
    double m_fVehSpeed;
    int m_nRawTotalWeight;
    int m_dwTotalWeight;
    quint32 m_dwInputedWeightLimit;
    QList<CAxisGroupData> m_RawAxisList;
    QList<CAxisGroupData> m_AutoAxisList;
    static quint32 GetAxisGroupOf(const QList<CAxisGroupData> &AxisList);
    static quint32 GetTotalWeightOf(const QList<CAxisGroupData> &AxisList);
    static int GetAxisCount(const QList<CAxisGroupData> &AxisList);
    static double GetAverageVehSpeed(const QList<CAxisGroupData> &AxisList);
    static void GetAxisInfoStr(char* szAxisInfo,size_t nSizeOfAxisInfo,const QList<CAxisGroupData> &AxisList);
    void GetAxisGroupList(QList<CAxisGroupData> &AxisGroupList) const
    {
        AxisGroupList = m_RawAxisList;
    }
public:
    CVehAxisInfo()
    {
        //Clear();
    }
    void operator =(const CVehAxisInfo &VehAxisInfo);
    bool IsNullVeh() const
    {
        return m_RawAxisList.size() == 0;
    }
    bool IsSplitted() const
    {
        return !WeightCut;
    }
    quint32 GetRawAxisGroup()const
    {
        return GetAxisGroupOf(m_RawAxisList);
    }
    double GetVehSpeed() const
    {
        return m_fVehSpeed;
    }
    int GetAxisCount() const
    {
        return GetAxisCount(m_RawAxisList);
    }
    int GetAutoAxisCount() const
    {
        return GetAxisCount(m_AutoAxisList);
    }
    int GetAxisGroupCount()const
    {
        return m_RawAxisList.size();
    }
    int GetAutoAxisGroupCount() const
    {
        return m_AutoAxisList.size();
    }
    quint32 GetTotalWeight()const
    {
        return m_dwTotalWeight;
    }
    quint32 GetAutoTotalWeight() const;
    quint32 GetWeightStandard() const;
    quint32 GetOverWeightPer() const;
    quint32 GetMinAxisWeight() const;
    void GetRawAxisList(QList<CAxisGroupData> *pList) const
    {
        *pList = m_RawAxisList;
    }
    bool operator ==(const CVehAxisInfo &VehAxisInfo) const
    {
        return this->m_RawAxisList == VehAxisInfo.m_RawAxisList;
    }
    void GetRawAxisInfoStr(char* szAxisInfo,size_t nSizeOfAxisInfo) const
    {
        GetAxisInfoStr(szAxisInfo,nSizeOfAxisInfo,m_RawAxisList);
    }
public:
    bool CanSplit() const;
public:
    void Clear();
    void SetAxisGroupList(const QList<CAxisGroupData> &AxisGroupList,double fVehSpeed);
    void ModifyAxisGroupList(const QList<CAxisGroupData> &AxisGroup);
    bool Split(CVehAxisInfo *pVeh1,CVehAxisInfo *pVeh2,int nFirstAxisGroupNum) const;
    void CombineVeh(const CVehAxisInfo &VehAxisInfo);
    void ModifyVehInfo(long nTotalWeight,long nWeightLimit);
    static const CVehAxisInfo &MakeVehInfo(long nTotalWeight,long nWeightLimit)
    {
        static CVehAxisInfo VehInfo;
        VehInfo.m_dwTotalWeight = nTotalWeight;
        VehInfo.m_dwInputedWeightLimit = nWeightLimit;
        VehInfo.WeightModifyStatus = false;
        VehInfo.WeightStandardStatus = false;
        return VehInfo;
    }
};
class CLaneAxisInfo
{
protected:
    static CLaneAxisInfo m_LaneAxisInfo;
    QList<CVehAxisInfo> m_VehList;
    int m_nWeightDeviceStatus;
    QMutex mutex;
public:
    CLaneAxisInfo(){}
    ~CLaneAxisInfo(){}
  public:
    static CLaneAxisInfo *GetLaneAxisInfo()
    {
        return &m_LaneAxisInfo;
    }
    int GetWeightDevStatus()
    {
        return m_nWeightDeviceStatus;
    }
    void SetWeightDevStatus(int nDevStatus)
    {
        m_nWeightDeviceStatus = nDevStatus;
    }
    void CopyVehList(const QList<CVehAxisInfo> &VehList);
    int GetVehCount();
    bool GetFirstVeh(CVehAxisInfo *pVehAxisInfo);
    void GetAllVeh(QList<CVehAxisInfo> *pVehAxisInfoList);
    bool IsFirstNullVeh();
    bool CanSplit();
    bool CanCombine();
    bool CanInsert();
    void AddVeh(const CVehAxisInfo &VehInfo,bool bPustFront = false);
    void AddVeh(long nWeight,long nWeightLimit);
    void ModifyVeh(const CVehAxisInfo &VehInfo);
    bool RemoveAll(QList<CVehAxisInfo> *pVehList = NULL);
    bool  RemoveFirst(CVehAxisInfo *pFirstVeh = NULL);
    bool CombineFirst2Veh(CVehAxisInfo *pVeh1 = NULL,CVehAxisInfo *pVeh2 = NULL);
    bool SplitVeh(int dwFirstAxisGroup,CVehAxisInfo *pSplittedVeh = NULL);
};
#endif // AXISINFO_H
