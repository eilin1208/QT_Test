#include "configimpl.h"
#include "common/notifymanager.h"
#include "common/utils.h"
//#include "db/dbsqlite3.h"
#include <QtGlobal>
#include <QDebug>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QDataStream>
#include <QRegExp>
#include <QDateTime>
#include <QSysInfo>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrentRun>

static_qstring(DB_FILE_NAME, "settings3");
qint64 ConfigImpl::sqlHistoryId = -1;
ConfigImpl* ConfigImpl::ciInstance=nullptr;

ConfigImpl::~ConfigImpl()
{
    cleanUp();
}

ConfigImpl *ConfigImpl::getInstance()
{
    if(!ciInstance)
        ciInstance = new ConfigImpl();

    return ciInstance;
}

void ConfigImpl::init()
{
    initDbFile();
}

void ConfigImpl::cleanUp()
{
    safe_delete(m_pSetting);
}

const QString &ConfigImpl::getConfigDir() const
{
    return configDir;
}

QString ConfigImpl::getConfigFilePath() const
{
//    if (!db)
//        return QString();

//    return db->getPath();
}

bool ConfigImpl::isMassSaving() const
{
    return massSaving;
}

void ConfigImpl::set(const QString &group, const QString &key, const QVariant &value)
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << value;

    QString tmpKey = tr("%1/%2").arg(group).arg(key);

    m_pSetting->setValue(tmpKey, bytes);
}

QVariant ConfigImpl::get(const QString &group, const QString &key)
{
    return m_pSetting->value(tr("%1/%2").arg(group).arg(key));
}

QHash<QString,QVariant> ConfigImpl::getAll()
{
    QStringList GroupList = m_pSetting->allKeys();

    QHash<QString,QVariant> cfg;
    QString key;
    foreach (key, GroupList) {
        cfg[key] = m_pSetting->value(key);
    }

    return cfg;
}

QString ConfigImpl::getConfigPath()
{
#ifdef Q_OS_WIN
    return "./OPETST";
#else
    return "./.config/OPETST";
#endif
//#ifdef Q_OS_WIN
//    if (QSysInfo::windowsVersion() & QSysInfo::WV_NT_based)
//        return SQLITESTUDIO->getEnv("APPDATA")+"/sqlitestudio";
//    else
//        return SQLITESTUDIO->getEnv("HOME")+"/sqlitestudio";
//#else
//    return SQLITESTUDIO->getEnv("HOME")+"/.config/sqlitestudio";
//#endif
}

QString ConfigImpl::getPortableConfigPath()
{
    QStringList paths = QStringList({"./sqlitestudio-cfg", qApp->applicationDirPath() + "/sqlitestudio-cfg"});
    QSet<QString> pathSet;
    QDir dir;
    for (const QString& path : paths)
    {
        dir = QDir(path);
        pathSet << dir.absolutePath();
    }

    QString potentialPath;
    QFileInfo file;
    for (const QString& path : pathSet)
    {
        dir = QDir(path);
        file = QFileInfo(dir.absolutePath());
        if (!file.exists())
        {
            if (potentialPath.isNull())
                potentialPath = dir.absolutePath();

            continue;
        }

        if (!file.isDir() || !file.isReadable() || !file.isWritable())
            continue;

        foreach (file, dir.entryInfoList())
        {
            if (!file.isReadable() || !file.isWritable())
                continue;
        }

        return dir.absolutePath();
    }

    return potentialPath;
}


void ConfigImpl::initDbFile()
{
    // Determinate global config location and portable one
    QString globalPath = getConfigPath();
    QString portablePath = getPortableConfigPath();

    QList<QPair<QString,bool>> paths;
    if (!globalPath.isNull() && !portablePath.isNull())
    {
        if (QFileInfo(portablePath).exists())
        {
            paths << QPair<QString,bool>(portablePath+"/"+DB_FILE_NAME, false);
            paths << QPair<QString,bool>(globalPath+"/"+DB_FILE_NAME, true);
        }
        else
        {
            paths << QPair<QString,bool>(globalPath+"/"+DB_FILE_NAME, true);
            paths << QPair<QString,bool>(portablePath+"/"+DB_FILE_NAME, false);
        }
    }
    else if (!globalPath.isNull())
    {
        paths << QPair<QString,bool>(globalPath+"/"+DB_FILE_NAME, true);
    }
    else if (!portablePath.isNull())
    {
        paths << QPair<QString,bool>(portablePath+"/"+DB_FILE_NAME, false);
    }

    // Go through all candidates and pick one
    QDir dir;
    for (const QPair<QString,bool>& path : paths)
    {
        dir = QDir(path.first);
        dir.cdUp();
        if (tryInitDbFile(path))
        {
            configDir = dir.absolutePath();
            break;
        }
    }

    qDebug() << "Using configuration directory:" << configDir;
}

bool ConfigImpl::tryInitDbFile(const QPair<QString, bool> &dbPath)
{
    // Create global config directory if not existing
    if (dbPath.second && !dbPath.first.isNull())
    {
        QDir dir(dbPath.first.mid(0, dbPath.first.length() - DB_FILE_NAME.length() - 1));
        if (!dir.exists())
            QDir::root().mkpath(dir.absolutePath());
    }

    m_pSetting = new QSettings(dbPath.first, QSettings::IniFormat);

    return true;
}


QVariant ConfigImpl::deserializeValue(const QVariant &value)
{
    if (!value.isValid())
        return QVariant();

    QByteArray bytes = value.toByteArray();
    if (bytes.isNull())
        return QVariant();

    QVariant deserializedValue;
    QDataStream stream(bytes);
    stream >> deserializedValue;
    return deserializedValue;
}
