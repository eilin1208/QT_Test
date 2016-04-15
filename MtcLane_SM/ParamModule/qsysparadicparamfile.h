#ifndef QSYSPARADICPARAMFILE_H
#define QSYSPARADICPARAMFILE_H

#include <QObject>
#include "qbaseparamfile.h"
//20 系统参数字典
class QSysParamInfo
{
public:
    int m_nStationID ;			//	站代码
    QString m_sStationName;		//	站名称
    int  m_nBLSubCenter;		//	所属分中心
    int  m_nBLCenter;			//	所属中心
    QString m_sStationIP;		//	收费站服务器IP
    bool  m_bVLPCheck;
    bool  m_bVLPFull;
    QString m_sCenterIP;
    long m_dwCenterPort;		//	中心服务器监听端口号
    QString m_sBadCardQryIP;		//	坏卡查询服务器IP
    long m_dwBadCardQryPort;            //	坏卡查询监听端口
    long m_dwCardNoQryTime;		//	按卡号查询超时时间（毫秒）
    long m_dwPlateQryTime;		//	按车牌号查询超时时间（毫秒）
    long m_dwKeyQryTime;		//	按关键字查询超时时间（毫秒）
    int m_nVoiceNoLen;			//	票号长度
    int m_nMinVoiceNum;			//	发票报警数
    int m_nPaperNoLen;			//	纸卡卡号长度
    bool m_bInputPaperNo;		//	出口是否必须输入纸卡卡号，0-可不输，1-必须输入
    int m_nOldDataSaveDays;		//	原始业务保存天数
    long m_nCardCost;			//	卡成本，参数文件内保存为元，加载时转为分
    int m_nAdjustTimeVal;		//	校时时间间隔 单位（分钟）
    quint32 m_dwMinBalance;		//	余额报警数（分）
    int m_nVLPLen;			//	车牌输入长度
    QString m_sDeftVLP;			//	本站默认车牌
    int m_nBakDataSaveDays;		//	备份库数据保存天数
    int m_nMemoryDataSaveCounts;        //	内存数据保存条数
    int m_nCardBoxMin;
    int m_nCardBoxMax;
public:
    QSysParamInfo()
    {
        SetDefault();
    }
    void SetDefault()
    {
        m_nStationID=0;
        m_sStationName.clear();
        m_nBLSubCenter=0;
        m_nBLCenter=0;
        m_sStationIP.clear();
        m_sCenterIP.clear();
        m_dwCenterPort=40008;
        m_sBadCardQryIP.clear();
        m_dwBadCardQryPort=40009;
        m_dwCardNoQryTime=15000;
        m_dwPlateQryTime=15000;
        m_dwKeyQryTime=15000;
        m_nVoiceNoLen=12;
        m_nMinVoiceNum=20;
        m_nPaperNoLen=7;
        m_bInputPaperNo=true;
        m_nOldDataSaveDays=90;
        m_nCardCost=3000;
        m_nAdjustTimeVal=30;
        m_dwMinBalance=2000;
        m_nVLPLen=7;
        m_sDeftVLP.clear();
        m_nBakDataSaveDays=30;
        m_nMemoryDataSaveCounts=100;
        //m_nCardBoxMin =
        //m_nCardBoxMax
    }
};
#pragma pack(push,1)
struct SSysParaDicDataContainer
{
    quint16 wPara;
    quint16 wBL_OrgType;
    quint16 wBL_SoftType;
    char szValue[65];
};
#pragma pack(pop)

struct SSysParaDicValue
{

};
class QSysParaDicParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QSysParaDicParamFile(QObject *parent = 0);
    virtual void InitParamFile();
    QSysParamInfo GetSysParamInfo(){return m_info;}
    virtual bool LoadParamFile(QString sFileName, SParamFileHead &cfgFileHead, bool &isNew);
protected:
    virtual bool loadParam_addRow(const char * dataBuffer);
    ParamMapContainer<quint16,QString> m_container;
signals:

public slots:
private:
    bool ParseParams();
    QSysParamInfo m_info;
};

#endif // QSYSPARADICPARAMFILE_H
