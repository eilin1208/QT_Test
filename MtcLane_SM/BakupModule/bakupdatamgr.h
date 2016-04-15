#ifndef BAKUPDATAMGR_H
#define BAKUPDATAMGR_H

#include <QObject>
#include "CommonModule/basesaver.h"
#include "SDDetector/qdevicewatcher.h"
enum SDSTATUS
{
    SDOK,       //SD卡正常
    SDFULL      //SD卡已满
};

class BakupDataMgr : public QObject
{
    Q_OBJECT
public:
    explicit BakupDataMgr(QObject *parent = 0);
    
signals:
    void devAdd(int status);
    void bakUpSuccess();
    void bakUpFail();
public slots:
    void sdAdd(const QString& sDev);
    void sdRemove(const QString& sDev);
    void timeOutToDetect();
public:
    void setDataSaver(BaseSaver* pSaver);
    void startDetect();
    void stopDetect();
    bool bakUp(int nType);
protected:
    bool bakUpAll();
    bool bakUpNoUpload();
private:
    void init();
    void clear();
private:
    BaseSaver* m_pSaver;
    QDeviceWatcher m_watcher;
    QString m_sDirName;
};

#endif // BAKUPDATAMGR_H
