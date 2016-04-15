#ifndef QOPERPARAMFILE_H
#define QOPERPARAMFILE_H

#include "qbaseparamfile.h"

enum TOperatorRoleID
{
    OR_TOLL,			//发卡收费
    OR_Special,			//特情认证
    OR_Maintaince		//维护
};

//文件存储结构
#pragma pack(push,1)
struct SOperContainer
{
    char szOper[10];
    char szOperName[21];
    quint16 wOperType;
    char szNetId[7];
    char szSubCenter[7];
    char szStation[7];
    quint16 wGroupId;
    char szOperCardID[10];
    char szPassWord[33];
    char szFunRole[257];
};
#pragma pack(pop)

struct SOperInfoValue
{
    quint32 dwOper;
    QString sOperName;
    quint16 wOperType;
    quint32 dwNetId;
    quint32 dwSubCenter;
    quint32 dwStation;
    quint16 wGroupId;
    quint32 dwOperCardID;
    QString szPassWord;
    char szFunRole[257];
public:
    void operator =(const SOperInfoValue& rp)
    {
        dwOper = rp.dwOper;
        dwNetId = rp.dwNetId;
        dwOperCardID = rp.dwOperCardID;
        dwStation = rp.dwStation;
        dwSubCenter = rp.dwSubCenter;
        sOperName = rp.sOperName;
        szPassWord = rp.szPassWord;
        wGroupId = rp.wGroupId;
        wOperType = rp.wOperType;
        qMemCopy(szFunRole, rp.szFunRole, sizeof(szFunRole));
    }
    void clear()
    {
        dwOper = 0;
        dwNetId = 0;
        dwOperCardID = 0;
        dwStation = 0;
        dwSubCenter = 0;
        sOperName.clear();
        szPassWord.clear();
        wGroupId = 0;
        wOperType = 0;
        memset(szFunRole, 0, sizeof(szFunRole));
    }
};

class QOperParamFile : public QBaseParamFile
{
    Q_OBJECT
public:
    explicit QOperParamFile(QObject *parent = 0);
    virtual void InitParamFile();
protected:
    virtual bool loadParam_addRow(const char *dataBuffer);
    ParamMapContainer<quint32, SOperInfoValue> m_container;
signals:

public slots:

public:
    bool getOperInfoByOperNo(quint32 operNo, SOperInfoValue& value);
    bool verifyOperCard(quint32 operId, quint32 operCardId, SOperInfoValue& operInfo, QString& sError);
};

#endif // QOPERPARAMFILE_H
