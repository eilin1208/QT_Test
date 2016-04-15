/*
  此类主要用于处理票据信息管理，和卡盒信息管理
  */

#ifndef DATAFILEMGR_H
#define DATAFILEMGR_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
//票据信息
struct SInvInfo
{
    qint64 PreInvBeginNo; //票据修改前起号
    quint32 PreInvCount; //票据修改前张数
    qint64 InvBeginNo; //票据号码
    quint32 InvCount; //发票张数
    qint64 InvEndNO;   //票据号码
};

struct SCardBoxInfo
{
    quint8 CardBoxIndex;
    quint32 FlagCardID;
    quint32 CardBoxID;
    quint16 nCardCount;
};
struct FileParamInfo
{
    QString sTableName;
    QString sCreateSql;
    QString sDisplayName;
};
enum emFileType
{
    emCardBox,
    emInvoice
};
class DataFileMgr : public QObject
{
    Q_OBJECT
public:
    explicit DataFileMgr(QObject *parent = 0);
    bool Init();
    void Close();
    //打印票据
    bool IncInvoice();
    //换上票据
    bool UpInvoice(qint64 nInvBeginNo, qint64 nInvEndNo, int nInvCount);
    //换下票据
    bool DownInvoice();
    //更改票号
    bool ChangeStartNo(qint64 nInvBeginNo, int nInvCount);
    //返回票据信息
    SInvInfo GetInvInfo();
    /****卡盒操作****/
    //是否有卡盒
    bool IsLoaded();
    //收卡
    void AddOneCard(quint32 dwCardId = 0);
    //发卡
    void DeleteOneCard(quint32 dwCardId = 0);
    //装盒
    bool LoadCardBox(quint32 nCardBoxNo, quint32 nFlagNo, quint16 nCount);
    //卸下卡盒
    bool UnLoadCardBox(int nOperType);
    //更新界面
    void UpdateWidgetData();
    SCardBoxInfo getCardBoxInfo()
    {
        return m_cardBox;
    }
    //获取票号信息
    //加载卡盒信息
    bool LoadCardBoxFromFile();
    //加载票据信息
    bool LoadInvFromFile();
signals:

public slots:

protected:
    //保存卡盒信息
    bool SaveCardBoxInfo();
    //保存票据信息
    bool SaveInvInfo();

private:
    //获取记录条数
    int GetRecordCount(QSqlQuery &query);
private:
    QSqlDatabase m_dataFile;
    SInvInfo m_invoiceInfo;
    QVector< FileParamInfo > m_fileInfo;
    SCardBoxInfo m_cardBox;
};

DataFileMgr * getDataFileMgr();

#endif // DATAFILEMGR_H
