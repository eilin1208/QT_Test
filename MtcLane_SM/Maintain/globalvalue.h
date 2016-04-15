#ifndef GLOBALVALUE_H
#define GLOBALVALUE_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QVariant>
#include "globaltypedef.h"

class GlobalValue : public QMap<int, QVariant>
{
public:
    
signals:
    
public slots:
    
public:
    void setValue(int key, const QVariant& value)
    {
        QMutexLocker locker(&m_mutex);
        this->insert(key, value);
    }

    QVariant getValue(int key)
    {
        QMutexLocker locker(&m_mutex);
        return this->value(key);
    }
public:
    static GlobalValue* instance()
    {
        static GlobalValue value;
        return &value;
    }

private:
    GlobalValue(){}
private:
    //共享锁
    QMutex m_mutex;
};

inline QVariant getGlobalValue(int key)
{
    return GlobalValue::instance()->getValue(key);
}

inline void setGlobalValue(int key, const QVariant& value)
{
    return GlobalValue::instance()->setValue(key, value);
}

#endif // GLOBALVALUE_H
