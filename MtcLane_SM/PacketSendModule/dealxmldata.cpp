#include "dealxmldata.h"
#include "CommonModule/ilogmsg.h"
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QDataStream>
#include "sqlitedatasaver.h"
#include <QSettings>
DealXmlData::DealXmlData(QObject *parent) :
    QObject(parent)
{
    QSettings tempSettings("./Lane.ini",QSettings::IniFormat);
    QString sGroupName = "LaneDBSrv";
    QString tmpStr=tempSettings.value(sGroupName + "/XMLPath").toString();
    QDir dir;
    if(dir.exists(tmpStr))
    {
        hasSD = true;
        sdPath = tmpStr;
        haveSendFilePath = tmpStr + "/xmlbk/sendxml";
        badFilePath = tmpStr + "/xmlbk/badxml";
        waitToSendFilePath = tmpStr + "/xmlbk/waitxml";
    }
    else
    {
        sdPath = "";
        hasSD = false;
    }
    CurrentTime = QDateTime::currentDateTime();
    m_doc.clear();
}

bool DealXmlData::CheckAndCreateXmlDatafolder()
{
    QDir dir;
    //程序启动前没有插入SD卡或者在程序运行过程中拔掉
    if(!hasSD||!dir.exists(sdPath))
    {
        LogMsg("lane",tr("SD不存在,系统不会进行数据备份"));
        return false;
    }
    if(!dir.exists(waitToSendFilePath))
    {
        if(dir.mkpath(waitToSendFilePath))
            LogMsg("lane",tr("待发送xml文件目录不存在,系统自动创建"));
        else
        {
            LogMsg("lane",tr("待发送xml文件目录不存在,系统自动创建失败!"));
            return false;
        }
    }
    if(!dir.exists(haveSendFilePath))
    {
        if(dir.mkpath(haveSendFilePath))
            LogMsg("lane",tr("已发送xml文件目录不存在,系统自动创建"));
        else
        {
            LogMsg("lane",tr("已发送xml文件目录不存在,系统自动创建失败!"));
            return false;
        }
    }
    if(!dir.exists(badFilePath))
    {
        if(dir.mkpath(badFilePath))
            LogMsg("lane",tr("损坏xml文件目录不存在,系统自动创建"));
        else
        {
            LogMsg("lane",tr("损坏xml文件目录不存在,系统自动创建失败!"));
            return false;
        }
    }
    return true;
}

bool DealXmlData::CreateNewXmlFile(bool isSend,QString &nfileName)
{
    //QString fileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    m_doc.clear();
    QString fileName = CurrentTime.toString("yyyyMMddhhmmsszzz");
    nfileName = fileName+".xml";
    QFile nFile;
    if(isSend)
    {
        nFile.setFileName(haveSendFilePath+"/"+fileName+".xml");
        if(!nFile.open(QIODevice::ReadWrite))
        {
            LogMsg("lane",tr("xml文件创建失败"));
            return false;
        }
    }else
    {
        nFile.setFileName(waitToSendFilePath+"/"+fileName+".xml");
        if(!nFile.open(QIODevice::ReadWrite))
        {
            LogMsg("lane",tr("xml文件创建失败"));
            return false;
        }
    }
    nFile.close();
    QDomProcessingInstruction ins = m_doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    m_doc.appendChild(ins);
    QDomElement root = m_doc.createElement("tbl_LaneWaste");
    m_doc.appendChild(root);
    if(!nFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
        return false;
    QTextStream out(&nFile);
    m_doc.save(out,4);
    nFile.close();
    nfileName = fileName+".xml";
    LogMsg("lane",tr("xml备份文件%1.xml创建成功").arg(fileName));
    //
    m_doc.clear();
    return true;
}

void DealXmlData::getFirstAndNewXmlInfo(bool isSend,xmlFileInfo &xmlInfo)
{
    QDir fileDir;
    QStringList fileList;
    QString tmpFStr = "";
    quint64 tmpFsize = 0;
    QString tmpNStr = "";
    quint64 tmpNsize = 0;
    if(isSend)
    {
        fileDir.setPath(haveSendFilePath);
    }
    else
    {
        fileDir.setPath(waitToSendFilePath);
    }
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(fileList));
    for(int i = 0;i<= fileInfo->count() -1;i++)
    {
        if(fileInfo->at(i).isFile())
        {
            if(tmpFStr =="" && tmpNStr == "")
            {
                tmpFStr = fileInfo->at(i).fileName();
                tmpNStr = fileInfo->at(i).fileName();
                tmpFsize = fileInfo->at(i).size();
                tmpNsize = fileInfo->at(i).size();
            }
            else
            {
                if(tmpFStr.left(14).toLongLong() >= fileInfo->at(i).fileName().left(14).toLongLong())
                {
                    tmpFStr = fileInfo->at(i).fileName();
                    tmpFsize = fileInfo->at(i).size();
                }
                else
                {
                    if(tmpNStr.left(14).toLongLong() <= fileInfo->at(i).fileName().left(14).toLongLong())
                    {
                        tmpNStr = fileInfo->at(i).fileName();
                        tmpNsize = fileInfo->at(i).size();
                    }
                }
            }
        }
    }
    xmlInfo.firstFileName = tmpFStr;
    xmlInfo.firstFileSize = tmpFsize;
    xmlInfo.newFileName = tmpNStr;
    xmlInfo.newFileSize = tmpNsize;
    delete fileInfo;
}
bool DealXmlData::CheckIsEmpty(bool isSend)
{
    QDir fileDir;
    QStringList fileList;
    int fileCount = 0;
    if(isSend)
    {
        fileDir.setPath(haveSendFilePath);
    }
    else
    {
        fileDir.setPath(waitToSendFilePath);
    }
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(fileList));
    for(int i = 0;i<= fileInfo->count() -1;i++)
    {
        if(fileInfo->at(i).isFile())
        {
            fileCount = fileCount + 1;
            break;
        }
    }
    delete fileInfo;
    return fileCount > 0;
}

bool DealXmlData::SaveLaneWasteData(int ID,quint32 staID,int laneID,TDataToSave *pData, bool isSend/*,QString fileName*/)
{
    //static QMutex xmlMutex;
    QMutexLocker locker(&SavexmlMutex);
    QDir dir;
    if(!hasSD||!dir.exists(sdPath))
    {
        LogMsg("lane",tr("SD不存在,系统不会进行数据备份"));
        return false;
    }
    QString xmlErro;
    int index,column;
    QFile file;
    QString fileName;
    //已上传数据,先删除原数据
    if(isSend)
    {
        xmlFileInfo fileInfo;
        getFirstAndNewXmlInfo(false,fileInfo);
        LogMsg("lane",tr("开始删除xml数据 ID:%1").arg(ID));
        DelLaneWasteData(ID,false,fileInfo.firstFileName);
        LogMsg("lane",tr("删除xml数据完成 ID:%1").arg(ID));
        pData->TransFlag1 = 1;
    }
    if(!CheckIsEmpty(isSend))
        CreateNewXmlFile(isSend,fileName);
    else
        CheckSizeAndGetNewFile(isSend,fileName);
    //
    if(isSend)
    {
        file.setFileName(haveSendFilePath+"/"+fileName);
    }else
    {
        file.setFileName(waitToSendFilePath+"/"+fileName);
    }
    if(!file.exists())
    {
        LogMsg("lane",tr("xml数据文件不存在,保存失败"));
        return false;
    }
    if(file.isOpen())
    {
        file.close();
    }
    m_doc.clear();
    if(!m_doc.setContent(&file,false,&xmlErro,&index,&column))
    {
        //        LogMsg("lane",tr("xml文件加载失败,备份失败"));
        //        return false;
        m_doc.clear();
        LogMsg("lane",tr("开始处理处理损坏xml文件%1").arg(fileName));
        if(!DealBadXml(file,isSend,fileName))
        {
            LogMsg("lane",tr("处理损坏xml文件%1失败").arg(fileName));
            return false;
        }
    }
    file.close();
    //获取根节点
    QDomElement root = m_doc.documentElement();
    QDomElement data = m_doc.createElement("Data");
    //创建节点
    QDomElement id = m_doc.createElement("ID");
    QDomElement staId = m_doc.createElement("StaId");
    QDomElement landId = m_doc.createElement("LaneId");
    QDomElement occurTime = m_doc.createElement("OccurTime");
    QDomElement msgId = m_doc.createElement("MsgID");
    QDomElement msgType = m_doc.createElement("MsgType");
    QDomElement msgLen = m_doc.createElement("MsgLen");
    QDomElement msg = m_doc.createElement("Msg");
    QDomElement transFlag1 = m_doc.createElement("TransFlag1");
    QDomElement transFlag2 = m_doc.createElement("TransFlag2");
    QDomElement transFlag3 = m_doc.createElement("TransFlag3");
    QDomElement transFlag4 = m_doc.createElement("TransFlag4");
    root.appendChild(data);
    data.appendChild(id);
    data.appendChild(staId);
    data.appendChild(landId);
    data.appendChild(occurTime);
    data.appendChild(msgId);
    data.appendChild(msgType);
    data.appendChild(msgLen);
    data.appendChild(msg);
    data.appendChild(transFlag1);
    data.appendChild(transFlag2);
    data.appendChild(transFlag3);
    data.appendChild(transFlag4);
    //节点赋值
    QDomText id_v = m_doc.createTextNode(QString::number(ID));
    QDomText staId_v = m_doc.createTextNode(QString::number(staID));
    QDomText laneId_v = m_doc.createTextNode(QString::number(laneID));
    QDomText occurTime_v = m_doc.createTextNode(QString::number(pData->OccurTime));
    QDomText msgId_v = m_doc.createTextNode(QString::number(pData->MsgID));
    QDomText msgType_v = m_doc.createTextNode(QString::number(pData->MsgType));
    QDomText msgLen_v = m_doc.createTextNode(QString::number(pData->nMsgLen));
    QByteArray m = QByteArray::fromRawData((char*)pData->pMsg,pData->nMsgLen).toHex();
    //QDomText msg_v = m_doc.createTextNode(QString::fromLocal8Bit(m));
    QDomText msg_v = m_doc.createTextNode(m);
    QDomText transFlag1_v = m_doc.createTextNode(QString::number(pData->TransFlag1));
    QDomText transFlag2_v = m_doc.createTextNode(QString::number(pData->TransFlag2));
    QDomText transFlag3_v = m_doc.createTextNode(QString::number(pData->TransFlag3));
    QDomText transFlag4_v = m_doc.createTextNode(QString::number(pData->TransFlag4));
    id.appendChild(id_v);
    staId.appendChild(staId_v);
    landId.appendChild(laneId_v);
    occurTime.appendChild(occurTime_v);
    msgId.appendChild(msgId_v);
    msgType.appendChild(msgType_v);
    msgLen.appendChild(msgLen_v);
    msg.appendChild(msg_v);
    transFlag1.appendChild(transFlag1_v);
    transFlag2.appendChild(transFlag2_v);
    transFlag3.appendChild(transFlag3_v);
    transFlag4.appendChild(transFlag4_v);
    //保存
    QTextStream out(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
        LogMsg("lane",tr("保存时打开xml文件失败,保存失败"));
        return false;
    }
    m_doc.save(out,4);
    file.close();
    LogMsg("lane",tr("xml备份一条数据成功,ID：%1").arg(ID));
    m_doc.clear();
    return true;
}

bool DealXmlData::DelLaneWasteData(int ID, bool isSend,QString fileName)
{

    QString xmlErro;
    int index,column;
    QFile file;

    QDomNode firstNode;
    QDomNode node;
    QDomText nodeText;
    if(isSend)
    {
        file.setFileName(haveSendFilePath+"/"+fileName);
    }else
    {
        file.setFileName(waitToSendFilePath+"/"+fileName);
    }
    if(!file.exists())
    {
        LogMsg("lane",tr("xml数据文件不存在,删除失败"));
        return false;
    }
    if(file.isOpen())
    {
        file.close();
    }
    m_doc.clear();
    if(!m_doc.setContent(&file,false,&xmlErro,&index,&column))
    {
        //        LogMsg("lane",tr("xml文件加载失败,备份失败"));
        //        return false;
        m_doc.clear();
        LogMsg("lane",tr("开始处理处理损坏xml文件%1").arg(fileName));
        if(!DealBadXml(file,isSend,fileName))
        {
            LogMsg("lane",tr("处理损坏xml文件%1失败").arg(fileName));
            return false;
        }
    }
    file.close();
    int count = 0;
    QDomElement root = m_doc.documentElement();
    QDomNodeList list = root.childNodes();
    for(int i = 0;i < list.count();i++)
    {
        node = list.at(i);
        if(node.isElement())
        {
            firstNode = node.firstChild();
            nodeText = firstNode.firstChild().toText();
            if(nodeText.data().toInt()== ID)
            {
                count++;
                root.removeChild(node);
                //保存
                QTextStream out(&file);
                if(!file.open(QIODevice::ReadWrite|QIODevice::Truncate))
                {
                    LogMsg("lane",tr("删除时打开xml文件失败,删除失败"));
                    return false;
                }
                m_doc.save(out,4);
                //file.close();
                m_doc.clear();
                //数据为空时删除文件
                if(list.count() == 0)
                {
                    file.remove();
                }
                break;
            }
        }
    }
    file.close();
    if(count == 0)
        LogMsg("lane",tr("xmlDelLaneWasteData删除时未找到数据,ID：%1").arg(ID));
    else
        LogMsg("lane",tr("xmlDelLaneWasteData删除一条数据成功,ID：%1").arg(ID));
    return true;
}
bool DealXmlData::DelAllLaneWasteData(bool isSend,int saveDays)
{
    QDir dir;
    if(!hasSD||!dir.exists(sdPath))
    {
        LogMsg("lane",tr("SD不存在,系统不会进行数据备份"));
        return false;
    }
    LogMsg("lane",tr("DelAllLaneWasteData:开始"));
    QDir fileDir;
    QStringList flist;
    QFile file;
    if(isSend)
    {
        fileDir.setPath(haveSendFilePath);
    }
    else
    {
        fileDir.setPath(waitToSendFilePath);
    }
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(flist));
    //    if(saveDays == 0)
    //    {
    //        for(int i = 0;i<fileInfo->count();i++)
    //        {
    //            if(fileInfo->at(i).isFile())
    //            {
    //                file.setFileName(fileDir.path()+"/"+fileInfo->at(i).fileName());
    //                if(file.isOpen())
    //                    continue;
    //                if(file.remove())
    //                    LogMsg("lane",tr("DelAllLaneWasteData:xml备份删除一个备份文件成功,filename：%1").arg(fileInfo->at(i).fileName()));
    //            }
    //        }
    //    }else
    //    {
    QDateTime tmpTime = CurrentTime.addDays(-saveDays);
    tmpTime.setTime(QTime(23,59,59,999));
    QString tmpName = tmpTime.toString("yyyyMMdd");
    for(int i = 0;i<fileInfo->count();i++)
    {
        if(fileInfo->at(i).isFile() && fileInfo->at(i).fileName().left(8).toInt() <= tmpName.toInt())
        {
            file.setFileName(fileDir.path()+"/"+fileInfo->at(i).fileName());
            if(file.isOpen())
                continue;
            if(file.remove())
                LogMsg("lane",tr("DelAllLaneWasteData:删除一个已上传xml文件成功,filename：%1").arg(fileInfo->at(i).fileName()));
        }
    }
    //}
    delete fileInfo;
    LogMsg("lane",tr("DelAllLaneWasteData:结束"));
    return true;
}
void DealXmlData::CheckSizeAndGetNewFile(bool isSend,QString &nfileName)
{
    xmlFileInfo fileInfo;
    getFirstAndNewXmlInfo(isSend,fileInfo);
    //文件大小设定为3m
    if(fileInfo.newFileSize < 1024*1000*2 && fileInfo.newFileName.left(8) == CurrentTime.toString("yyyyMMdd"))
    {
        nfileName = fileInfo.newFileName;
    }else
    {
        CreateNewXmlFile(isSend,nfileName);
    }
}

void DealXmlData::GetXmlDataByID(int ID, quint32 &staId, int &LaneId, TDataToSave &pData, bool isSend, QString fileName)
{
    QString xmlErro;
    int index,column;
    QFile file;
    QDomNode node;
    QDomNode first;
    if(isSend)
    {
        file.setFileName(haveSendFilePath+"/"+fileName);
    }else
    {
        file.setFileName(waitToSendFilePath+"/"+fileName);
    }
    if(!file.exists())
    {
        LogMsg("lane",tr("xml数据文件不存在,获取备份数据失败"));
        return;
    }
    if(file.isOpen())
    {
        file.close();
    }
    if(!m_doc.setContent(&file,false,&xmlErro,&index,&column))
    {
        m_doc.clear();
        LogMsg("lane",tr("开始处理处理损坏xml文件%1").arg(fileName));
        if(!DealBadXml(file,isSend,fileName))
        {
            LogMsg("lane",tr("处理损坏xml文件%1失败").arg(fileName));
            return;
        }
    }
    file.close();
    QDomElement root = m_doc.documentElement();
    QDomNodeList list = root.childNodes();
    for(int i=0;i<list.count();i++)
    {
        //DataNode
        node = list.at(i);
        if(node.isElement())
        {
            //IDNode
            first = node.firstChild();
            if(first.firstChild().toText().data().toInt() == ID)
            {
                QDomNodeList nlist = node.childNodes();
                staId = nlist.at(1).firstChild().toText().data().toInt();
                LaneId = nlist.at(2).firstChild().toText().data().toInt();
                pData.OccurTime = nlist.at(3).firstChild().nodeValue().toLongLong();
                pData.MsgID = nlist.at(4).firstChild().toText().data().toInt();
                pData.MsgType = nlist.at(5).firstChild().toText().data().toInt();
                pData.nMsgLen = nlist.at(6).firstChild().toText().data().toInt();
                QByteArray m = nlist.at(7).firstChild().nodeValue().toLocal8Bit();
                QByteArray mm = QByteArray::fromHex(m);
                pData.pMsg = mm.data();
                pData.TransFlag1 = nlist.at(8).firstChild().toText().data().toInt();
                pData.TransFlag2 = nlist.at(9).firstChild().toText().data().toInt();
                pData.TransFlag3 = nlist.at(10).firstChild().toText().data().toInt();
                pData.TransFlag3 = nlist.at(11).firstChild().toText().data().toInt();
                break;
            }
        }
    }
    m_doc.clear();
    //
    file.close();
}
DealXmlData* getXmlManager()
{    
    static QMutex xmlMutex;
    QMutexLocker locker(&xmlMutex);
    static DealXmlData xmldata;
    xmldata.CurrentTime = QDateTime::currentDateTime();
    return &xmldata;
}

bool DealXmlData::DealBadXml(QFile &file,bool isSend,QString fileName)
{
    LogMsg("lane",tr("DealBadXml:开始"));
    QString xmlErro;
    int index,column;
    if(file.isOpen())
        file.close();
    if(isSend)
    {
        LogMsg("lane",tr("已上传xml文件%1损坏,移动").arg(file.fileName()));
        file.copy(badFilePath+"/"+fileName.insert(17,"1"));
        file.remove();
    }
    else
    {
        LogMsg("lane",tr("xml文件%1损坏,移动%2").arg(file.fileName()).arg(badFilePath));
        file.copy(badFilePath+"/"+fileName.insert(17,"0"));
        file.remove();
    }
    CreateNewXmlFile(isSend,fileName);
    if(isSend)
    {
        file.setFileName(haveSendFilePath+"/"+fileName);
    }else
    {
        file.setFileName(waitToSendFilePath+"/"+fileName);
    }
    if(!m_doc.setContent(&file,false,&xmlErro,&index,&column))
    {
        LogMsg("lane",tr("xml文件%1加载失败,备份失败").arg(file.fileName()));
        return false;
    }
    LogMsg("lane",tr("DealBadXml:开始"));
    return true;
}

bool DealXmlData::DelBadXml(int saveDays)
{
    LogMsg("lane",tr("DelBadXml:开始"));
    QDir dir;
    if(!hasSD||!dir.exists(sdPath))
    {
        LogMsg("lane",tr("SD不存在,系统不会进行数据备份"));
        return false;
    }
    LogMsg("lane",tr("DelBadXml:开始"));
    QDir fileDir;
    QStringList flist;
    QFile file;
    fileDir.setPath(badFilePath);
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(flist));
    //    if(saveDays == 0)
    //    {
    //        for(int i = 0;i<fileInfo->count();i++)

    //        {
    //            if(fileInfo->at(i).isFile())
    //            {
    //                file.setFileName(fileDir.path()+"/"+fileInfo->at(i).fileName());
    //                if(file.isOpen())
    //                    continue;
    //                if(file.remove())
    //                    LogMsg("lane",tr("DelBadXml:xml备份删除一个备份损坏文件成功,filename：%1").arg(fileInfo->at(i).fileName()));
    //            }
    //        }
    //        LogMsg("lane",tr("DelBadXml:结束"));
    //    }
    //    else
    //    {
    QDateTime tmpTime = CurrentTime.addDays(-saveDays);
    tmpTime.setTime(QTime(23,59,59,999));
    QString tmpName = tmpTime.toString("yyyyMMdd");
    for(int i = 0;i<fileInfo->count();i++)
    {
        if(fileInfo->at(i).isFile() && fileInfo->at(i).fileName().left(8).toInt() <= tmpName.toInt())
        {
            file.setFileName(fileDir.path()+"/"+fileInfo->at(i).fileName());
            if(file.isOpen())
                continue;
            if(file.remove())
                LogMsg("lane",tr("DelBadXml:删除一个一个备份损坏文件成功,filename：%1").arg(fileInfo->at(i).fileName()));
        }
    }
    LogMsg("lane",tr("DelBadXml:结束"));
    //}
    delete fileInfo;
    return true;
}
