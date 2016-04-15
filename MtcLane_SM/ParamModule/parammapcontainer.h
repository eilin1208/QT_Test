#ifndef PARAMMAPCONTAINER_H
#define PARAMMAPCONTAINER_H

#include <QMap>
#include <QMutex>
#include <QMutexLocker>
class AbstractParamContainer
{
public:
    virtual bool load_begin()=0;
    virtual bool load_cancel()=0;
    virtual bool load_complete()=0;
};

template<class T_Key, class T_Value>
class ParamMapContainer : public AbstractParamContainer
{
public:
    virtual bool load_begin()
    {
        QMutexLocker locker(&m_mutex);
        m_mapTemp.clear();
        return true;
    }

    virtual bool load_cancel()
    {
        QMutexLocker locker(&m_mutex);
        m_mapTemp.clear();
        return true;
    }

    virtual bool load_complete()
    {
        QMutexLocker locker(&m_mutex);
        qSwap(m_mapMain,m_mapTemp);
        m_mapTemp.clear();
        return true;
    }

    bool addItemToTemp(const T_Key &key, const T_Value &value)
    {
        QMutexLocker locker(&m_mutex);
        m_mapTemp[key]=value;
        return true;
    }

    bool addItemToTemp_multi(const T_Key &key, const T_Value &value)
    {
        QMutexLocker locker(&m_mutex);
        m_mapTemp.insertMulti(key, value);
        return true;
    }

    const T_Value getValue(const T_Key &key) const
    {
        return m_mapMain.value(key);
    }

    const QList<T_Value> getValues(const T_Key &key) const
    {
        return m_mapMain.values(key);
    }

    bool findValue(const T_Key &key, T_Value &value) const
    {
        if (m_mapMain.find(key)!=m_mapMain.end())
        {
            value=m_mapMain.value(key);
            return true;
        }
        return false;
    }

    const QMap<T_Key, T_Value> & getMap() const
    {
        return m_mapMain;
    }

public:
    ParamMapContainer()
    {
    }

protected:
    QMap<T_Key, T_Value> m_mapMain;
    QMap<T_Key, T_Value> m_mapTemp;
    QMutex m_mutex;
};

#endif // PARAMMAPCONTAINER_H
