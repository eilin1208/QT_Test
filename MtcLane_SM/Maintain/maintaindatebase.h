#ifndef MAINTAINDATEBASE_H
#define MAINTAINDATEBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QMetaType>
//建表语句
const QString CREATE_LOGIN_TABLE=
"create table if not exists 'tb_login'"\
"([ip] text NOT NULL PRIMARY KEY,"\
"[user] text not null,"
"[passWord] text not null);";
const QString CREATE_ANTH_TABLE=
"create table if not exists 'tb_anth'"\
"([user] text NOT NULL PRIMARY KEY,"\
"[MD5] text not null);";
struct LoginNode
{
    QString ip;
    QString user;
    QString passWord;
};
Q_DECLARE_METATYPE(LoginNode)
struct AuthNode
{
    QString user;
    QString MD5;
};
class MaintainDateBase : public QObject
{
    Q_OBJECT
public:
    explicit MaintainDateBase(QObject *parent = 0);

public:
    //设置数据库的名字
    void setDateBaseName(const QString & name);
    //获取数据库的名字
    const QString& getDateBaseName();
    //数据库连接
    bool connectToDB(const QString& databaseName);
    //获取错误描述
    QString getErrorMessage();
    //插入数据到tb_login表
    bool InsertData(const QString& ip
                    ,const QString& user
                    ,const QString& password);
    //插入数据到tb_anth表
    bool InsertDataToAnth(const QString &user,
                    const QString &MD5);
    //获取数据从表tb_login
    QList<LoginNode> selectData();
    //获取数据从表tb_auth
    QList<AuthNode> selectDataFromTbAnth();
    //删除tb_login指定ip的数据
    bool delIpData(const QString &ip);
    //删除tb_auth指定的数据
    bool delUserData(const QString &user);
    //关闭数据库
    void close();
private:
    //创建数据库表
    bool createTable();
    //查看数据库是否存在，不存在的话创建
    bool openDataBase(const QString &db);
    //执行语句
    bool execSql(const QString& sql);
private:
    QString sDatebaseName;
    QSqlDatabase mainTainDB;
    QMutex mutex;
    QString message;
    QList<LoginNode> listNode;
    QList<AuthNode> listAuthNode;
};
MaintainDateBase *getMaintainDB();
#endif // MAINTAINDATEBASE_H
