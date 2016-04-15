#include "xmlbakupwriter.h"
#include <QDateTime>
#include "CommonModule/ilogmsg.h"
#include <QSettings>
#include <QDir>
#include <QProcess>
const char* g_tmpBakUpDir = "/Program_Data/tmpBakUp/";
XMLBakupWriter::XMLBakupWriter(QObject *parent) :
    QObject(parent)
{
}

XMLBakupWriter::~XMLBakupWriter()
{
    //如果程序关闭时还有打开的文件，则关闭文件
    if(m_curFile.isOpen())
    {
        m_xmlWriter.writeEndElement();
        m_xmlWriter.writeEndDocument();
        m_curFile.flush();
        m_curFile.close();
    }
}

void XMLBakupWriter::bakup(TDataToSave &data)
{
    if(!checkFileOpen())
    {
        return;
    }
    //写入
    m_xmlWriter.writeStartElement("packet");
    m_xmlWriter.writeTextElement("OccurTime", QString::number(data.OccurTime));
    m_xmlWriter.writeTextElement("MsgID", QString::number(data.MsgID));
    m_xmlWriter.writeTextElement("MsgType", QString::number(data.MsgType));
    m_xmlWriter.writeTextElement("MsgLen", QString::number(data.nMsgLen));
    m_xmlWriter.writeTextElement("Data", QByteArray::fromRawData(data.pMsg, data.nMsgLen).toHex().data());
    m_xmlWriter.writeEndElement();
    //检查文件大小
    checkFileSize();

}

bool XMLBakupWriter::checkFileOpen()
{
    //判断文件夹是否建立
    QDir dir(g_tmpBakUpDir);
    if(!dir.exists())
    {
        if(!dir.mkpath(g_tmpBakUpDir))
        {
            return false;
        }
    }
    //判断文件是否打开
    if(!m_curFile.isOpen())
    {
        QString fileName = g_tmpBakUpDir + QDateTime::currentDateTime().toString("hh_mm_ss") + ".xml";
        LogMsg("lane", tr("文件已经关闭，打开新文件：[%1]").arg(fileName));
        m_curFile.setFileName(fileName);
        //打开文件
        if(!m_curFile.open(QIODevice::ReadWrite))
        {
            LogMsg("lane", tr("文件[%1]打开失败，原因为:[%2]").arg(fileName).arg(m_curFile.errorString()));
            return false;
        }
        else
        {
            m_xmlWriter.setDevice(&m_curFile);
            m_xmlWriter.setAutoFormatting(true);
            m_xmlWriter.writeStartDocument();
            m_xmlWriter.writeStartElement("data");
            m_curFile.flush();
        }
    }

    return true;
}

bool XMLBakupWriter::checkFileSize()
{
    //当文件大于5M的时候关闭文件
    if(m_curFile.size() > getBakupSize() * 1024 * 1024)
    {
        LogMsg("lane", tr("文件[%1]已经达到规定大小，关闭文件").arg(m_curFile.fileName()));
        m_xmlWriter.writeEndElement();
        m_xmlWriter.writeEndDocument();
        m_curFile.flush();
        m_curFile.close();

        return true;
    }

    return false;
}

int XMLBakupWriter::getBakupSize()
{
    QSettings setting("./Lane.ini", QSettings::IniFormat);
    return setting.value("Other/BakUpSize", 5).toInt();
}
