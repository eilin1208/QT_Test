#include <QDateTime>
#ifndef SQLITEDATASAVER_H
#define SQLITEDATASAVER_H
#include "CommonModule/basesaver.h"
#include <QSqlDatabase>
#include "CommonModule/MsgDefine.h"
#include <QMutex>
#include <QObject>
#include <QList>
//#include "dealxmldata.h"
#include <QMutex>
//创建主库中的流水表，如果已存在不重复创建
const QString  CREATE_MAIN_TABLE_STMT =
"CREATE TABLE IF NOT EXISTS 'tbl_LaneWaste'" \
"([ID] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT," \
"[StaId] INTEGER NOT NULL," \
"[LaneId] INTEGER NOT NULL," \
"[OccurTime] INTEGER NOT NULL," \
"[MsgID] INTEGER NOT NULL," \
"[MsgType] INTEGER NOT NULL," \
"[MsgLen] INTEGER NOT NULL," \
"[Msg] BLOB NOT NULL," \
"[TransFlag1] INTEGER NOT NULL," \
"[TransFlag2] INTEGER NOT NULL," \
"[TransFlag3] INTEGER NOT NULL," \
"[TransFlag4] INTEGER NOT NULL);";

const QString CREATE_MAIN_TABLE_INDEX_STMT =
"CREATE INDEX IF NOT EXISTS 'OccurTimeIndex' ON 'tbl_LaneWaste'('OccurTime' ASC)";

const QString CREATE_MAIN_TMP_TABLE_STMT =
"CREATE TABLE IF NOT EXISTS 'tbl_LaneTmp'" \
"([ID] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT," \
"[StaId] INTEGER NOT NULL," \
"[LaneId] INTEGER NOT NULL," \
"[OccurTime] INTEGER NOT NULL," \
"[MsgID] INTEGER NOT NULL," \
"[MsgType] INTEGER NOT NULL," \
"[MsgLen] INTEGER NOT NULL," \
"[Msg] BLOB NOT NULL);";

// 创建主库中的同步表（最多1行），如果已存在不重复创建
const QString  CREATE_MAIN_SYN_TABLE_STMT =
"CREATE TABLE IF NOT EXISTS 'tbl_LaneSynData'" \
"([ID] INTEGER NOT NULL PRIMARY KEY," \
"[StaId] INTEGER NOT NULL," \
"[LaneId] INTEGER NOT NULL," \
"[MsgLen] INTEGER NOT NULL," \
"[Data] BLOB NOT NULL);";

// 创建主库中的大工班表（最多1行），如果已存在不重复创建
const QString CREATE_MAIN_SHIFT_TABLE_STMT =
"CREATE TABLE IF NOT EXISTS 'tbl_LaneShiftData'" \
"([ID] INTEGER NOT NULL PRIMARY KEY," \
"[StaId] INTEGER NOT NULL," \
"[LaneId] INTEGER NOT NULL," \
"[MsgLen] INTEGER NOT NULL," \
"[Data] BLOB NOT NULL);";

class SqliteDataSaver : public BaseSaver
{
    Q_OBJECT
public:
    explicit SqliteDataSaver(QObject *parent = 0);
    virtual ~SqliteDataSaver(){}
    bool ConnectDB();
    bool CreateTable(QSqlDatabase &DB);
    bool ReadStaIdAndLaneID();
    bool SaveNormalData(TDataToSave* PData,bool IsMainDB = true);
    bool SaveTmpData(TDataToSave* PData,bool IsMainDB = true);
    bool SaveSynData(void* PSynData,int SynDataLen,bool IsMainDB = true);
    bool GetMaxID(quint64 &AID);
    bool DoRemoveSynData(bool IsMainDB = true);
    bool DoRemoveShiftData(bool IsMainDB = true);
    bool DoSaveShiftData(void* SShiftData,int DataLen,bool IsMainDB = true);
    void SynExecSql(QSqlQuery& sql,int &id);
    //添加 重载SynExecSql
    bool AttachBackupDB();
    bool DoRemoveData(quint64 AID,bool IsMainDB = true);
    bool DoRemoveTempData(bool IsMainDB = true);
    bool DoMarkDataState(quint64 AID,TDispatchType DispathType,bool IsMainDB = true);
    bool DoDelHistoryData(quint16 SaveDays,bool IsMainDB = true);
    void ProcessDBPathNotExist(QString DBName);
    QString GetTransFlagFieldName(TDispatchType DispathType);
    bool BatchMarkDataState(QList<int> DataIDLst,TDispatchType DispatchType);
    bool SaveBigShiftData(void* PShiftData,int DataLen);
    bool GetBigShiftData(void* PShiftData,int &DataLen);
    bool RemoveBigShiftData();
    void setDBName(QString name);
    QString getDBName();
    void setBackupDBName(QString name);
    QString getBackupDBName();


    bool Init(quint32 StaId, int LaneId);
    bool SaveData(quint64 &AID, TDataToSave *PDate, bool RemoveTmpData, void *PSynData, int SynDataLen);
    //bool SaveTempData(TDataToSave *PData, bool IsMainDB);
    bool SaveTempData(TDataToSave *PDate, void *PSynData, int SynDataLen);
    bool GetTempData(TDataToSave *PData);
    bool RemoveTempData();
    bool GetSynData(void *PDate, int &SynDataLen);
    bool RemoveSynData();
    bool GetFirstData(const quint64 AID, TDataToSave *PData, TDispatchType DispatchType, int OrderNum = 1);
    bool GetNextData(quint64 &AID, TDataToSave *PData);
    bool RemoveData(quint64 AID);
    bool GetDataByCount(QList<QByteArray*>& dataList, int nBegin, int nCount, bool bUnDispatch = false);
    int GetTotalCount();
    bool DelHistoryData(quint16 SaveDays);
    int GetUnDispatchCount(TDispatchType DispatchType);
    int GetUnDispatchID(TDispatchType DispatchType);
    bool MarkDataState(quint64 AID, TDispatchType DispatchType);
    //修改xml相关
    int getStaId(int id);
    int getLaneId(int id);
    bool setDataState(quint64 start,quint64 end,bool isUp);
    QString getDBPath();
    void closeDB();
private:
    QString FDBName;
    QString FBackupDBName;
    bool FAttached;
    int FDataSaveDays;
    bool FInitSuccess;
    QMutex FLock;
    QSqlDatabase Fdb1;
    //bool isExec;
private:
//    DealXmlData m_xmlData;
    QMutex mutex;
signals:

public slots:
private:
};

SqliteDataSaver* getDataSaver();

#endif // SQLITEDATASAVER_H
