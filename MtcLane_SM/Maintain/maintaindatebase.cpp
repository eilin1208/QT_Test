#include "maintaindatebase.h"
#include <QSqlQuery>
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QVariant>
MaintainDateBase::MaintainDateBase(QObject *parent) :
    QObject(parent)
{
}

bool MaintainDateBase::connectToDB(const QString &databaseName)
{
    bool result = false;
    if(openDataBase(databaseName))
    {
        result = createTable();
    }
    return result;
}

bool MaintainDateBase::createTable()
{
    bool b1=execSql(CREATE_LOGIN_TABLE);
    bool b2=execSql(CREATE_ANTH_TABLE);
    return b1&&b2;
}

bool MaintainDateBase::openDataBase(const QString &db)
{
    mainTainDB = QSqlDatabase::addDatabase("QSQLITE", "daodaoliang");
    mainTainDB.setDatabaseName(db);
    return mainTainDB.open();
}

const QString & MaintainDateBase::getDateBaseName()
{
    return sDatebaseName;
}

void MaintainDateBase::setDateBaseName(const QString &name)
{
    sDatebaseName=name;
}

QString MaintainDateBase::getErrorMessage()
{
    return message;
}

bool MaintainDateBase::InsertData(const QString& ip
                                  ,const QString& user
                                  ,const QString& password)
{
    QString execString = tr("insert into tb_login values('%1','%2','%3');")
            .arg(ip)
            .arg(user)
            .arg(password);
    return execSql(execString);
}

void MaintainDateBase::close()
{
    mainTainDB.close();
}

bool MaintainDateBase::execSql(const QString &sql)
{
    mainTainDB.exec(sql);
    bool res = mainTainDB.lastError().type() == QSqlError::NoError;
    if(!res)
    {
        qDebug() << "data base error:" << mainTainDB.lastError().text();
    }
    return res;
}

QList<LoginNode> MaintainDateBase::selectData()
{
    listNode.clear();
    QString execString=tr("select * from tb_login");
    QSqlQuery tempQuery=mainTainDB.exec(execString);
    while(tempQuery.next())
    {
        LoginNode tempNode;
        tempNode.ip=tempQuery.value(0).toString();
        tempNode.user=tempQuery.value(1).toString();
        tempNode.passWord=tempQuery.value(2).toString();
        listNode.append(tempNode);
    }
    return listNode;
}

bool MaintainDateBase::delIpData(const QString &ip)
{
    QString execString=tr("delete from tb_login where ip='%1';").arg(ip);
    QSqlQuery tempQuery=mainTainDB.exec(execString);
    if(tempQuery.lastError().type()==QSqlError::NoError)
    {
        return true;
    }
    return false;
}

bool MaintainDateBase::InsertDataToAnth(const QString &user, const QString &MD5)
{
    QString execString = tr("insert into tb_anth values('%1','%2');")
            .arg(user)
            .arg(MD5);
    return execSql(execString);
}

QList<AuthNode> MaintainDateBase::selectDataFromTbAnth()
{
    listAuthNode.clear();
    QString execString=tr("select * from tb_anth");
    QSqlQuery tempQuery=mainTainDB.exec(execString);
    while(tempQuery.next())
    {
        AuthNode tempNode;
        tempNode.user=tempQuery.value(0).toString();
        tempNode.MD5=tempQuery.value(1).toString();
        listAuthNode.append(tempNode);
    }
    return listAuthNode;
}

bool MaintainDateBase::delUserData(const QString &user)
{
    QString execString=tr("delete from tb_anth where user='%1';").arg(user);
    QSqlQuery tempQuery=mainTainDB.exec(execString);
    if(tempQuery.lastError().type()==QSqlError::NoError)
    {
        return true;
    }
    return false;
}
MaintainDateBase *getMaintainDB()
{
    static MaintainDateBase myDB;
    return &myDB;
}
