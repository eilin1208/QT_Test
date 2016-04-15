#include "qsysparadicparamfile.h"
#include <QtEndian>
#include "CommonModule/laneutils_new.h"
#include <QDebug>
#include "CommonModule/ilogmsg.h"
QSysParaDicParamFile::QSysParaDicParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QSysParaDicParamFile::InitParamFile()
{
    m_nFileID = 20;
    //m_nCfgFileType = cfOper;
    m_sFileName = getCfgFilePath() + "SysParaDic.cfg";
    //m_sFileName = "SysParaDic.cfg";
    m_sNewFileName = "SysParaDic.new";
    m_sDispFileName = "系统参数字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SSysParaDicDataContainer);
}

bool QSysParaDicParamFile::LoadParamFile(QString sFileName, SParamFileHead &cfgFileHead, bool &isNew)
{
    bool ret = QBaseParamFile::LoadParamFile(sFileName, cfgFileHead, isNew);
    ParseParams();
    return ret;
}
bool QSysParaDicParamFile::loadParam_addRow(const char *dataBuffer)
{
    SSysParaDicDataContainer * tmpDataContainer = (SSysParaDicDataContainer*)dataBuffer;
    quint16 Key;
    QString Value;
    Key = qFromBigEndian(tmpDataContainer->wPara);
    Value = GB2312toUtf8(tmpDataContainer->szValue);
    m_container.addItemToTemp(Key, Value);
    // 测试
    ////qDebug() << Key << ":" << Value;
    return true;
}
enum SYSFILEID
{
    sfStationID=1,          //	01	站代码
    sfStationName,          //	02	站名称
    sfBLSubcenter,          //	03	所属分中心
    sfBLCenter,             //	04	所属中心
    sfStationIP,            //	05	收费站服务器IP
    sfVLPChecked,
    sfVLPFull,
    sfStationPort,          //	06	业务处理服务监听端口号
    sfCenderIP,             //	07	中心IP地址
    sfCenterPort,           //	08	中心服务器监听端口号
    sfBadcrdQryIP,          //	09	坏卡查询服务器IP
    sfBadcrdQryPort,        //	10	坏卡查询监听端口
    sfCardNoQryTime,        //	11	按卡号查询超时时间（毫秒）
    sfPlateQryTime,         //	12	按车牌号查询超时时间（毫秒）
    sfKeyQryTime,           //	13	按关键字查询超时时间（毫秒）
    sfVoiceNoLen,           //	14	票号长度
    sfMinVoiceNum,          //	15	发票报警数
    sfPaperNoLen,           //	16	纸卡卡号长度
    sfInputPaperNo,         //	17	出口是否必须输入纸卡卡号
    sfOldDataSaveDays,      //	18	原始业务保存天数
    sfCardCost,             //	19	卡成本，参数文件内保存为元，加载时转为分
    sfAdjusttimeVal,        //	20	校时时间间隔（分钟）
    sfMinBalance,           //	21	余额报警数（分）
    sfVLPLen,               //	22	车牌输入长度
    sfDeftVLP,              //	23	本站默认车牌
    sfBakDataSaveDays,      //	24	备份库数据保存天数
    sfMemoryDataSaveCounts, //	25	内存数据保存条数
    sfCardBoxMin,
    sfCardBoxMax,
    sfend
};

bool QSysParaDicParamFile::ParseParams()
{
    bool ok = false;
    // 站ID
    m_info.m_nStationID = m_container.getValue(sfStationID).toInt(&ok);
    if(ok == false || m_info.m_nStationID <= 0 || m_info.m_nStationID > 999999)
    {
        return false;
    }
    // 站名称
    m_info.m_sStationName = m_container.getValue(sfStationName);
    if(m_info.m_sStationName.count() == 0)
    {
        return false;
    }

    // 子中心
    m_info.m_nBLSubCenter = m_container.getValue(sfBLSubcenter).toLong(&ok);
    if(ok == false)
        return false;

    // 中心
    m_info.m_nBLCenter = m_container.getValue(sfBLCenter).toLong(&ok);
    if(ok == false)
        return false;

    // IP
    m_info.m_sStationIP = m_container.getValue(sfStationIP);
    if(m_info.m_sStationIP.isEmpty() || !IsValidIP(m_info.m_sStationIP.toLocal8Bit().data()))
    {
        LogMsg("lane", "station ip is invalid!");
        return false;
    }
    //liuj
    m_info.m_bVLPCheck = '1';
    m_info.m_bVLPFull = '1';
    //end
    // 中心IP
    m_info.m_sCenterIP = m_container.getValue(sfCenderIP);
    if(m_info.m_sCenterIP.isEmpty() || !IsValidIP(m_info.m_sCenterIP.toLocal8Bit().data()))
    {
       LogMsg("lane", "center ip is invalid!");
        return false;
    }
    // 中心端口号
    m_info.m_dwCenterPort = m_container.getValue(sfCenterPort).toLong(&ok);


    // 坏卡查询服务器IP
    m_info.m_sBadCardQryIP = m_container.getValue(sfBadcrdQryIP);
    if(m_info.m_sBadCardQryIP.isEmpty() || !IsValidIP(m_info.m_sBadCardQryIP.toLocal8Bit().data()))
    {
        LogMsg("lane", "bad card server ip is invalid!");
        return false;
    }
    // 坏卡端口号
    m_info.m_dwBadCardQryPort = m_container.getValue(sfBadcrdQryPort).toLong(&ok);

    // 按卡号查询的 超时时间
    m_info.m_dwCardNoQryTime = m_container.getValue(sfCardNoQryTime).toLong(&ok);


    // 按车牌查询的 超时时间
    m_info.m_dwPlateQryTime = m_container.getValue(sfPlateQryTime).toLong(&ok);

    // 按关键字查询的 超时时间
    m_info.m_dwKeyQryTime = m_container.getValue(sfKeyQryTime).toLong(&ok);

    // 票号长度
    m_info.m_nVoiceNoLen = m_container.getValue(sfVoiceNoLen).toLong(&ok);

    // 发票报警数
    m_info.m_nMinVoiceNum = m_container.getValue(sfMinVoiceNum).toLong(&ok);
    if(ok == false)
        return false;

    // 纸卡卡号长度
    m_info.m_nPaperNoLen = m_container.getValue(sfPaperNoLen).toLong(&ok);
    if(ok == false)
        return false;

    // 出口是否必须输入纸卡卡号
    m_info.m_bInputPaperNo = m_container.getValue(sfInputPaperNo).toLong(&ok);
    if(ok == false)
        return false;

    // 原始业务保存天数
    m_info.m_nOldDataSaveDays = m_container.getValue(sfOldDataSaveDays).toLong(&ok);
    if(ok == false)
        return false;
    //卡成本，参数文件内保存为元，加载时转为分
    m_info.m_nCardCost = m_container.getValue(sfCardCost).toLong(&ok) * 100;
    if(ok == false)
        return false;

    //校时时间间隔（分钟）
    m_info.m_nAdjustTimeVal = m_container.getValue(sfAdjusttimeVal).toLong(&ok);
    if(ok == false)
        return false;

    //余额报警数（分）
    m_info.m_nMinVoiceNum = m_container.getValue(sfMinBalance).toLong(&ok);
    if(ok == false)
        return false;

    //车牌输入长度
    m_info.m_nVLPLen = m_container.getValue(sfVLPLen).toLong(&ok);
    if(ok == false)
        return false;
   LogMsg("lane", tr("in the param, the card enter number is %1").arg(m_info.m_nVLPLen));
    //本站默认车牌
    m_info.m_sDeftVLP = m_container.getValue(sfDeftVLP);

    //备份库数据保存天数
    m_info.m_nBakDataSaveDays = m_container.getValue(sfBakDataSaveDays).toLong(&ok);
    if(ok == false)
        return false;

    //内存数据保存条数
    m_info.m_nMemoryDataSaveCounts = m_container.getValue(sfMemoryDataSaveCounts).toLong(&ok);

    m_info.m_nCardBoxMin = m_container.getValue(sfCardBoxMin).toInt(&ok);
    m_info.m_nCardBoxMax = m_container.getValue(sfCardBoxMax).toInt(&ok);
    return true;
}
