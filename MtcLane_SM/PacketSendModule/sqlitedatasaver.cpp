#include "sqlitedatasaver.h"
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include "CommonModule/ilogmsg.h"
#include "CommonModule/LaneType.h"
#include "ZLib/include/zlib.h"
#include <QSettings>
SqliteDataSaver::SqliteDataSaver(QObject *parent) :
    BaseSaver(parent)
{
    FInitSuccess = false;
    FDataSaveDays = 30;
    QSettings setting("./Lane.ini",QSettings::IniFormat);
    setDBName(setting.value("LaneDBSrv/YWDataName").toString());
    setBackupDBName("./YeWu2.db");
    FAttached = false;
}
bool SqliteDataSaver::ConnectDB()
{
    ProcessDBPathNotExist(getDBName());
    Fdb1 = QSqlDatabase::addDatabase("QSQLITE","YEWU");
    Fdb1.setDatabaseName(getDBName());
    if(!QFile().exists(getDBName()))
    {
        LogMsg("lane",tr("数据库%1不存在,创建失败").arg(getDBName()));
        return false;
    }else
        if(!Fdb1.open())
        {
            LogMsg("lane",tr("数据库%1打开失败").arg(getDBName()));
            return false;
        }
        else
            return true;
}

bool SqliteDataSaver::CreateTable(QSqlDatabase &DB)
{
    FLock.lock();
    QSqlQuery query(DB);
    bool bl1 = query.exec(CREATE_MAIN_TABLE_STMT);
    bool bl2 = query.exec(CREATE_MAIN_TABLE_INDEX_STMT);
    bool bl3 = query.exec(CREATE_MAIN_TMP_TABLE_STMT);
    bool bl4 = query.exec(CREATE_MAIN_SYN_TABLE_STMT);
    bool bl5 = query.exec(CREATE_MAIN_SHIFT_TABLE_STMT);
    FLock.unlock();
    if(!bl1 ||!bl2 || !bl3 || !bl4 || !bl5)
    {
        LogMsg("lane",tr("创建数据库表失败"));
        return false;
    }
    LogMsg("lane",tr("创建数据库表成功"));
    return true;
}

bool SqliteDataSaver::ReadStaIdAndLaneID()
{
    QString sql;
    sql = "select StaId,LaneId,max(Id) as Id from tbl_LaneWaste;";
    QSqlQuery query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            FStaID = 0;
            FLaneID = 0;
        }else
        {
            FStaID = query.value(0).toInt();
            FLaneID = query.value(1).toInt();
        }
    }
    return true;
}

bool SqliteDataSaver::SaveNormalData(TDataToSave *PData, bool IsMainDB)
{
    QString sql,s;
    QByteArray m;
    s = tr("insert into %1 (StaId,LaneId,OccurTime,MsgID,MsgType,MsgLen,Msg,TransFlag1,TransFlag2,TransFlag3,TransFlag4)"\
           " values (?,?,?,?,?,?,?,?,?,?,?);");
    if(IsMainDB)
    {
        sql = s.arg("tbl_LaneWaste");
    }
    else
    {   //liujian 备份库不需要
        sql = s.arg("tbl_LaneWaste");
    }
    //TMsgToSend* send = (TMsgToSend*)PData->pMsg;

    m = QByteArray::fromRawData(PData->pMsg,PData->nMsgLen);
    QSqlQuery query(sql, Fdb1);
    query.bindValue(0, FStaID);
    query.bindValue(1, FLaneID);
    query.bindValue(2, PData->OccurTime);
    query.bindValue(3, PData->MsgID);
    query.bindValue(4, PData->MsgType);
    query.bindValue(5, PData->nMsgLen);
    query.bindValue(6, m);
    query.bindValue(7, PData->TransFlag1);
    query.bindValue(8, PData->TransFlag2);
    query.bindValue(9, PData->TransFlag3);
    query.bindValue(10, PData->TransFlag4);
    int insertId = 0;
    LogMsg("lane",tr("SaveNormalData执行"));
    SynExecSql(query,insertId);
    if(insertId != 0)
    {
        LogMsg("lane",tr("开始备份未上传xml文件 ID:%1").arg(insertId));
        LogMsg("lane",tr("备份未上传xml文件完成 ID:%1").arg(insertId));
    }
    m.clear();
    return true;
}

bool SqliteDataSaver::SaveTmpData(TDataToSave *PData, bool IsMainDB)
{
    QString sql,s;
    QByteArray m;
    s = "delete from %1";
    if(IsMainDB)
    {
        sql = s.arg("tbl_LaneTmp");
    }else
    {
        sql = s.arg("tbl_LaneTmp");
    }
    Fdb1.exec(sql);
    s = "insert into %1 (StaId,LaneId,OccurTime,MsgID,MsgType,MsgLen,Msg) values(?,?,?,?,?,?,?)";
    if(IsMainDB)
    {
        sql = s.arg("tbl_LaneTmp");
    }else
    {
        //liujian 备份库不需要
        sql = s.arg("tbl_LaneTmp");
    }
    m = QByteArray::fromRawData((char*)PData->pMsg,PData->nMsgLen);
    QSqlQuery query(sql,Fdb1);
    query.bindValue(0, FStaID);
    query.bindValue(1, FLaneID);
    query.bindValue(2, PData->OccurTime);
    query.bindValue(3, PData->MsgID);
    query.bindValue(4, PData->MsgType);
    query.bindValue(5, PData->nMsgLen);
    query.bindValue(6, m);
    int insertId = 0;
    LogMsg("lane",tr("SaveTmpData执行"));
    SynExecSql(query,insertId);
    m.clear();
    return true;
}

bool SqliteDataSaver::SaveSynData(void *PSynData, int SynDataLen, bool IsMainDB)
{
    QString s,sql;
    QByteArray m;
    bool ret = DoRemoveSynData(IsMainDB);
    s = "insert into %1 (StaId,LaneId,MsgLen,Data) values (?,?,?,?)";
    if(IsMainDB)
    {
        sql = s.arg("tbl_LaneSynData");
    }else
    {
        //liujian 备份库不需要
        sql = s.arg("tbl_LaneSynData");
    }
    m = QByteArray::fromRawData((char*)PSynData,SynDataLen);
    QSqlQuery query(sql, Fdb1);
    query.bindValue(0, FStaID);
    query.bindValue(1, FLaneID);
    query.bindValue(2, SynDataLen);
    query.bindValue(3, m);
    int insertId = 0;
    LogMsg("lane",tr("SaveSynData执行"));
    SynExecSql(query,insertId);

    return true;
}

bool SqliteDataSaver::GetMaxID(quint64 &AID)
{
    QString sql;
    sql = "select max(ID) as MaxID from tbl_LaneWaste;";
    QSqlQuery query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("[SqliteDataSaver.GetMaxID]查询数据为0 %1").arg(sql));
            return false;
        }else
        {
            AID = query.value(0).toInt();
        }
    }
    return true;
}

bool SqliteDataSaver::DoRemoveSynData(bool IsMainDB)
{
    QString sql;
    if(IsMainDB)
    {
        sql = "delete from tbl_LaneSynData;";
    }else
    {
        sql = "delete from tbl_LaneSynData;";
    }
    QSqlQuery query(sql, Fdb1);
    LogMsg("lane",tr("DoRemoveSynData执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}

bool SqliteDataSaver::DoRemoveShiftData(bool IsMainDB)
{
    QString sql;
    if(IsMainDB)
    {
        sql = "delete from tbl_LaneShiftData;";
    }else
    {
        sql = "delete from tbl_LaneShiftData;";
    }
    QSqlQuery query(sql,Fdb1);
    LogMsg("lane",tr("DoRemoveShiftData执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}
bool SqliteDataSaver::DoSaveShiftData(void *SShiftData, int DataLen, bool IsMainDB)
{
    QString sql,s;
    QByteArray m;
    bool ret = DoRemoveShiftData(IsMainDB);
    s = "insert into %1 (StaId,LaneId,MsgLen,Data) values (?,?,?,?)";
    if(IsMainDB)
    {
        sql = s.arg("tbl_LaneShiftData");
    }else
    {
        sql = s.arg("tbl_LaneShiftData");
    }
    m = QByteArray::fromRawData((char*)SShiftData, DataLen);
    QSqlQuery query(sql,Fdb1);
    query.bindValue(0, FStaID);
    query.bindValue(1, FLaneID);
    query.bindValue(2, DataLen);
    query.bindValue(3, m);
    LogMsg("lane",tr("DoSaveShiftData执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    //m.clear();
    return ret == true;

}

bool SqliteDataSaver::AttachBackupDB()
{
    return true;
}

void SqliteDataSaver::SynExecSql(QSqlQuery& sql,int &id)
{
    QMutexLocker locker(&FLock);
    if(Fdb1.isOpen() == false)
    {
        //LogMsg("lane", tr("数据库打开失败"));
        LogMsg("lane", tr("[SqliteDataSaver.SynExecSql]数据库未打开,将打开数据库"));
        Fdb1.open();
        if(Fdb1.isOpen() == false)
        {
            LogMsg("lane", tr("数据库打开失败"));
            return;
        }
        //return;
    }
    if(sql.exec() == false)
    {
        LogMsg("lane", tr("执行失败%1,sql:%2").arg(sql.lastError().text()).arg(sql.executedQuery()));
        return;
    }
    if(sql.lastInsertId().isValid())
    {
        id = sql.lastInsertId().toInt();
    }
}

bool SqliteDataSaver::DoRemoveData(quint64 AID, bool IsMainDB)
{
    QString sql;
    if(IsMainDB)
    {
        sql = tr("delete from %1 where ID = %2;").arg("tbl_LaneWaste").arg(AID);
    }else
    {
        //liujian 备份库不需要
        sql = tr("delete from %1 where ID = %2;").arg("tbl_LaneWaste").arg(AID);
    }
    QSqlQuery query(sql, Fdb1);
    LogMsg("lane",tr("DoRemoveData执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}

bool SqliteDataSaver::DoMarkDataState(quint64 AID, TDispatchType DispathType, bool IsMainDB)
{
    QString s,sql;
    QString FieldName;

    FieldName = GetTransFlagFieldName(DispathType);
    if(IsMainDB)
    {
        sql = tr("update %1 set %2 = 1 where ID = %3;").arg("tbl_LaneWaste").arg(FieldName).arg(AID);
        s = "主库";
    }else
    {
        //liujian 备份库不需要
        sql = tr("update %1 set %2 = 1 where ID = %3;").arg("tbl_LaneWaste").arg(FieldName).arg(AID);
        s = "备份库";
    }
    QSqlQuery query(sql, Fdb1);
    LogMsg("lane",tr("DoMarkDataState执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}

bool SqliteDataSaver::DoDelHistoryData(quint16 SaveDays, bool IsMainDB)
{
    QString sql;
    QDateTime time1,time2;
    quint64 d1;
    time1 = QDateTime::currentDateTime().addDays(-SaveDays);
    time1.setTime(QTime(23,59,59,999));
    time2 = time1.addMSecs(-10);
    d1 = time1.toTime_t();
    //d2 = time2.toTime_t();
    if(IsMainDB)
    {
        sql = tr("delete from %1 where (OccurTime <= %2) and TransFlag1 = 1;").arg("tbl_LaneWaste").arg(d1);
    }else
    {
        sql = tr("delete from %1 where (OccurTime < %2) and TransFlag1 = 1;").arg("tbl_LaneWaste").arg(d1);
    }
    QSqlQuery query(sql, Fdb1);
    int insertId = 0;
    SynExecSql(query,insertId);

    return true;
}

void SqliteDataSaver::ProcessDBPathNotExist(QString DBName)
{
    if(!QFile().exists(DBName))
    {
        LogMsg("lane", tr("数据库%1不存在,程序进行创建").arg(DBName));
        QFile file(DBName);
        if(file.open(QIODevice::ReadWrite))
        {
            Fdb1 = QSqlDatabase::addDatabase("QSQLITE");
            Fdb1.setDatabaseName(getDBName());
            if(!Fdb1.open())
            {
                LogMsg("lane", tr("数据库创建完成,打开失败").arg(getDBName()));
            }
            CreateTable(Fdb1);
            LogMsg("lane", tr("数据库 %1创建完成").arg(DBName));
            file.close();
            return;
        }
    }else
    {
        LogMsg("lane", tr("数据库 %1已存在").arg(DBName));
        return;
    }
}

bool SqliteDataSaver::Init(quint32 StaId, int LaneId)
{
    LogMsg("lane", tr("SqliteDataSaver::Init开始"));
    FInitSuccess = false;
    if(!ConnectDB())
    {
        LogMsg("lane", tr("SqliteDataSaver::Init数据库连接ConnectDB失败"));
        return false;
    }

    if(!ReadStaIdAndLaneID())
    {
        LogMsg("lane", tr("SqliteDataSaver::Init--ReadStaIdAndLaneID()失败"));
        return false;
    }
    if(FStaID == 0 && FLaneID == 0)
    {
        FStaID = StaId;
        FLaneID = LaneId;
    }else
    {
        if(FStaID != StaId || FLaneID != LaneId)
        {
            return false;
        }
    }
    FInitSuccess = true;
    //数据库初始化时进行一次删除
    QSettings setting("./Lane.ini", QSettings::IniFormat);

    DoDelHistoryData(setting.value("StationInfo/SqlSaveDay", 10).toInt(),true);
    LogMsg("lane", tr("SqliteDataSaver::Init结束"));
    return true;
}

bool SqliteDataSaver::SaveData(quint64 &AID, TDataToSave *PDate, bool RemoveTmpData, void *PSynData, int SynDataLen)
{
    TDataToSave* AData;
    QByteArray ms;
    if(!FInitSuccess)
    {
        LogMsg("lane", tr("[TSqliteDataSaver.SaveData]可能站代码或车道代码配置错误,导致TSqliteDataSaver.init失败, 禁止写入数据"));
    }
    AData = PDate;
    ms = QByteArray::fromRawData((char*)(PDate->pMsg),PDate->nMsgLen);
    LogMsg("lane","进入流前BeginTransaction");
    Fdb1.transaction();
    if(!SaveNormalData(AData))
    {
        Fdb1.rollback();
        LogMsg("lane","[SaveNormalData]保存到主库时失败");
    }
    if(!SaveSynData(PSynData,SynDataLen))
    {
        Fdb1.rollback();
        LogMsg("lane","[SaveNormalData]保存到主库时失败");
    }
    if(RemoveTmpData)
    {
        if(!DoRemoveTempData())
        {
            Fdb1.rollback();
            LogMsg("lane","[SaveNormalData]保存到主库时失败");
        }
    }
    Fdb1.commit();

    GetMaxID(AID);
    return true;
}

bool SqliteDataSaver::GetTempData(TDataToSave *PData)
{
    QString sql;
    QByteArray m;
    sql = "select * from tbl_LaneTmp;";
    QSqlQuery query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("查询数据为0 %1").arg(sql));
            return false;
        }else
        {
            PData->OccurTime = query.value(3).toInt();
            PData->MsgID =  query.value(4).toInt();
            PData->MsgType =  query.value(5).toInt();
            PData->nMsgLen =  query.value(6).toInt();
            m =  query.value(7).toByteArray();
            memcpy(PData->pMsg,m.data(),m.length());
            return true;
        }
    }
    return true;
}

bool SqliteDataSaver::GetSynData(void *PDate, int &SynDataLen)
{
    QString sql;
    QByteArray m;
    int len;
    sql = "select * from tbl_LaneSynData;";
    QSqlQuery query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("查询数据为0 %1").arg(sql));
            return false;
        }else
        {
            len = query.value(3).toInt();
            m = query.value(4).toByteArray();
            memcpy(PDate,m.data(),qMin(m.length(),SynDataLen));
            SynDataLen = 0;
            return true;
        }
    }
    return true;
}

bool SqliteDataSaver::RemoveSynData()
{
    bool ret = DoRemoveSynData(true);

    return ret;
}

bool SqliteDataSaver::GetFirstData(const quint64 AID, TDataToSave *PData, TDispatchType DispatchType, int OrderNum)
{
    QString sql;
    QByteArray m;
    QString FiledName;
    int StaID,LaneID;
    bool ret = false;
    FiledName = GetTransFlagFieldName(DispatchType);
    sql = tr("select * from tbl_LaneWaste where ID = %1;").arg(AID);
    QSqlQuery  query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("TSqliteDataSaver.GetFirstData]记录数为0"));
            return false;
        }else
        {
            PData->OccurTime = query.value(3).toInt();
            PData->MsgID = query.value(4).toInt();
            PData->MsgType = query.value(5).toInt();
            StaID = query.value(1).toInt();
            LaneID = query.value(2).toInt();
            PData->nMsgLen = query.value(6).toInt();
            m = query.value(7).toByteArray();
            //解压缩
            int zlibLen = 10240;
            uncompress((Bytef*)PData->pMsg, (uLongf*)&zlibLen, (Bytef*)m.data(), m.length());
            QString tmpstr = tr("%1%2").arg(StaID,6,10, QLatin1Char('0'))
                    .arg(LaneID, 3, 10, QLatin1Char('0'));
            memcpy(((TMsgToSend*)(PData->pMsg))->BasicInfo.msgHead.Sender,tmpstr.toLocal8Bit().data(), 9);
            PData->TransFlag1 = query.value(8).toInt();
            PData->TransFlag2 = query.value(9).toInt();
            PData->TransFlag3 = query.value(10).toInt();
            PData->TransFlag4 = query.value(11).toInt();
            ret = true;
        }
    }
    return ret;
}

bool SqliteDataSaver::RemoveData(quint64 AID)
{
    bool ret = DoRemoveData(AID,true);

    return ret;
}

int SqliteDataSaver::GetUnDispatchCount(TDispatchType DispatchType)
{
    QString sql;
    QString FieldName;
    FieldName = GetTransFlagFieldName(DispatchType);
    sql = tr("select count(*) from tbl_LaneWaste where %1 = 0").arg(FieldName);
    QSqlQuery query = Fdb1.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("查询数据为0 %1").arg(sql));
            return 0;
        }else
        {
            return query.value(0).toInt();
        }
    }
    return 0;
}

bool SqliteDataSaver::MarkDataState(quint64 AID, TDispatchType DispatchType)
{
    bool ret = DoMarkDataState(AID,DispatchType,true);

    return ret;
}

bool SqliteDataSaver::BatchMarkDataState(QList<int> DataIDLst, TDispatchType DispatchType)
{
    int i;
    quint64 p;
    Fdb1.transaction();
    if(DataIDLst.count() == 0)
    {
        LogMsg("lane","DataIDLst.count() = 0");
        Fdb1.rollback();
        return false;
    }else
    {
        for(i = 0;i <= DataIDLst.count() -1;i++)
        {
            p = (quint64)DataIDLst.at(i);
            DoMarkDataState(p,DispatchType,true);
            DoMarkDataState(p,DispatchType,false);
        }
        Fdb1.commit();
        return true;
    }
}

bool SqliteDataSaver::SaveBigShiftData(void *PShiftData, int DataLen)
{
    if(!FInitSuccess)
    {
        LogMsg("lane","[TSqliteDataSaver.SaveBigShiftData]可能站代码或车道代码配置错误,导致TSqliteDataSaver.init失败, 禁止写入数据");
        return false;
    }
    if(!DoSaveShiftData(PShiftData,DataLen))
    {
        return false;
    }
    return true;
}

bool SqliteDataSaver::GetBigShiftData(void *PShiftData, int &DataLen)
{
    QString sql;
    QByteArray m;
    int len;
    sql = "select * from tbl_LaneShiftData";
    QSqlQuery query(Fdb1);
    query.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("Lane",tr("查询数据为0 %1").arg(sql));
            return false;
        }
        else
        {
            len = query.value(3).toInt();
            m = query.value(4).toByteArray();
            DataLen = qMin(len,DataLen);
            memcpy(PShiftData, m.data(), DataLen);
        }
        DataLen = 0;
    }
    m.clear();
    return true;
}

bool SqliteDataSaver::RemoveBigShiftData()
{
    bool ret = DoRemoveShiftData(true);

    return ret;
}

void SqliteDataSaver::setDBName(QString name)
{
    FDBName = name;
}

QString SqliteDataSaver::getDBName()
{
    return FDBName;
}

void SqliteDataSaver::setBackupDBName(QString name)
{
    FBackupDBName = name;
}

QString SqliteDataSaver::getBackupDBName()
{
    return FBackupDBName;
}

bool SqliteDataSaver::GetNextData(quint64 &AID, TDataToSave *PData)
{
    return true;
}
//删除过时数据,每次只删除10分钟的数据
bool SqliteDataSaver::DelHistoryData(quint16 SaveDays)
{
    bool result = DoDelHistoryData(SaveDays);

    return result;

}

bool SqliteDataSaver::RemoveTempData()
{
    bool ret = DoRemoveTempData(true);

    return ret;
}

QString SqliteDataSaver::GetTransFlagFieldName(TDispatchType DispathType)
{
    QString strTmp;
    switch(DispathType)
    {
    case dtSocket:
        strTmp = "TransFlag1";
        break;
    case dtSqlServer:
        strTmp = "TransFlag2";
        break;
    case dtReserve1:
        strTmp = "TransFlag3";
        break;
    case dtReserve2:
        strTmp = "TransFlag4";
        break;
    }
    return strTmp;
}

bool SqliteDataSaver::DoRemoveTempData(bool IsMainDB)
{
    QString sql;
    if(IsMainDB)
    {
        sql = "delete from tbl_LaneTmp;";
    }else
    {
        // liujian 备份库不需要
        sql = "delete from tbl_LaneTmp;";
    }
    QSqlQuery query(sql, Fdb1);
    LogMsg("lane",tr("DoRemoveTempData执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}

bool SqliteDataSaver::SaveTempData(TDataToSave *PDate, void *PSynData, int SynDataLen)
{
    if(!FInitSuccess)
    {
        LogMsg("Lane","[TSqliteDataSaver.SaveTempData]可能站代码或车道代码配置错误,导致TSqliteDataSaver.init失败, 禁止写入数据");
        return false;
    }
    Fdb1.transaction();
    if(!SaveTmpData(PDate))
    {
        Fdb1.rollback();
    }
    if(!SaveSynData(PSynData,SynDataLen))
    {
        Fdb1.rollback();
    }
    Fdb1.commit();

    return true;
}

int SqliteDataSaver::GetUnDispatchID(TDispatchType DispatchType)
{
    QString sql;
    int ret = 0;
    QString FieldName = GetTransFlagFieldName(DispatchType);
    if(GetUnDispatchCount(DispatchType) == 0)
        return 0;
    sql = tr("select min(ID) as ID from tbl_LaneWaste where %1 = 0").arg(FieldName);
    QSqlQuery query(Fdb1);
    query.exec(sql);
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("Lane",tr("查询数据为0 %1").arg(sql));
            return -1;
        }
        ret = query.value(0).toInt();
    }
    return ret;
}

bool SqliteDataSaver::GetDataByCount(QList<QByteArray*> &dataList, int nBegin, int nCount, bool bUnDispatch)
{
    QString sql;
    if(bUnDispatch)
    {
        sql = tr("select * from tbl_LaneWaste where TransFlag1 = 0 limit %1,%2").arg(nBegin).arg(nCount);
    }
    else
    {
        sql = tr("select * from tbl_LaneWaste limit %1, %2").arg(nBegin).arg(nCount);
    }
    QSqlQuery query(sql, Fdb1);
    QByteArray *tempBox = NULL;
    bool ret = query.exec();
    while(query.next())
    {
        tempBox = new QByteArray();
        //ID
        tempBox->append(QByteArray::number(query.value(0).toInt()));
        tempBox->append(',');
        //StaId
        tempBox->append(QByteArray::number(query.value(1).toInt()));
        tempBox->append(',');
        //LaneId
        tempBox->append(QByteArray::number(query.value(2).toInt()));
        tempBox->append(',');
        //OccurTime
        tempBox->append(QByteArray::number(query.value(3).toLongLong()));
        tempBox->append(',');
        //MsgID
        tempBox->append(QByteArray::number(query.value(4).toInt()));
        tempBox->append(',');
        //MsgType
        tempBox->append(QByteArray::number(query.value(5).toInt()));
        tempBox->append(',');
        //MsgLen
        tempBox->append(QByteArray::number(query.value(6).toInt()));
        tempBox->append(',');
        //Msg
        tempBox->append(query.value(7).toByteArray());
        tempBox->append(',');
        //TransFlag1
        tempBox->append(QByteArray::number(query.value(8).toInt()));
        tempBox->append(',');
        //TransFlag2
        tempBox->append(QByteArray::number(query.value(9).toInt()));
        tempBox->append(',');
        //TransFlag3
        tempBox->append(QByteArray::number(query.value(10).toInt()));
        tempBox->append(',');
        //TransFlag4
        tempBox->append(QByteArray::number(query.value(11).toInt()));
        tempBox->append('\n');
        dataList.append(tempBox);
    }
    return ret;
}

int SqliteDataSaver::GetTotalCount()
{
    QString sql = tr("select count(*) from tbl_LaneWaste;");
    QSqlQuery query(sql, Fdb1);
    int ret = 0;
    query.exec();
    if(query.next())
    {
        bool bRet = false;
        ret = query.value(0).toInt(&bRet);
        if(!bRet)
        {
            return -1;
        }
    }
    return ret;
}



int SqliteDataSaver::getStaId(int id)
{
    QString sql = tr("select StaId from tbl_LaneWaste where TransFlag1 = 0");
    QSqlQuery query(sql,Fdb1);
    int ret = 0;
    query.exec();
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("lane",tr("查询数据为0 %1").arg(sql));
            return ret;
        }
        else
            ret = query.value(0).toInt();
    }
    return ret;
}

int SqliteDataSaver::getLaneId(int id)
{
    QString sql = tr("select LaneId from tbl_LaneWaste where TransFlag1 = 0");
    QSqlQuery query(sql,Fdb1);
    int ret = 0;
    query.exec();
    while(query.next())
    {
        if(query.size() == 0)
        {
            LogMsg("Lane",tr("查询数据为0 %1").arg(sql));
            return ret;
        }
        else
            ret = query.value(0).toInt();
    }
    return ret;
}

bool SqliteDataSaver::setDataState(quint64 start, quint64 end, bool isUp)
{
    int ret = isUp;
    QString sql = tr("update tbl_LaneWaste set TransFlag1 = %1 where OccurTime between %2 and %3;").arg(ret).arg(start).arg(end);
    QSqlQuery query(sql, Fdb1);
    LogMsg("lane",tr("setDataState执行"));
    int insertId = 0;
    SynExecSql(query,insertId);
    return true;
}

QString SqliteDataSaver::getDBPath()
{
    QSettings setting("./Lane.ini",QSettings::IniFormat);
    return setting.value("LaneDBSrv/YWDataName").toString();
}

void SqliteDataSaver::closeDB()
{
    Fdb1.close();
    //    QString sql = tr("select * from tbl_LaneWaste");
    //    QSqlQuery query(sql,Fdb1);
    //    QString name = Fdb1.connectionName();
    //    Fdb1.removeDatabase(name);
}
SqliteDataSaver* getDataSaver()
{
    static QMutex sqlMutex;
    QMutexLocker locker(&sqlMutex);
    static SqliteDataSaver saver;
    return &saver;
}
