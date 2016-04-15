#ifndef CONFIGIMPL_H
#define CONFIGIMPL_H

#include "config.h"
//#include "db/sqlquery.h"
#include <QMutex>
#include <QSettings>

class AsyncConfigHandler;
class SqlHistoryModel;

class ConfigImpl : public Config
{
    Q_OBJECT

    friend class AsyncConfigHandler;

    public:
        virtual ~ConfigImpl();

        static ConfigImpl *getInstance();
        void init();
        void cleanUp();
        const QString& getConfigDir() const;
        QString getConfigFilePath() const;

        bool isMassSaving() const;
        void set(const QString& group, const QString& key, const QVariant& value);
        QVariant get(const QString& group, const QString& key);
        QHash<QString,QVariant> getAll();

    private:
        QString getConfigPath();
        QString getPortableConfigPath();
        void initDbFile();
        bool tryInitDbFile(const QPair<QString, bool>& dbPath);
        QVariant deserializeValue(const QVariant& value);

        static Config* instance;
        static qint64 sqlHistoryId;

        QSettings *m_pSetting = nullptr;
        //Db* db = nullptr;
        QString configDir;
        QString lastQueryError;
        bool massSaving = false;
        QString sqlite3Version;

        static ConfigImpl *ciInstance;

};
#define CFG ConfigImpl::getInstance()
#endif // CONFIGIMPL_H
