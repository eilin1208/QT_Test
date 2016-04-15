
#include "qvccodeparamfile.h"
#include <QtEndian>
#include <QDebug>
#include "CommonModule/laneutils_new.h"
QVCCodeParamFile::QVCCodeParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QVCCodeParamFile::InitParamFile()
{
    m_nFileID = 2;
    m_sFileName = getCfgFilePath() + "VCCode.cfg";
    m_sNewFileName =  getCfgFilePath() + "VCCode.new";
    m_sDispFileName = "车型编码";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SVCCodeContainer);
}
bool QVCCodeParamFile::loadParam_addRow(const char *dataBuffer)
{
    SVCCodeContainer * tempContainer = (SVCCodeContainer *)dataBuffer;
    quint16 Key;
    SVCCodeValue Value;

    Key=qFromBigEndian(tempContainer->wVCCode);

    Value.wVCCode=qFromBigEndian(tempContainer->wVCCode);
    Value.sVCName=GB2312toUtf8(tempContainer->szVCName);
    Value.wOverSpeed=qFromBigEndian(tempContainer->wOverSpeed);
    Value.wVUSpeed=qFromBigEndian(tempContainer->wVUSpeed);
    Value.wMaxSpeed=qFromBigEndian(tempContainer->wMaxSpeed);
    Value.dwMinVWeight=qFromBigEndian(tempContainer->dwMinVWeight);
    Value.dwMaxVWeight=qFromBigEndian(tempContainer->dwMaxVWeight);
    Value.wMinAxle=qFromBigEndian(tempContainer->wMinAxle);
    Value.wMaxAxle=qFromBigEndian(tempContainer->wMaxAxle);
    Value.sVCDesc=GB2312toUtf8(tempContainer->szVCDesc);
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szUseTime, 14),"yyyyMMddhhmmss");

    //测试用例
    //    //qDebug() << Key << Value.wVCCode << Value.sVCName << Value.wOverSpeed << Value.wVUSpeed
    //                << Value.wMaxSpeed << Value.dwMinVWeight << Value.dwMaxVWeight << Value.wMinAxle
    //                   << Value.wMaxAxle << Value.sVCDesc << Value.sLastVer << Value.sUseTime;
    m_container.addItemToTemp(Key,Value);
    return true;
}

bool QVCCodeParamFile::queryVehClassInfo(SVCCodeValue &vehClassInfo, int nVehClass)
{
    return m_container.findValue(nVehClass, vehClassInfo);
}
