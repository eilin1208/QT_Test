#include "bakupdatamgr.h"
#include <QDebug>
#include <QSettings>
#include <QDir>
#include "CommonModule/laneutils_new.h"
BakupDataMgr::BakupDataMgr(QObject *parent) :
    QObject(parent),m_pSaver(NULL), m_sDirName("/SDHC")
{
    init();
}

void BakupDataMgr::sdAdd(const QString &sDev)
{
    ////qDebug() << "removable device [" << sDev << "] add...";
    //当有设备接入时，应先判断是否是sd卡，在开发板中，
    //sd卡会被自动挂载为某个名称，这个名称固定，
    //但也不排目录名修改的可能，因此做成配置项
    //每次有新的设备接入时重新加载一遍挂载名称，可确保不重启程序，即可修改目录
    QSettings settings("./Lane.ini", QSettings::IniFormat);
    QString tmpStr = settings.value("Other/SDName", "").toString();
    if(!tmpStr.isEmpty())
    {
        m_sDirName = tmpStr;
    }
    ////qDebug() << "dir name [" << m_sDirName << "]";
    QTimer::singleShot(500, this, SLOT(timeOutToDetect()));
}

void BakupDataMgr::sdRemove(const QString &/*sDev*/)
{
    //暂时无用
}

void BakupDataMgr::timeOutToDetect()
{
    QDir dir(m_sDirName);
    if(dir.exists())
    {
        quint64 space = getDiskSpaceFree(m_sDirName);
        ////qDebug() << "space is " << space;
        space = space >> 20;           //转化为M
        //判断SD卡是否有空间备份
        if(space >= 500)
        {
            emit devAdd(SDOK);
        }
        else
        {
            emit devAdd(SDFULL);
        }
    }
    else
    {
        //qDebug() << "folder is not exists";
    }
}

void BakupDataMgr::setDataSaver(BaseSaver *pSaver)
{
    m_pSaver = pSaver;
}

void BakupDataMgr::startDetect()
{
    m_watcher.start();
}

void BakupDataMgr::stopDetect()
{
    m_watcher.stop();
}

bool BakupDataMgr::bakUp(int nType)
{
    if(m_pSaver != NULL)
    {
        if(nType == 0)
        {
            return bakUpAll();
        }
        else
        {
            return bakUpNoUpload();
        }
    }
    return false;
}

bool BakupDataMgr::bakUpAll()
{
    int dataSize = m_pSaver->GetTotalCount();
    QFile file(tr("%1/bak_%2.dat").arg(m_sDirName).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
    int nWritSize = 0;
    QByteArray * data = NULL;
    if(file.open(QIODevice::ReadWrite))
    {
        QList<QByteArray*> tmpData;
        const int BAKUPCOUNT = 100;
        for(int i = 0; i < dataSize; i = i + BAKUPCOUNT)
        {
            ////qDebug() << m_pSaver->GetDataByCount(tmpData, i, BAKUPCOUNT);
            for(int i = 0; i < tmpData.size(); i++)
            {
                data = tmpData[i];
                nWritSize = file.write(*data);
                if(nWritSize != data->size())
                {
                    emit bakUpFail();
                    file.close();
                    return false;
                }
                delete data;
            }
        }
        emit bakUpSuccess();
        file.close();
        return true;
    }
    else
    {
        emit bakUpFail();
        return false;
    }

}

bool BakupDataMgr::bakUpNoUpload()
{
    ////qDebug() << "备份部分数据成功";
    emit bakUpSuccess();
    return true;
}

void BakupDataMgr::init()
{
    //连接信号槽
    connect(&m_watcher, SIGNAL(deviceAdded(QString)), this, SLOT(sdAdd(QString)));
    connect(&m_watcher, SIGNAL(deviceRemoved(QString)), this, SLOT(sdRemove(QString)));
}

void BakupDataMgr::clear()
{
    m_sDirName.clear();
}
