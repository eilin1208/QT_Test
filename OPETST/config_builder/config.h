#ifndef CONFIG_H
#define CONFIG_H

#include "config_builder.h"
#include "common/functionmanager.h"
#include "common/collationmanager.h"
//#include "common/utils.h"
#include <QObject>
#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QSharedPointer>
#include <QDateTime>

const int SQLITESTUDIO_CONFIG_VERSION = 1;

CFG_CATEGORIES(Core,
    CFG_CATEGORY(General,
        CFG_ENTRY(int,          SqlHistorySize,          10000)
        CFG_ENTRY(int,          DdlHistorySize,          1000)
        CFG_ENTRY(QString,      LoadedPlugins,           "")
        CFG_ENTRY(QVariantHash, ActiveCodeFormatter,     QVariantHash())
        CFG_ENTRY(bool,         CheckUpdatesOnStartup,   true)
        CFG_ENTRY(QString,      Language,                "en")
    )
    CFG_CATEGORY(Console,
        CFG_ENTRY(int,          HistorySize,             100)
    )
    CFG_CATEGORY(Internal,
        CFG_ENTRY(QVariantList, Functions,               QVariantList())
        CFG_ENTRY(QVariantList, Collations,              QVariantList())
        CFG_ENTRY(QString,      BugReportUser,           QString())
        CFG_ENTRY(QString,      BugReportPassword,       QString())
        CFG_ENTRY(QString,      BugReportRecentTitle,    QString())
        CFG_ENTRY(QString,      BugReportRecentContents, QString())
        CFG_ENTRY(bool,         BugReportRecentError,    false)
    )
)

#define CFG_CORE CFG_INSTANCE(Core)

class QAbstractItemModel;

class Config : public QObject
{
    Q_OBJECT

    public:
        virtual ~Config();

        struct CfgDb
        {
            QString name;
            QString path;
            QHash<QString,QVariant> options;
        };

        typedef QSharedPointer<CfgDb> CfgDbPtr;

        struct DbGroup;
        typedef QSharedPointer<DbGroup> DbGroupPtr;

        struct DbGroup
        {
            qint64 id;
            QString referencedDbName;
            QString name;
            QList<DbGroupPtr> childs;
            int order;
            bool open = false;
        };

        struct SqlHistoryEntry
        {
            QString query;
            QString dbName;
            int rowsAffected;
            int unixtime;
        };

        typedef QSharedPointer<SqlHistoryEntry> SqlHistoryEntryPtr;

        struct DdlHistoryEntry
        {
            QString dbName;
            QString dbFile;
            QDateTime timestamp;
            QString queries;
        };

        typedef QSharedPointer<DdlHistoryEntry> DdlHistoryEntryPtr;

        struct ReportHistoryEntry
        {
            int id = 0;
            bool isFeatureRequest = false;
            int timestamp = 0;
            QString title;
            QString url;
        };

        typedef QSharedPointer<ReportHistoryEntry> ReportHistoryEntryPtr;

        virtual void init() = 0;
        virtual void cleanUp() = 0;
        virtual const QString& getConfigDir() const = 0;
        virtual QString getConfigFilePath() const = 0;

        virtual bool isMassSaving() const = 0;
        virtual void set(const QString& group, const QString& key, const QVariant& value) = 0;
        virtual QVariant get(const QString& group, const QString& key) = 0;
        virtual QHash<QString,QVariant> getAll() = 0;
    signals:
        void massSaveBegins();
        void massSaveCommited();
        void sqlHistoryRefreshNeeded();
        void ddlHistoryRefreshNeeded();
        void reportsHistoryRefreshNeeded();
};

#endif // CONFIG_H
