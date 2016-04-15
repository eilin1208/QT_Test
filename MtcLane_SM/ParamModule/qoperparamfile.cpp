#include "qoperparamfile.h"
#include "CommonModule/laneutils_new.h"
#include <QDebug>
const int OPERCODE_LEN = 100000;

QOperParamFile::QOperParamFile(QObject *parent) :
    QBaseParamFile(parent)
{
    m_pContainer=&m_container;
}

void QOperParamFile::InitParamFile()
{
    m_nFileID = 23;
    //m_nCfgFileType = cfOper;
    m_sFileName = getCfgFilePath() + "Oper.cfg";
    //m_sFileName = "Oper.cfg";
    m_sNewFileName = getCfgFilePath() + "Oper.new";
    m_sDispFileName = "操作员字典";
    m_bAllowNull = false;
    m_bDownLoadOnTime = true;
    m_bBootMust = true;
    m_bIsLoad = false;
    m_nDataContainerLength = sizeof(SOperContainer);
}

bool QOperParamFile::loadParam_addRow(const char *dataBuffer)
{
    // 将数据转化为key与value，存放在map中
    SOperContainer * tempContainer = (SOperContainer * )dataBuffer;
    quint32 Key;
    SOperInfoValue Value;

    Key=atoi(tempContainer->szOper);
    Value.dwOper=Key;
    Value.sOperName=GB2312toUtf8(tempContainer->szOperName);
    Value.wOperType=qFromBigEndian(tempContainer->wOperType);
    Value.dwNetId=atoi(tempContainer->szNetId);
    Value.dwSubCenter=atoi(tempContainer->szSubCenter);
    Value.dwStation=atoi(tempContainer->szStation);
    //Value.sFormOrg=tempContainer->szFormOrg;
    Value.wGroupId=qFromBigEndian(tempContainer->wGroupId);
    Value.dwOperCardID=atoi(tempContainer->szOperCardID);
    Value.szPassWord=QString::fromLocal8Bit(tempContainer->szPassWord);
    //Value.sFunRole = QString(tempContainer->szFunRole+    16);
    memcpy(Value.szFunRole, tempContainer->szFunRole, sizeof(Value.szFunRole));
    //--DEBUG
    //qDebug() << Key << ":" << Value.dwOper << Value.sOperName << Value.wOperType;
    m_container.addItemToTemp(Key, Value);
    return true;
}

bool QOperParamFile::getOperInfoByOperNo(quint32 operNo, SOperInfoValue& value)
{
    return m_container.findValue(operNo,value);
}

bool QOperParamFile::verifyOperCard(quint32 operId, quint32 operCardId, SOperInfoValue &operInfo, QString& sError)
{
    QMap<quint32, SOperInfoValue>::const_iterator it = m_container.getMap().begin();
    for(; it != m_container.getMap().end(); it++)
    {
        if(it.value().dwOper == operId)
        {
            //qDebug()<<"operCardId="<<operCardId<<"    it.value().dwOperCardID="<<it.value().dwOperCardID;
            //判断卡号
            if(operCardId != it.value().dwOperCardID)
            {
                sError = tr("卡号不符");
                return false;
            }
            operInfo = it.value();
            return true;
        }
    }
    sError = tr("无此收费员");
    return false;
}

