#ifndef ILOGMSG_H
#define ILOGMSG_H

#include <QObject>
#include "log4qt/logger.h"


class ILogMsg : public QObject
{
    Q_OBJECT
public:
    explicit ILogMsg(QObject *parent = 0);

    void loadConfig(const QString &sFileName);
signals:

public slots:
};


Log4Qt::Logger *LogFmtMsg();


#endif // ILOGMSG_H
