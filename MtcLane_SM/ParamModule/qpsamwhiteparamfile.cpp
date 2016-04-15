#include "qpsamwhiteparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QDebug>
QPSamWhiteParamFile::QPSamWhiteParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QPSamWhiteParamFile::InitParamFile()
{
    m_nFileID = 1;
    m_sFileName = getCfgFilePath() + "PSamWhite.cfg";
    m_sNewFileName =  getCfgFilePath() + "PSamWhite.new";
    m_sDispFileName = "PSam白名单";
    m_bAllowNull = true;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SPSamWhiteContainer);
}
bool QPSamWhiteParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SPSamWhiteContainer * tempContainer = (SPSamWhiteContainer * )dataBuffer;
    SPSamWhiteKey Key;
    SPSamWhiteValue Value;

    quint8 TermId[6];
    memset(TermId,0,sizeof (TermId));
    //添加 Hex2Raw 处理
    if(!Hex2Raw(TermId,tempContainer->szTermCode,12))
    {
        //LogFmtMsg(_T("终端机编号%s,非16进制字符串,转换失败!"),szTermCode);
        //QFileLogger::WriteLog("QPSamWhiteParamFile",tr(("终端机编号%1,非16进制字符串,转换失败!"))
        //       .arg(tempContainer->szTermCode),QFileLogger::LOG_ERROR);
        return false;
    }

    char szTmpTermCode[13]="";
    //添加  Raw2HexStr处理
    Raw2HexStr(szTmpTermCode,TermId,sizeof(TermId));

    Key.sTermId = QString::fromLocal8Bit(szTmpTermCode);

    Value.sPSam=Key.sTermId;
    Value.wPSamType=qFromBigEndian(tempContainer->wPSamType);

    Value.sTermCode=QString::fromLocal8Bit(tempContainer->szTermCode);
    Value.wOccurOrgType=qFromBigEndian(tempContainer->wOccurOrgType);
    Value.sOccurOrg=QString::fromLocal8Bit(tempContainer->szOccurOrg);
    Value.sUseLane=QString::fromLocal8Bit(tempContainer->szUseLane);
    Value.sStatus=QString::fromLocal8Bit(tempContainer->szStatus);
    Value.sLastVer=QString::fromLocal8Bit(tempContainer->szLastVer);
    Value.sVerUseTime=QDateTime::fromString(QString::fromLocal8Bit(tempContainer->szVerUseTime));
    //--DEBUG
//    //qDebug() << Key.sTermId << ":" << Value.sPSam << Value.wPSamType <<
//                Value.sTermCode << Value.wOccurOrgType << Value.sOccurOrg<<
//                Value.sUseLane << Value.sStatus << Value.sLastVer << Value.sVerUseTime;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool operator <(const SPSamWhiteKey &a, const SPSamWhiteKey &b)
{
    if (a.sTermId<b.sTermId)
        return true;
    else
        return false;
}
