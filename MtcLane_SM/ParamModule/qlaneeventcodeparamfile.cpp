#include "qlaneeventcodeparamfile.h"
#include "parammapcontainer.h"
#include <QtEndian>
#include "CommonModule/laneutils_new.h"
QLaneEventCodeParamFile::QLaneEventCodeParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer = &m_container;
}

void QLaneEventCodeParamFile::InitParamFile()
{
    m_nFileID = 21;
    m_sFileName = getCfgFilePath() + "LaneEvent.cfg";
    m_sNewFileName =  getCfgFilePath() + "LaneEvent.new";
    m_sDispFileName = "车道事件编码";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SLaneEvenCodeContainer);
}
bool QLaneEventCodeParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SLaneEvenCodeContainer * tempContainer = (SLaneEvenCodeContainer * )dataBuffer;
    quint16 Key;
    SLaneEventValue Value;
    Key=qFromBigEndian(tempContainer->wEventCode);
    Value.wEventCode=qFromBigEndian(tempContainer->wEventCode);
    Value.sCaption=QString::fromLocal8Bit(tempContainer->szCaption);
    Value.bSuperAuthorize=atoi(tempContainer->szSuperAuthorize);
    Value.wSuperOverTime=qFromBigEndian(tempContainer->wSuperOverTime);
    Value.wSuperConfirmTime=qFromBigEndian(tempContainer->wSuperConfirmTime);
    Value.bMonitorAuthorize=atoi(tempContainer->szMonitorAuthorize);
    //--DEBUG
//    //qDebug() << Key << ":" << Value.wEventCode << Value.sCaption << Value.bSuperAuthorize
//             << Value.wSuperOverTime << Value.wSuperConfirmTime << Value.bMonitorAuthorize;
    m_container.addItemToTemp(Key, Value);
    return true;
}
