#ifndef BASESAVER_H
#define BASESAVER_H

#include "MsgDefine.h"
#include <QObject>
const int  MaxAppFileNameLen = 128;
enum TDispatchType
{
    dtSocket,
    dtSqlServer,
    dtReserve1,
    dtReserve2
};
class BaseSaver : public QObject
{
    Q_OBJECT
public:
    explicit BaseSaver(QObject *parent = 0);
    //数据库初始化，包括数据库的连接和打开
    virtual bool Init(quint32 StaId, int LaneId) = 0;
    //保存数据
    virtual bool SaveData(quint64 &AID,TDataToSave* PDate,bool RemoveTmpData = false,
                          void* PSynData = NULL,int SynDataLen = 0) = 0;
    //保存临时数据
    virtual bool SaveTempData(TDataToSave* PDate, void* PSynData = NULL,int SynDataLen = 0) = 0;
    //取临时数据
    virtual bool GetTempData(TDataToSave* PData) = 0;
    //删临时数据
    virtual bool RemoveTempData() = 0;
    //取同步数据
    virtual bool GetSynData(void* PDate,int &SynDataLen) = 0;
    //删同步数据
    virtual bool RemoveSynData() = 0;
    //取第N条要发送的数据
    virtual bool GetFirstData(const quint64 AID,TDataToSave* PData,TDispatchType DispatchType,int OrderNum = 1) = 0;
    //取下一条数据
    virtual bool GetNextData(quint64 &AID,TDataToSave* PData) = 0;
    //删除指定id的数据
    virtual bool  RemoveData(quint64 AID) = 0;
    //删除历史数据
    virtual bool DelHistoryData(quint16 SaveDays) = 0;
    //标记数据的状态
    virtual bool MarkDataState(quint64 AID,TDispatchType DispatchType) = 0;
    //获取未发送的数据条数
    virtual int GetUnDispatchCount(TDispatchType DispatchType) = 0;
    //获取全部数据条数
    virtual int GetTotalCount() = 0;
    //获取指定条数的全部数据
    virtual bool GetDataByCount(QList<QByteArray*>& dataList, int nBegin, int nCount, bool bUnDispatch = false) = 0;
protected:
    //收费站编号
    quint32 FStaID;
    //车道编号
    int FLaneID;
    //是否有临时数据
    bool FHasTempData;
signals:

public slots:

};

#endif // BASESERVER_H
