#include<QVector>
#include <QtSql>
#include<QList>
#include<QDebug>
#include"CommonModule/laneutils_new.h"
#include "LocalCfgDb.h"
#include"LogModule/qfilelogger.h"

// 数据库文件名
static const char *m_szDbFileName="./LocalCfg.db";


// 根据输入拼音查询可选车牌号
bool QLocalDbCfg ::QueryVehPlateChar(QString szPinyin,QStringList  &ListResult)
{
    PromptIsConnectDB();
    QSqlQuery query("", m_Db);
    query.prepare("select Hanzi from tbl_PlateChar where Pinyin like :szPinyin group by Hanzi");
    query.bindValue(":szPinyin",szPinyin+"%");
    if (query.exec() != true)
    {
        ////qDebug() << "根据输入拼音查询可选车牌号失败，错误代码为: " << query.lastError().text();
        return false;
    }
    while(query.next())
    {
        ListResult.append(QObject::tr(query.value(0).toString().toLocal8Bit().data()));
    }

    if(ListResult.size()==0)
    {
        ////qDebug()<<"VehPlate do not exist or Ensure that the correct spelling";
        return false;
    }
    return true;
}


// 清空收费站字典表
bool QLocalDbCfg ::ClearSfzDic()
{
    PromptIsConnectDB();
    QSqlQuery query;
    query.prepare("delete from tbl_SfzPinyin ");
    if (query.exec() != true)
    {
        ////qDebug() << "清空收费站字典表失败，错误代码为: " << query.lastError().text();
        return false;

    }
    if(query.numRowsAffected()!= -1)
    {
        return true;
    }
    else
    {
        ////qDebug()<<"ClearSfzDic fail";
        return false;
    }
}

// 根据输入收费站简称拼音查询符合条件的收费站
bool QLocalDbCfg ::QuerySfzList(const char *szSfzJcPinyin, QStringList  &SfzList)
{
    PromptIsConnectDB();
    QSqlQuery query;
    query.prepare("select SfzJc from tbl_SfzPinyin where Pinyin like :szSfzJcPinyin");
    query.bindValue(":szSfzJcPinyin",(QString)szSfzJcPinyin+"%");
    if (query.exec() != true)
    {
        ////qDebug() << "根据输入收费站简称拼音查询符合条件的收费站失败，错误代码为: " << query.lastError().text();
        return false;
    }
    while(query.next())
    {
        SfzList.append(QObject::tr(query.value(0).toString().toLocal8Bit().data()));
    }
    if(SfzList.isEmpty())
    {
        ////qDebug()<<"Station not exist or Ensure that the correct spelling";
        return false;
    }
    return true;
}

// 根据收费站简称查询对应的拼音首字母
bool QLocalDbCfg::QuerySfzJcPinyin(const QString &sSfzJc, QString &sSfzJcPinyin)
{
    PromptIsConnectDB();
    QSqlQuery query;
    query.prepare("select Pinyin from tbl_SfzPinyin where SfzJc = :sSfzJc");
    query.bindValue(":sSfzJc",sSfzJc);
    if (query.exec() != true)
    {
        ////qDebug() << "根据收费站简称查询对应的拼音首字母失败，错误代码为: " << query.lastError().text();
        return false;
    }
    if(!query.next())
    {
        ////qDebug()<<"Station not exist or Ensure that the correct short";
        return false;
    }
    else
    {
        sSfzJcPinyin = QObject::tr(query.value(0).toString().toLocal8Bit().data());
        return true;
    }
}
// 向收费站字典表中添加一条记录
bool QLocalDbCfg ::AddRecToSfzDic(const QString &sSfzJc, const QString &sPinyin)
{
    PromptIsConnectDB();
    //查询数据库，判断该记录是否存在(如果存在报错，如果不存在执行插入新数据操作)
    QSqlQuery query;
    query.exec("PRAGMA encoding='UTF-8';");
    query.prepare("select SfzJc from tbl_SfzPinyin where SfzJc = :sSfzJc and Pinyin = :sPinyin");
    query.bindValue(":sSfzJc",sSfzJc);
    query.bindValue(":sPinyin",sPinyin);
    query.exec();
    if (query.exec() != true)
    {
        ////qDebug() << "查询收费站字典表失败，错误代码为: " << query.lastError().text();
        return false;
    }
    if(query.next())
    {
        ////qDebug()<<"The record already exists, do not add up";
        LogMsg("lane","Insert data failed to bind station records");
        return false;
    }

    query.prepare("INSERT INTO tbl_SfzPinyin (SfzJc ,Pinyin) VALUES (?,?)");
    query.addBindValue(sSfzJc);
    query.addBindValue(sPinyin);
    query.exec();
    if (query.exec() != true)
    {
        ////qDebug() << "向收费站字典表中添加一条记录失败，错误代码为: " << query.lastError().text();
        return false;
    }
    //进行数据库的插入操作
    if(query.numRowsAffected()==-1)
    {
        ////qDebug()<<"tbl_SfzPinyin  insert data fail";
        LogMsg("lane","tbl_SfzPinyin insert fail");
        return false;
    }
    else
    {
       LogMsg("lane","tbl_SfzPinyin insert success");
        return true;
    }
}

// 向收费站字典表中添加一条记录
bool QLocalDbCfg ::AddRecToSfzDic(const QSfzRecForManualSelect &Rec)
{
    return AddRecToSfzDic(Rec.sSfzJc, Rec.sPinyin);
}

QLocalDbCfg ::QLocalDbCfg ()
{
    ConnectDB(m_szDbFileName);
}

QLocalDbCfg ::~QLocalDbCfg ()
{
    CloseDB();
}

void QLocalDbCfg::ConnectDB(QString DBname)
{
    m_Db = QSqlDatabase::addDatabase("QSQLITE");
    m_Db.setDatabaseName(DBname);

    if(!QFile().exists(DBname))
    {
        ////qDebug()<<"连接数据库失败,数据库不存在    "+DBname;
        return;
    }

    if(!m_Db.open())
    {
       LogMsg("lane","连接数据库连接失败或者已经连接");
    }
    else
    {
       LogMsg("lane","连接数据库成功");
    }
}


void QLocalDbCfg::CloseDB()
{
    if(m_Db.isOpen())
    {
        m_Db.close();
    }
}

bool QLocalDbCfg::PromptIsConnectDB()
{
    if(!m_Db.isOpen())
    {
        return false;
    }
    return true;
}

QLocalDbCfg* getLocalDbCfg()
{
    static QLocalDbCfg cfg;
    return &cfg;
}








