#include "datafilemgr.h"
#include "CommonModule/ilogmsg.h"
#include "laneinfo.h"
#include <QStringList>
#include <QSqlRecord>
#include <QVariant>
#include "CommonModule/LaneType.h"
#include "formmtctitle.h"
#include "formmainwindow.h"
DataFileMgr::DataFileMgr(QObject *parent) :
    QObject(parent)
{
    FileParamInfo tempInfo;
    tempInfo.sDisplayName = tr("卡盒字典表");
    tempInfo.sTableName = tr("tbl_CardBoxDic");
    tempInfo.sCreateSql = "CREATE TABLE "+tempInfo.sTableName+
            "([ID] INTEGER PRIMARY KEY not null,"+
            "LaneID INTEGER  not null,"+
            "FlagCardID INTEGER not null,"+
            "CardBoxID INTEGER not null,"+
            "CardBoxCount INTEGER not null);";
    m_fileInfo.append(tempInfo);
    tempInfo.sDisplayName = tr("票据字典表");
    tempInfo.sTableName = tr("tbl_InvoiceDic");
    tempInfo.sCreateSql = "CREATE TABLE "+tempInfo.sTableName+
            "([ID] INTEGER PRIMARY KEY not null,"+
            "LaneID INTEGER  not null,"+
            "InvoiceBeginNo Integer not null,"+
            "InVoiceCount INTEGER not null ,"+
            "InvoiceEndNo Integer not null);";
    m_fileInfo.append(tempInfo);
    memset(&m_invoiceInfo, 0, sizeof(SInvInfo));
    memset(&m_cardBox,0, sizeof(SCardBoxInfo));
}

bool DataFileMgr::Init()
{
    m_dataFile = QSqlDatabase::addDatabase("QSQLITE", "DATAFILECONNECTION");
    m_dataFile.setDatabaseName("./FileAnnal.sav");
    if(!m_dataFile.open())
    {
        LogMsg("lane", tr("数据库[%1]打开失败").arg("./FileAnnal.sav"));
        return false;
    }
    else
    {
        //如果不存在表则重新创建
        foreach(FileParamInfo tempFilInfo, m_fileInfo)
        {
            if(!m_dataFile.tables().contains(tempFilInfo.sTableName))
            {
                m_dataFile.exec(tempFilInfo.sCreateSql);
            }
        }
    }
    LoadInvFromFile();
    LoadCardBoxFromFile();
    return true;
}

void DataFileMgr::Close()
{
    m_dataFile.close();
}

bool DataFileMgr::IncInvoice()
{
    //判断是否正在上班
    //qDebug() << "get shift State" << (int)getLaneInfo()->getShiftState();
    if(getLaneInfo()->getShiftState() != lsNormalWorking)
    {
        return false;
    }

    //起号等于止号   票据用完，清零
    //否则起始票号加1，张数减1
    LogMsg("lane", tr("打印票据，当前票号:[%1],当前张数[%2]").arg(m_invoiceInfo.InvBeginNo).arg(m_invoiceInfo.InvCount));
    m_invoiceInfo.PreInvBeginNo = m_invoiceInfo.InvBeginNo;
    m_invoiceInfo.PreInvCount = m_invoiceInfo.InvEndNO;
    if(m_invoiceInfo.InvBeginNo == m_invoiceInfo.InvEndNO)
    {
        m_invoiceInfo.InvBeginNo = 0;
        m_invoiceInfo.InvEndNO = 0;
        m_invoiceInfo.InvCount = 0;
    }
    else
    {
        m_invoiceInfo.InvBeginNo++;
        m_invoiceInfo.InvCount--;
    }
    SaveInvInfo();
    return true;
}

bool DataFileMgr::SaveInvInfo()
{
    QSqlQuery query("select * from " + m_fileInfo[emInvoice].sTableName, m_dataFile);
    if(!query.exec())
    {
        LogMsg("lane", "查询" + m_fileInfo[emInvoice].sDisplayName +"失败");
        return false;
    }
    int recordCount = GetRecordCount(query);
    if(recordCount > 1)
    {
        LogMsg("lane", tr("票据记录中数据多于一条,数据非法删除该数据!"));
        query.exec("delete from " + m_fileInfo[emInvoice].sTableName);
    }
    if(recordCount <= 0)
    {
        LogMsg("lane", tr("插入新的票据字典:新票据起号(%1),新票据止号(%2),票据张数(%3)")
               .arg(m_invoiceInfo.InvBeginNo).arg(m_invoiceInfo.InvEndNO).arg(m_invoiceInfo.InvCount));
        QString sql = "insert into " + m_fileInfo[emInvoice].sTableName +
                         " (LaneID,InvoiceBeginNo,InVoiceCount,InvoiceEndNo) VALUES (" +
                         tr("%1, %2, %3, %4);").arg(getLaneInfo()->getLaneCode())
                         .arg(m_invoiceInfo.InvBeginNo).arg(m_invoiceInfo.InvCount).arg(m_invoiceInfo.InvCount);
        query.exec(sql);
        LogMsg("lane", sql);
    }
    if(recordCount == 1)
    {
        LogMsg("lane", tr("更新票据记录:票据起号(%1),票据止号(%2),票据张数(%3)")
               .arg(m_invoiceInfo.InvBeginNo).arg(m_invoiceInfo.InvEndNO).arg(m_invoiceInfo.InvCount));
        QString sql = "update " + m_fileInfo[emInvoice].sTableName +
                         tr(" set InvoiceBeginNo=%1,").arg(m_invoiceInfo.InvBeginNo) +
                         tr("InVoiceCount=%1,").arg(m_invoiceInfo.InvCount) +
                         tr("InvoiceEndNo=%1").arg(m_invoiceInfo.InvEndNO) +
                         " where ID = 1 and " +
                         tr("laneid=%1").arg(getLaneInfo()->getLaneCode());
        query.exec(sql);
        LogMsg("lane", sql);
    }

    //修改页面
    UpdateWidgetData();
    return true;
}

bool DataFileMgr::UpInvoice(qint64 nInvBeginNo, qint64 nInvEndNo, int nInvCount)
{
    m_invoiceInfo.PreInvBeginNo = nInvBeginNo;
    m_invoiceInfo.PreInvCount = nInvCount;
    m_invoiceInfo.InvBeginNo = nInvBeginNo;
    m_invoiceInfo.InvCount = nInvCount;
    m_invoiceInfo.InvEndNO = nInvEndNo;
    SaveInvInfo();
    return true;
}

bool DataFileMgr::DownInvoice()
{
    m_invoiceInfo.InvBeginNo = 0;
    m_invoiceInfo.InvCount = 0;
    m_invoiceInfo.InvEndNO = 0;
    SaveInvInfo();
    return true;
}

SInvInfo DataFileMgr::GetInvInfo()
{
    return m_invoiceInfo;
}

bool DataFileMgr::IsLoaded()
{
    return m_cardBox.CardBoxID > 0 && m_cardBox.FlagCardID > 0;
}

bool DataFileMgr::ChangeStartNo(qint64 nInvBeginNo, int nInvCount)
{
    m_invoiceInfo.InvBeginNo = nInvBeginNo;
    m_invoiceInfo.InvCount = nInvCount;
    SaveInvInfo();
    return true;
}

bool DataFileMgr::LoadInvFromFile()
{
    QSqlQuery query("select * from " + m_fileInfo[emInvoice].sTableName, m_dataFile);
    if(!query.exec())
    {
        LogMsg("lane", tr("查询") + m_fileInfo[emInvoice].sTableName + tr("失败"));
        return false;
    }
    int recordCount = GetRecordCount(query);
    if(recordCount == 0)
    {
        LogMsg("lane", tr("票据记录为空"));
        return false;
    }
    if(recordCount >1 )
    {
        LogMsg("lane", tr("票据记录中数据多于一条,数据非法删除该数据"));
        query.exec("delete from " + m_fileInfo[emInvoice].sTableName);
        return true;
    }
    if(query.next())
    {
        int nLaneId = query.value(1).toInt();
        if(nLaneId != getLaneInfo()->getLaneCode())
        {
            LogMsg("lane", tr("票据记录中车道代码[%1]与本车道代码[%2]不符").arg(nLaneId).arg(getLaneInfo()->getLaneCode()));
            query.exec("delete from " + m_fileInfo[emInvoice].sTableName);
            return true;
        }
        if(getLaneInfo()->getLaneType() != LaneType_MtcOut)
        {
            LogMsg("lane", tr("本车不是MTC出口车道，不需要加载票据"));
            query.exec("delete from " + m_fileInfo[emInvoice].sTableName);
            return true;
        }

        m_invoiceInfo.InvBeginNo = query.value(2).toLongLong();
        m_invoiceInfo.InvCount = query.value(3).toUInt();
        m_invoiceInfo.InvEndNO = query.value(4).toLongLong();
        m_invoiceInfo.PreInvBeginNo = m_invoiceInfo.InvBeginNo;
        m_invoiceInfo.PreInvCount = m_invoiceInfo.InvCount;
        return true;
    }
    return false;
}

int DataFileMgr::GetRecordCount(QSqlQuery &query)
{
    int nCount = 0;
    while(query.next())
    {
        nCount++;
    }
    for(int i = 0; i < nCount; i++)
    {
        query.previous();
    }
    query.previous();
    return nCount;
}

bool DataFileMgr::SaveCardBoxInfo()
{
    QSqlQuery query("select * from " + m_fileInfo[emCardBox].sTableName, m_dataFile);
    if(!query.exec())
    {
        LogMsg("lane", tr("查询") + m_fileInfo[emCardBox].sTableName + tr("失败"));
        return false;
    }
    int recordCount = GetRecordCount(query);
    if(recordCount > 1)
    {
        LogMsg("lane", tr("卡盒记录中数据多于一条,数据非法删除该数据!"));
        query.exec("delete from " + m_fileInfo[emCardBox].sTableName);
    }
    if(recordCount <= 0)
    {
        LogMsg("lane", tr("插入新的卡盒记录:卡盒卡卡号(%1),卡盒号(%2),卡数(%3)")
               .arg(m_cardBox.FlagCardID).arg(m_cardBox.CardBoxID).arg(m_cardBox.nCardCount));
        QString sql = "insert into " + m_fileInfo[emCardBox].sTableName +
                         " (LaneID,FlagCardID,CardBoxID,CardBoxCount) VALUES (" +
                         tr("%1, %2, %3, %4);").arg(getLaneInfo()->getLaneCode())
                         .arg(m_cardBox.FlagCardID).arg(m_cardBox.CardBoxID).arg(m_cardBox.nCardCount);
        query.exec(sql);
        LogMsg("lane", sql);
    }
    if(recordCount == 1)
    {
        LogMsg("lane", tr("更新卡盒记录:卡盒卡卡号(%1),卡盒号(%2),卡数(%3)")
               .arg(m_cardBox.FlagCardID).arg(m_cardBox.CardBoxID).arg(m_cardBox.nCardCount));
        QString sql = "update " + m_fileInfo[emCardBox].sTableName +
                         tr(" set FlagCardID=%1,").arg(m_cardBox.FlagCardID) +
                         tr("CardBoxID=%1,").arg(m_cardBox.CardBoxID) +
                         tr("CardBoxCount=%1").arg(m_cardBox.nCardCount) +
                         " where ID = 1 and " +
                         tr("laneid=%1").arg(getLaneInfo()->getLaneCode());
        query.exec(sql);
        LogMsg("lane", sql);
    }
    return true;
}

bool DataFileMgr::LoadCardBoxFromFile()
{
    QSqlQuery query("select * from " + m_fileInfo[emCardBox].sTableName, m_dataFile);
    if(!query.exec())
    {
        LogMsg("lane", tr("查询") + m_fileInfo[emCardBox].sTableName + tr("失败"));
        return false;
    }
    int recordCount = GetRecordCount(query);
    if(recordCount == 0)
    {
        LogMsg("lane", tr("票据记录为空"));
        return false;
    }
    if(recordCount >1 )
    {
        LogMsg("lane", tr("票据记录中数据多于一条,数据非法删除该数据"));
        query.exec("delete from " + m_fileInfo[emCardBox].sTableName);
        return false;
    }
    if(query.next())
    {
        int nLaneId = query.value(1).toInt();
        if(nLaneId != getLaneInfo()->getLaneCode())
        {
            LogMsg("lane", tr("票据记录中车道代码[%1]与本车道代码[%2]不符").arg(nLaneId).arg(getLaneInfo()->getLaneCode()));
            query.exec("delete from " + m_fileInfo[emCardBox].sTableName);
            return true;
        }
        if(getLaneInfo()->getLaneType() != LaneType_MtcOut)
        {
            LogMsg("lane", tr("本车不是MTC出口车道，不需要加载票据"));
            query.exec("delete from " + m_fileInfo[emCardBox].sTableName);
            return true;
        }
        m_cardBox.CardBoxID = query.value(3).toUInt();
        m_cardBox.FlagCardID = query.value(2).toInt();
        m_cardBox.nCardCount = query.value(4).toUInt();
        return true;
    }
    return false;
}


DataFileMgr* getDataFileMgr()
{
    static DataFileMgr* dataMgr = NULL;
    if(dataMgr == NULL)
    {
        dataMgr = new DataFileMgr();
    }
    return dataMgr;
}

void DataFileMgr::UpdateWidgetData()
{
    //显示票号信息
    FormMTCTitle * pTitle = NULL;
    getWidgetByType(emFormTitle, (QWidget**)&pTitle);
    pTitle->setInvoice(m_invoiceInfo.InvBeginNo, m_invoiceInfo.InvCount);
    ::showWarning(tr("请核对票号，如有错误，请按【票号】键修改"),true);
}

void DataFileMgr::AddOneCard(quint32 dwCardId)
{
    m_cardBox.nCardCount++;
    SaveCardBoxInfo();
}

void DataFileMgr::DeleteOneCard(quint32 dwCardId)
{
    m_cardBox.nCardCount--;
    SaveCardBoxInfo();
}

bool DataFileMgr::LoadCardBox(quint32 nCardBoxNo, quint32 nFlagNo, quint16 nCount)
{
    m_cardBox.nCardCount = nCount;
    m_cardBox.CardBoxID = nCardBoxNo;
    m_cardBox.FlagCardID = nFlagNo;
    return SaveCardBoxInfo();
}

bool DataFileMgr::UnLoadCardBox(int nOperType)
{
    m_cardBox.nCardCount = 0;
    m_cardBox.CardBoxID = 0;
    m_cardBox.FlagCardID = 0;
    return SaveCardBoxInfo();
}
