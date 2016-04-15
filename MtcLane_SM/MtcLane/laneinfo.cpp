#include "laneinfo.h"
#include <QFile>
#include <QSettings>
#include <QDebug>
#include "ParamModule/qparammgr.h"
#include "shiftsum.h"
#include "lanectrl.h"
LaneInfo::LaneInfo(QObject *parent) :
    QObject(parent)
{
    m_nCardCount = 0;
    m_nVehCount = 0;
    m_nPlateColor = 0;
    m_sAppVer = APP_VERSION;
    m_bSubPay = false;
    m_PicPath = "/Program_Data/";
    m_bShutDown = false;
    m_nCloseType = -1;
    m_nAllowPass = 0;
    m_controlDevType = 0;
    m_shiftState = lsNone;
}

bool LaneInfo::loadDataFromIni()
{
    //首先移除现有的lane.ini
    const QString sFileName = "./Lane.ini";
    QFile g_sFileName(sFileName);
    g_sFileName.remove();

    //复制备份文件
    QFile bakFile("./Lane.bak");
    if(!bakFile.exists())
    {
        return false;
    }
    if(!bakFile.copy(sFileName))
    {
        return false;
    }
    if(!g_sFileName.exists())
    {
        return false;
    }
    if(!QFile::exists(sFileName))
    {
        return false;
    }
    //    QFile::flush();
    QSettings settings(sFileName, QSettings::IniFormat);
    QString sGroupName = "LaneInfo";
    setLaneType(settings.value(sGroupName + "/LaneType").toInt());
    setLaneCode(settings.value(sGroupName + "/LaneCode").toInt());
    setLapTop(settings.value(sGroupName+ "/Laptop").toInt());

    sGroupName = "StationInfo";
    setDataSrvIp(settings.value(sGroupName + "/DataSrvIP").toString());
    setDataSrvPort(settings.value(sGroupName + "/DataSrvPort").toUInt());
    setMonSrvIp(settings.value(sGroupName + "/MonSrvIP").toString());
    setMonSrvPort(settings.value(sGroupName + "/MonSrvPort").toUInt());
    setPicSaveDay(settings.value(sGroupName + "/PicSaveDay").toInt());
    setXmlSaveDay(settings.value(sGroupName + "/XmlSaveDay").toInt());
    setSqlDay(settings.value(sGroupName + "/SqlSaveDay").toInt());
    setDiskSizeLimit(settings.value(sGroupName + "/DiskLimitSize").toInt());

    sGroupName = "LaneDBSrv";
    setLaneDBSrvIp(settings.value(sGroupName + "/LaneDBSrvIP").toString());
    setLaneDBSrvDataBase(settings.value(sGroupName + "/LaneDBSrvDataBase").toString());
    setLaneDBSrvUser(settings.value(sGroupName + "/LaneDBSrvUser").toString());
    setLaneDBSrvPW(settings.value(sGroupName + "/LaneDBSrvPW").toString());
    setYWDataName(settings.value(sGroupName + "/YWDataName").toString());
    setYWDataBackName(settings.value(sGroupName + "/YWDataBackName").toString());
    setXMLPath(settings.value(sGroupName + "/XMLPath").toString());

    sGroupName = "VPR";
    setManualCapture(settings.value(sGroupName + "/ManualCapture").toBool());

    //获取使用IO还是费显进行控制通行灯、报警器
    m_controlDevType = settings.value("Other/ControlDev", 0).toInt();
    if(!QFile::exists("./Setting"))
    {
        return false;
    }
//    QSettings setupFile("./Setting.ini", QSettings::IniFormat);
//    sGroupName = "Setup";
//    setUseSimulate(setupFile.value(sGroupName + "/UseSimulate").toInt());
//    setUseFrontLoop(setupFile.value(sGroupName + "/UseFrontLoop").toInt());
//    setUsePlate(setupFile.value(sGroupName + "/UsePlate").toInt());
//    setUseWeight(setupFile.value(sGroupName + "/UseWeight").toInt());
//    setIsUseSpFree(setupFile.value(sGroupName + "/IsUseSpFree").toInt());
    QFile file("./Setting");
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QString strValue = file.readAll();
    file.close();
    if(strValue.length() != 4)
    {
        return false;
    }
    int numValue = strValue.toInt();
    m_nUseSimulate = (numValue % 10);
    m_nUseFrontLoop = ((numValue / 10) % 10);
    m_nUsePlate = ((numValue / 100) % 10);
    m_nUseWeight = ((numValue / 1000) % 10);

    return true;
}

LaneInfo* getLaneInfo()
{
    static LaneInfo info;
    return &info;
}

bool LaneInfo::isMtcLane()
{
    return (getLaneType() == 1 || getLaneType() == 2);
}

bool LaneInfo::isEntryLane()
{
    return getLaneType() %2 == 1;
}

bool LaneInfo::isAutoLane()
{
    return getLaneType() == LaneType_AutoCard;
}

int LaneInfo::getCloseType()
{
    return m_nCloseType;
}

void LaneInfo::setCloseType(int nType)
{
    m_nCloseType = nType;
}
bool LaneInfo::getDiskSize(int &ntotal, int &navail)
{
#ifdef Q_OS_UNIX
    struct statfs buf;
    //"/表示挂载点"
    int ret = statfs("/",&buf);
    //    //总
    //    //qDebug()<<"!!!"<<(long long)buf.f_bsize*(long long)buf.f_blocks/(long long)1024<<ret;
    //    //剩余
    //    //qDebug()<<"!!!"<<(long long)buf.f_bsize*(long long)buf.f_bfree/(long long)1024<<ret;
    //    //可用
    //    //qDebug()<<"!!!"<<(long long)buf.f_bsize*(long long)buf.f_bavail/(long long)1024<<ret;
    if(ret == 0)
    {
        ntotal = (long long)buf.f_bsize*(long long)buf.f_blocks/((long long)(1024*1024));
        navail = (long long)buf.f_bsize*(long long)buf.f_bavail/((long long)(1024*1024));
        if(navail <= m_nDiskLimitSize)
        {
            ////qDebug()<<navail<<":"<<m_nDiskLimitSize;
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        ntotal = -1;
        navail = -1;
        return true;
    }
#else
    long long freeBytesAvail=0,totalBytes=0,freeBytes=0;
    bool b;
    b=GetDiskFreeSpaceEx((LPCWSTR)QString("/").utf16(),(PULARGE_INTEGER)&freeBytesAvail,
                         (PULARGE_INTEGER)&totalBytes,(PULARGE_INTEGER)&freeBytes);
    if(b)
    {
        ntotal = totalBytes/(1024*1024);
        navail = freeBytesAvail/(1024*1024);
        if(navail <= m_nDiskLimitSize)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        ntotal = -1;
        navail = -1;
        return true;
    }
#endif
}

quint8 LaneInfo::GetTransID()
{
    if(255 == m_nTransId)
    {
        m_nTransId = 0;
    }
    else
    {
        m_nTransId++;
    }
    //wumax
    return m_nTransId;
}

quint8 LaneInfo::ReadTransID()
{
    return m_nTransId;
}

quint32 LaneInfo::GetAppVer()
{
    return m_sAppVer.remove(QChar('.')).toUInt();
}

QString LaneInfo::GetAppVerStr()
{
    return m_sAppVer;
}

void LaneInfo::setUseFrontLoop(int nUseFrontLoop)
{
    m_nUseFrontLoop = nUseFrontLoop;
    QFile file("Setting");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadWrite))
        {
            QByteArray data = file.readAll();
            file.seek(0);
            data.replace(1, 1, QString::number(nUseFrontLoop).toLocal8Bit().data());
            file.write(data);
            file.flush();
            file.close();
#ifndef Q_OS_WIN
    QProcess::execute("sync");
#endif
        }
    }
}
void LaneInfo::setUseSimulate(int nUseSimulate)
{
    m_nUseSimulate = nUseSimulate;
    QFile file("Setting");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadWrite))
        {
            QByteArray data = file.readAll();
            file.seek(0);
            data.replace(3, 1, QString::number(nUseSimulate).toLocal8Bit().data());
            file.write(data);
            file.flush();
            file.close();
#ifndef Q_OS_WIN
    QProcess::execute("sync");
#endif
        }
    }
}
void LaneInfo::setUsePlate(int nUsePlate)
{
    m_nUsePlate = nUsePlate;
    QFile file("Setting");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadWrite))
        {
            QByteArray data = file.readAll();
            file.seek(0);
            data.replace(2, 1, QString::number(nUsePlate).toLocal8Bit().data());
            file.write(data);
            file.flush();
            file.close();
#ifndef Q_OS_WIN
    QProcess::execute("sync");
#endif
        }
    }
}
void LaneInfo::clearCurVehInfo()
{
    //车型
    m_nVehClass = 0;
    //车牌
    m_sVehPlate.clear();
    //车牌颜色
    m_nPlateColor = 0;
    //车类
    m_nVehType = 0;
    //应收金额
    m_dMoney = 0;
    //卡号
    m_sCardNum = "";
    //优惠金额
    m_dFavorMoney = 0;
    //其他金额
    m_dOtherMoney = 0;
    //系统版本号
    //m_sAppVer = "";
    //车辆入口信息
    m_vehEntryInfo.Clear();
    //应收通行费
    m_tollMoneyInfo.clear();
    //本次实收通行费
    m_lastMoneyInfo.clear();
    //收费信息
    m_CardTollInfo.clear();
    //通行卡基本信息
    m_tollCardBasicInfo.bCRC = false;
    m_tollCardBasicInfo.bOldCard = false;
    m_tollCardBasicInfo.M1BasicInfo.clear();
    //运通卡交易信息
    m_proCardConsumeInfo.clear();
    m_bSubPay = false;
    m_nSpCard = 0;
}

void LaneInfo::UpdateShiftDateWhenUnlogin()
{
    if(getShiftState() == lsUnlogin)
    {
        QShiftParamFile* pShift = (QShiftParamFile*)getParamMgr()->GetParamFile(cfShift);
        if(pShift == NULL)
        {
            return;
        }
        QList<SShiftValue> usableShift;
        pShift->getShiftInfoByTime(QDateTime::currentDateTime(), usableShift);
        if(usableShift.isEmpty())
        {
            return;
        }
        setShiftInfo(usableShift.first());
        if(getShiftSum()->LessThan(usableShift.first().wShift, usableShift.first().iWorkDate))
        {
            bool bRet = !getShiftSum()->IsBigShiftSame(usableShift.first().wShift,
                                                       usableShift.first().iWorkDate);
            if(bRet)
            {
                getLaneCtrl()->getTollItem()->NotifyWorkShiftChange();
            }
        }
    }
}
