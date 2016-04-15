#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include "common/global.h"
#include <QStringList>
#include <QObject>

class NotifyManager : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON(NotifyManager)

    public:
        explicit NotifyManager(QObject *parent = 0);

        QList<QString> getRecentErrors() const;
        QList<QString> getRecentWarnings() const;
        QList<QString> getRecentInfos() const;

    signals:
        void notifyError(const QString& msg);
        void notifyWarning(const QString& msg);
        void notifyInfo(const QString& msg);

    public slots:
        void error(const QString& msg);
        void warn(const QString& msg);
        void info(const QString& msg);

    private:
        void addToRecentList(QStringList& list, const QString& message);

        static const constexpr int maxRecentMessages = 10;

        QStringList recentErrors;
        QStringList recentWarnings;
        QStringList recentInfos;
};

#define NOTIFY_MANAGER NotifyManager::getInstance()

void notifyError(const QString& msg);
void notifyWarn(const QString& msg);
void notifyInfo(const QString& msg);

#endif // NOTIFYMANAGER_H
