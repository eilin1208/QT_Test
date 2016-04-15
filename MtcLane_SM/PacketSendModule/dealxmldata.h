#ifndef DEALXMLDATA_H
#define DEALXMLDATA_H

#include <QObject>
#include <QDir>
#include <QDomDocument>
#include "CommonModule/MsgDefine.h"
#include <QMutex>
#include <QDateTime>
//存储最早与最新的xml文件的信息
struct xmlFileInfo
{
    QString firstFileName;
    QString newFileName;
    //int dataCount;
    quint64 firstFileSize;
    quint64 newFileSize;
};
class DealXmlData : public QObject
{
    Q_OBJECT
public:
    explicit DealXmlData(QObject *parent = 0);
    //检查xml文件路径,不存在则自动创建
    bool CheckAndCreateXmlDatafolder();
    //检查文件夹是否为空
    bool CheckIsEmpty(bool isSend);
    //创建新的xml文件,以当前时间为文件名,精确到毫秒
    bool CreateNewXmlFile(bool isSend,QString &nfileName);
    //获取最早和最新的xml文件名称,大小
    void getFirstAndNewXmlInfo(bool isSend,xmlFileInfo &xmlInfo);
    //保存tbl_LaneWaste表数据
    bool SaveLaneWasteData(int ID,quint32 staId,int laneId,TDataToSave *pData,bool isSend/*,QString fileName*/);
    //删除tbl_LaneWaste表某条数据
    bool DelLaneWasteData(int ID,bool isSend,QString fileName);
    //删除所有已上传的tbl_LaneWaste数据
    bool DelAllLaneWasteData(bool isSend,int saveDays = 0);
    //检查文件大小,超出上限后自动创建并返回文件名
    void CheckSizeAndGetNewFile(bool isSend,QString &nfileName);
    //获取xml中tbl_LaneWaste数据,用户备份数据恢复
    void GetXmlDataByID(int ID,quint32 &staId,int &LaneId,TDataToSave& pData,bool isSend,QString fileName);
    //处理打不开的xml文件
    bool DealBadXml(QFile &file,bool isSend,QString fileName);
    bool DelBadXml(int saveDays = 0);
public:
    //xmlFileInfo haveSendXmlInfo;
    //xmlFileInfo waitSendXmlInfo;
    //bool useFlag;
    QDateTime CurrentTime;
protected:
private:
    //已发送数据文件路径
    QString waitToSendFilePath;
    //未发送数据文件路径
    QString haveSendFilePath;
    //损坏的数据文件路径
    QString badFilePath;
    //domTree
    QDomDocument m_doc;
    QMutex SavexmlMutex;
    bool hasSD;
    QString sdPath;
signals:

public slots:

};
DealXmlData* getXmlManager();
#endif // DEALXMLDATA_H
