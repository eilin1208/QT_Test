
#include "qbaserateparamfile.h"
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QBaseRateParamFile::QBaseRateParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

bool operator <(const SBaseRateKey &lp, const SBaseRateKey &rp)
{
    if (lp.sRateCode<rp.sRateCode)
        return true;
    else if (lp.sRateCode > rp.sRateCode)
        return false;

    if (lp.wRateParaType<rp.wRateParaType)
        return true;
    else if (lp.wRateParaType>rp.wRateParaType)
        return false;

    if (lp.wVC<rp.wVC)
        return true;
    else
        return false;
}

void QBaseRateParamFile::InitParamFile()
{
    m_nFileID = 8;
    m_sFileName = getCfgFilePath() + "BaseRate.cfg";
    m_sNewFileName =  getCfgFilePath() + "BaseRate.new";
    m_sDispFileName = tr("基础费率");
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SBaseRateContainer);
}
bool QBaseRateParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SBaseRateKey Key;
    SBaseRateValue Value;
    SBaseRateContainer * tmpContainer = (SBaseRateContainer *)dataBuffer;
    Key.sRateCode=QString::fromLocal8Bit(tmpContainer->szRateCode);
    Key.wRateParaType=qFromBigEndian(tmpContainer->wRateParaType);
    Key.wVC=qFromBigEndian(tmpContainer->wVC);

    Value.sRateCode=Key.sRateCode;
    Value.wRateParaType=Key.wRateParaType;
    Value.wVC=Key.wVC;
    Value.sRateDesc=GB2312toUtf8(tmpContainer->szRateDesc);
    Value.wVCRate=qFromBigEndian(tmpContainer->wVCRate);
    Value.wWBaseRate = qFromBigEndian(tmpContainer->wWBaseRate);
    Value.wAMinParam= qFromBigEndian(tmpContainer->wAMinParam);
    Value.wWAMaxParam=qFromBigEndian(tmpContainer->wWAMaxParam);
    Value.wWMMinParam=qFromBigEndian(tmpContainer->wWMMinParam);
    Value.sLastVer=tmpContainer->szLastVer;
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tmpContainer->szVerUseTime),"yyyyMMddhhmmss");
        ////qDebug() << Key.sRateCode << Key.wRateParaType << Key.wVC << ":" << Value.sRateCode << Value.wRateParaType
         //<< Value.wVC << Value.sRateDesc << Value.wVCRate <<Value.wWBaseRate<<Value.wAMinParam<<Value.wWAMaxParam
         //<<Value.wWMMinParam<< Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool QBaseRateParamFile::queryBaseRate(SBaseRateValue &BaseRate, const QString &sRateCode, int nRateType, int nVehClass)
{
    SBaseRateKey key;
    key.sRateCode = sRateCode;
    key.wRateParaType = nRateType;
    if ((SBaseRateValue::ROUTE_VEH_CLASS==nRateType) ||
            (SBaseRateValue::POINT_VEH_CLASS==nRateType))
    {
        key.wVC = nVehClass;
    }
    else
    {
        //key.wVC = VC_Truck;WXK==根据实际情况，计重计算这里应赋值为0
        key.wVC = 0;

    }
    return m_container.findValue(key, BaseRate);
}


