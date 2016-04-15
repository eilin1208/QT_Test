#ifndef FDINTERFACE_H
#define FDINTERFACE_H

#include <QObject>
#include <QThread>
#include <QTextCodec>
class FDInterface : public QThread
{
    Q_OBJECT
public:
    explicit FDInterface(QObject *parent = 0);
    
signals:
    
public slots:
public:
    virtual int openDevImpl(char* lpszConnPara)=0;
    virtual int setModeImpl(int nLaneType, bool bTollByWeight)=0;
    virtual int closeDevImpl()=0;
    virtual int vehClassImpl(int nVehClass)=0;
    virtual int vehWeightImpl(int nTotalWeight, int nWeightLimit, int nOverweightRate)=0;
    virtual int fareImpl(int nFare)=0;
    virtual int balanceBeforeDeductImpl(int nBalanceBeforeDeduct, bool bIsTallyCard = false)=0;
    virtual int balanceAfterDeductImpl(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard = false)=0;
    virtual int entryStaImpl(const char* lpszEntrySta)=0;
    virtual int textImpl(char* lpszText, int nColor)=0;
    virtual int synDataImpl()=0;
    virtual int clearAllImpl()=0;
    virtual int brightnessImpl(int nBrightness)=0;
    virtual int alarmLightImpl(bool bStatus)=0;
    virtual int alarmSoundImpl(bool bStatus)=0;
    virtual int passLightImpl(bool bIsGreen)=0;

};
void setInstance(FDInterface* instance);
FDInterface* getInstance();

//UTF8 TO GB2312
inline QByteArray UTF8toGB2312(const QString& str)
{
    return QTextCodec::codecForName("GB2312")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str.toLocal8Bit().data()));
}
#endif // FDINTERFACE_H
