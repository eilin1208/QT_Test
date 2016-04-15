#ifndef MAINTAINCFG_H
#define MAINTAINCFG_H
#include <QObject>
#include <QSettings>
#include <QList>
#include <QTreeWidgetItem>
struct cfgNode
{
    quint8  code;
    QString cfg_name;
    QString cfg_currentValue;
    QString cfg_defaultValue;
    QString cfg_ModifyValue;
    QString cfg_introduce;
    QString cfg_Path;
};
class MaintainCFG : public QObject
{
    Q_OBJECT

public:
    explicit MaintainCFG(QObject *parent = 0);
    ~MaintainCFG();

public:
    //获取配置文件路径
    static QString getCfgPath();

    //获取程序配置文件内容
    void getCfgInfo(QTreeWidget *widget);

    //获取维护程序配置文件内容到内存
    static void readAllcfg();

    //写配置文件
    static void WriteCfg(QString path,QString value);

    //获取更新文件路径
    static QString getUpdateFilePath();

    //获取更新文件名字
    static QString getUpdateFileName();

    //获取老文件路径
    static QString getOldFilePath();

    //获取用户名密码
    static QString getPwdMd5();

    //获取车道配置文件名字
    static QString getLaneCfgName();

    //
    static int getSoftRole();

    //
    static QString getDBName();

public:
    //设置配置文件路径
    static void setCfgPath(QString path);

    //设置更新文件路径
    static void setUpdateFilePath(QString path);

    //设置更新文件名字
    static void setUpdateFileName(QString name);

    //设置老文件路径
    static void setOldFilePath(QString path);

    //设置用户名密码
    static void setPwdMD5(QString pwd);

    //设置车道配置文件名字
    static void setLaneCfg(QString LaneCfgName);

    //设置栏位名称
    QStringList setColumnName(QString name);

    static void setSoftRole(int role);

    static void setDBName(QString name);

    //判断是否有修改的值
    bool isHaveModify();

    //获取定级节点
    QList<QTreeWidgetItem *> getTopNode();
    QList<QTreeWidgetItem *> listCfgData;
    QList<cfgNode *> listCfgNode;
    QMap<quint16,QString> mapHistory;

private:

    //增加次级节点到顶级结点
    void addNextLevelNodeToList(QTreeWidgetItem *parent
                                ,QString childName
                                ,QString cfgValuePath
                                ,quint8 code
                                ,QString name
                                ,QString defaultValue
                                ,QString introduce);
private:
    //配置文件内容
    //程序配置文件路径
    static QString m_CfgPath;
    static QString m_UpdateFilePath;
    static QString m_UpdateFileName;
    static QString m_OldFilePath;
    static QString m_pwd;
    static QString m_LaneCfgName;
    static QString m_DBName;
    static int m_SoftRole;
    //设置标题
    QStringList m_ColumnTitle;
    QTreeWidget *ui_treeWidget;
};

#endif // MAINTAINCFG_H
