#ifndef FDUC_H
#define FDUC_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include "fdinterface.h"
#include "QtSerialPort/QSerialPort"
using namespace TNX;

class FDUC : public FDInterface
{
    Q_OBJECT
public:
    explicit FDUC(QObject *parent = 0);
    
signals:
    
public slots:
public:
    virtual int openDevImpl(char* lpszConnPara);
    virtual int setModeImpl(int nLaneType, bool bTollByWeight);
    virtual int closeDevImpl();
    virtual int vehClassImpl(int nVehClass);
    virtual int vehWeightImpl(int nTotalWeight, int nWeightLimit, int nOverweightRate);
    virtual int fareImpl(int nFare);
    virtual int balanceBeforeDeductImpl(int nBalanceBeforeDeduct, bool bIsTallyCard = false);
    virtual int balanceAfterDeductImpl(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard = false);
    virtual int entryStaImpl(const char* lpszEntrySta);
    virtual int textImpl(char* lpszText, int nColor);
    virtual int synDataImpl();
    virtual int clearAllImpl();
    virtual int brightnessImpl(int nBrightness);
    virtual int alarmLightImpl(bool bStatus);
    virtual int alarmSoundImpl(bool bStatus);
    virtual int passLightImpl(bool bIsGreen);
protected:
    void run();
private:
    void createLine(QByteArray& text, char row);
    void createCommand(const QByteArray& content);
    //创建客车播报语音命令
    void createFareCommand(int fare);
    //创建清除语音命令
    void createCleanVoiceCommand();
    //创建抬杆放行语音命令
    QByteArray createPassCommand();
private:
    QSerialPort* m_serialPort;
    QQueue<QByteArray> m_data;
    int m_laneType;
    int m_tollByWeight;
    QMutex m_mutex;

};

#endif // FDUC_H
