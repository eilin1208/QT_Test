#ifndef LOCALCFGDB_H
#define LOCALCFGDB_H

#pragma once

#include<QObject>
#include<QString>
#include<QVector>
#include<QList>
#include <QSqlDatabase>

class QSfzRecForManualSelect
{
public:
    QString sSfzJc; //收费站简称
    QString sPinyin;//收费站简称的拼音
};

class   QLocalDbCfg
{
public:
    QLocalDbCfg ();
    ~  QLocalDbCfg();
public:


    //*-------------------------------------------------------------
    //功能：连接数据库
    //参数：1、数据库名字
    //返回：无
    //-------------------------------------------------------------*
    void ConnectDB(QString DBname);


    //*-------------------------------------------------------------
    //功能：close数据库
    //参数：1、数据库名字
    //返回：无
    //-------------------------------------------------------------*
    void CloseDB();

    //*-------------------------------------------------------------
    //功能：根据输入拼音查询可选车牌号
    //参数：1.收费站简称的拼音 2.结果集
    //返回：TRUE<查询成功>、FALSE<查询失败>
    //-------------------------------------------------------------*
    bool QueryVehPlateChar(QString szPinyin, QStringList  &ListResult);

    //*-------------------------------------------------------------
    //功能：根据收费站简称查询对应的拼音首字母
    //参数：1.收费站简称 2. 收费站拼音
    //返回：拼音首字母的集合
    //-------------------------------------------------------------*
    bool QuerySfzJcPinyin(const QString &sSfzJc, QString &sSfzJcPinyin);

    //*-------------------------------------------------------------
    //功能：清空收费站字典表
    //参数：无
    //返回：TRUE<清空成功>、FALSE<清空失败>
    //-------------------------------------------------------------*
    bool ClearSfzDic();

    //*-------------------------------------------------------------
    //功能：向收费站字典表中添加一条记录
    //参数：1.收费站简称  2.收费站拼音
    //返回：TRUE<添加成功>、FALSE<添加失败>
    //-------------------------------------------------------------*
    bool AddRecToSfzDic(const QString &sSfzJc, const QString &sPinyin);

    //*-------------------------------------------------------------
    //功能：向收费站字典表中添加一条记录
    //参数：收费站中一条纪录
    //返回：TRUE<添加成功>、FALSE<添加失败>
    //-------------------------------------------------------------*
    bool AddRecToSfzDic(const QSfzRecForManualSelect &Rec);

    //*-------------------------------------------------------------
    //功能：根据输入收费站简称拼音查询符合条件的收费站
    //参数：1.收费站拼音 2.收费站集合
    //返回：TRUE<查询成功>、FALSE<查询失败>
    //-------------------------------------------------------------*
    bool QuerySfzList(const char *szSfzJcPinyin, QStringList  &SfzList);

    // 初始化
    bool Init();

bool PromptIsConnectDB();

protected:
    QSqlDatabase m_Db;
};

QLocalDbCfg* getLocalDbCfg();

#endif // LOCALCFGDB_H
