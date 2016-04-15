#include "ftpclient.h"
#include <QEventLoop>
#include <QVector>
#include <QFile>
#include <QDebug>
FtpClient::FtpClient(QObject *parent) :
    QObject(parent), m_fileList(NULL), m_currentId(0), m_hasError(false), m_currentFile(NULL)
{
    connect(&m_ftp, SIGNAL(commandStarted(int)), this ,SLOT(executeStart(int)));
    connect(&m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(executeFinish(int,bool)));
    connect(&m_ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(receiveFileInfo(QUrlInfo)));
}

FtpClient::~FtpClient()
{
    this->close();
}

void FtpClient::executeStart(int id)
{
    if(id != m_currentId)
    {
        emit finished();
    }
}

void FtpClient::executeFinish(int id, bool error)
{
    if(id == m_currentId)
    {
        m_hasError = error;
        if(m_ftp.currentCommand() == QFtp::Get ||
                m_ftp.currentCommand() == QFtp::Put)
        {
            if(m_currentFile != NULL)
            {
                m_currentFile->close();
                delete m_currentFile;
                m_currentFile = NULL;
            }
        }
        emit finished();
    }
}

void FtpClient::receiveFileInfo(const QUrlInfo &i)
{
    if(m_fileList != NULL)
    {
        m_fileList->push_back(i);
    }
}

bool FtpClient::cd(const QString &dir)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.cd(dir);
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

bool FtpClient::list(QVector<QUrlInfo> *fileList, const QString &dir)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.list(dir);
        m_fileList = fileList;
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

bool FtpClient::get(const QString &fileName)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentFile = new QFile(fileName);
        if(m_currentFile->open(QIODevice::ReadWrite))
        {
            m_currentId = m_ftp.get(fileName,m_currentFile);
            loop.exec();
            ret = !m_hasError;
            clearInfo();
        }
    }
    return ret;
}

bool FtpClient::put(const QString &sourceFileName, const QString &destinationFileName)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentFile = new QFile(sourceFileName);
        if(m_currentFile->open(QIODevice::ReadWrite))
        {
            m_currentId = m_ftp.put(m_currentFile, "./" + destinationFileName);
            loop.exec();
            ret = !m_hasError;
            clearInfo();
        }
    }
    else
    {
        qDebug() << "is not log in" << m_ftp.state();
    }
    qDebug() << m_ftp.state();
    return ret;
}

bool FtpClient::connectToServer(const QString& ip, const QString &user, const QString &pwd)
{
    if(isConnected() || isLoggedIn())
    {
        m_ftp.close();
    }
    bool ret = false;
    QEventLoop loop;
    connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
    m_currentId = m_ftp.connectToHost(ip);
    loop.exec();
    ret = !m_hasError;
    clearInfo();
    //如果连接成功
    if(ret)
    {
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.login(user, pwd);
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

bool FtpClient::remove(const QString &fileName)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.remove(fileName);
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

bool FtpClient::mkdir(const QString &dir)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.mkdir(dir);
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

bool FtpClient::rmdir(const QString &dir)
{
    bool ret = false;
    if(isLoggedIn())
    {
        QEventLoop loop;
        connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
        m_currentId = m_ftp.rmdir(dir);
        loop.exec();
        ret = !m_hasError;
        clearInfo();
    }
    return ret;
}

void FtpClient::close()
{
    if(isConnected() || isLoggedIn())
    {
        m_ftp.close();
    }
}


bool FtpClient::isConnected()
{
    return m_ftp.state() == QFtp::Connected;
}

bool FtpClient::isLoggedIn()
{
    return m_ftp.state() == QFtp::LoggedIn;
}

QString FtpClient::error()
{
    return m_ftp.errorString();
}


FtpClient* getFtpClient()
{
    static FtpClient client;
    return &client;
}
