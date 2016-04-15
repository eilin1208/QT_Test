#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QFtp>
#include <QFile>
class FtpClient : public QObject
{
    Q_OBJECT
public:
    explicit FtpClient(QObject *parent = 0);
    ~FtpClient();
signals:
    void finished();
public slots:
    void executeStart(int id);
    void executeFinish(int id, bool error);
    void receiveFileInfo(const QUrlInfo& i);
public:
    bool cd(const QString& dir);
    bool list( QVector<QUrlInfo>* fileList,const QString& dir = QString());
    bool get(const QString& fileName);
    bool put(const QString &sourceFileName, const QString &destinationFileName);
    bool connectToServer(const QString& ip, const QString& user, const QString& pwd);
    bool remove(const QString& fileName);
    bool mkdir(const QString& dir);
    bool rmdir(const QString& dir);
    void close();
    bool isConnected();
    bool isLoggedIn();
    bool hasError()
    {
        return m_hasError;
    }
    QString error();
private:
    void clearInfo()
    {
        m_fileList = NULL;
        m_currentId = 0;
        m_hasError = false;
    }

private:
    QFtp m_ftp;
    QVector<QUrlInfo>* m_fileList;
    int m_currentId;
    bool m_hasError;
    QFile* m_currentFile;
};

FtpClient* getFtpClient();
#endif // FTPCLIENT_H
